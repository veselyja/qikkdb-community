//
// Created by Martin Staňo on 2019-01-15.
//

#include "GpuSqlDispatcher.h"
#include "../QueryEngine/Context.h"
#include "../Types/ComplexPolygon.pb.h"
#include "../Types/Point.pb.h"
#include "ParserExceptions.h"
#include "../QueryEngine/Context.h"
#include "../QueryEngine/GPUCore/GPUMemory.cuh"
#include "../ComplexPolygonFactory.h"
#include "../StringFactory.h"
#include "../Database.h"
#include "../Table.h"
#include <any>
#include <string>
#include <unordered_map>
#include "InsertIntoStruct.h"

const std::string GpuSqlDispatcher::KEYS_SUFFIX = "_keys";
const std::string GpuSqlDispatcher::NULL_SUFFIX = "_nullMask";

int32_t GpuSqlDispatcher::groupByDoneCounter_ = 0;
int32_t GpuSqlDispatcher::orderByDoneCounter_ = 0;

std::mutex GpuSqlDispatcher::groupByMutex_;
std::mutex GpuSqlDispatcher::orderByMutex_;

std::condition_variable GpuSqlDispatcher::groupByCV_;
std::condition_variable GpuSqlDispatcher::orderByCV_;

int32_t GpuSqlDispatcher::deviceCountLimit_;
std::unordered_map<std::string, int32_t> GpuSqlDispatcher::linkTable;

#ifndef NDEBUG
void AssertDeviceMatchesCurrentThread(int dispatcherThreadId)
{
	int device = -1;
	cudaGetDevice(&device);
	std::cout << "Current device for tid " << dispatcherThreadId << " is " << device << "\n";
	if(device != dispatcherThreadId)
	{
		abort();
}
}
#endif

GpuSqlDispatcher::GpuSqlDispatcher(const std::shared_ptr<Database> &database, std::vector<std::unique_ptr<IGroupBy>>& groupByTables, std::vector<OrderByBlocks>& orderByBlocks, int dispatcherThreadId) :
	database(database),
	blockIndex(dispatcherThreadId),
	instructionPointer(0),
	constPointCounter(0),
	constPolygonCounter(0),
	constStringCounter(0),
	filter_(0),
	usedRegisterMemory(0),
	maxRegisterMemory(0), // TODO value from config e.g.
	groupByTables(groupByTables),
	dispatcherThreadId(dispatcherThreadId),
	usingGroupBy(false),
	usingOrderBy(false),
	usingJoin(false),
	isLastBlockOfDevice(false),
	isOverallLastBlock(false),
	noLoad(true),
	loadNecessary(1),
	cpuDispatcher(database),
	jmpInstuctionPosition(0),
	insertIntoData(std::make_unique<InsertIntoStruct>()),
	joinIndices(nullptr),
	orderByTable(nullptr),
	orderByBlocks(orderByBlocks)
{
}

GpuSqlDispatcher::~GpuSqlDispatcher()
{
}


void GpuSqlDispatcher::copyExecutionDataTo(GpuSqlDispatcher & other, CpuSqlDispatcher & sourceCpuDispatcher)
{
	other.dispatcherFunctions = dispatcherFunctions;
	other.arguments = arguments;
	other.jmpInstuctionPosition = jmpInstuctionPosition;
	sourceCpuDispatcher.copyExecutionDataTo(other.cpuDispatcher);
}

void GpuSqlDispatcher::setJoinIndices(std::unordered_map<std::string, std::vector<std::vector<int32_t>>>* joinIdx)
{
	if (!joinIdx->empty())
	{
		joinIndices = joinIdx;
		usingJoin = true;
	}
}

/// Main execution loop of dispatcher
/// Iterates through all dispatcher functions in the operations array (filled from GpuSqlListener) and executes them
/// until running out of blocks
/// <param name="result">Response message to the SQL statement</param>
void GpuSqlDispatcher::execute(std::unique_ptr<google::protobuf::Message>& result, std::exception_ptr& exception)
{
	try
	{
		Context& context = Context::getInstance();
		context.getCacheForCurrentDevice().setCurrentBlockIndex(blockIndex);
		context.bindDeviceToContext(dispatcherThreadId);
		context.getCacheForCurrentDevice().setCurrentBlockIndex(blockIndex);
		int32_t err = 0;

		while (err == 0)
		{

			err = (this->*dispatcherFunctions[instructionPointer++])();
#ifndef NDEBUG
			printf("tid:%d ip: %d \n", dispatcherThreadId, instructionPointer - 1);
			AssertDeviceMatchesCurrentThread(dispatcherThreadId);
#endif
			if (err)
			{
				if (err == 1)
				{
					std::cout << "Out of blocks." << std::endl;
				}
				if (err == 2)
				{
					std::cout << "Show databases completed sucessfully" << std::endl;
				}
				if (err == 3)
				{
					std::cout << "Show tables completed sucessfully" << std::endl;
				}
				if (err == 4)
				{
					std::cout << "Show columns completed sucessfully" << std::endl;
				}
				if (err == 5)
				{
					std::cout << "Insert into completed sucessfully" << std::endl;
				}
				if (err == 6)
				{
					std::cout << "Create database completed sucessfully" << std::endl;
				}
				if (err == 7)
				{
					std::cout << "Drop database completed sucessfully" << std::endl;
				}
				if (err == 8)
				{
					std::cout << "Create table completed sucessfully" << std::endl;				
				}
				if (err == 9)
				{
					std::cout << "Drop table completed sucessfully" << std::endl;
				}
				if (err == 10)
				{
					std::cout << "Alter table completed sucessfully" << std::endl;
				}
				if (err == 11)
				{
					std::cout << "Create index completed sucessfully" << std::endl;
				}
				if (err == 12)
				{
					std::cout << "Load skipped" << std::endl;
					err = 0;
					continue;
				}
				break;
			}
		}
		result = std::make_unique<ColmnarDB::NetworkClient::Message::QueryResponseMessage>(std::move(responseMessage));
	}
	catch (...)
	{
		exception = std::current_exception();
	}
	cleanUpGpuPointers();
}

const ColmnarDB::NetworkClient::Message::QueryResponseMessage &GpuSqlDispatcher::getQueryResponseMessage()
{
	return responseMessage;
}

void GpuSqlDispatcher::addRetFunction(DataType type)
{
    dispatcherFunctions.push_back(retFunctions[type]);
}

void GpuSqlDispatcher::addOrderByFunction(DataType type)
{
	dispatcherFunctions.push_back(orderByFunctions[type]);
}

void GpuSqlDispatcher::addOrderByReconstructOrderFunction(DataType type)
{
	dispatcherFunctions.push_back(orderByReconstructOrderFunctions[type]);
}

void GpuSqlDispatcher::addOrderByReconstructRetFunction(DataType type)
{
	dispatcherFunctions.push_back(orderByReconstructRetFunctions[type]);
}

void GpuSqlDispatcher::addFreeOrderByTableFunction()
{
	dispatcherFunctions.push_back(freeOrderByTableFunction);
}

void GpuSqlDispatcher::addOrderByReconstructRetAllBlocksFunction()
{
	dispatcherFunctions.push_back(orderByReconstructRetAllBlocksFunction);
}

void GpuSqlDispatcher::addLockRegisterFunction()
{
	dispatcherFunctions.push_back(lockRegisterFunction);
}

void GpuSqlDispatcher::addFilFunction()
{
    dispatcherFunctions.push_back(filFunction);
}

void GpuSqlDispatcher::addWhereEvaluationFunction()
{
	dispatcherFunctions.push_back(whereEvaluationFunction);
}

void GpuSqlDispatcher::addJmpInstruction()
{
	dispatcherFunctions.push_back(jmpFunction);
	jmpInstuctionPosition = dispatcherFunctions.size() - 1;
}

void GpuSqlDispatcher::addDoneFunction()
{
    dispatcherFunctions.push_back(doneFunction);
}

void GpuSqlDispatcher::addShowDatabasesFunction()
{
	dispatcherFunctions.push_back(showDatabasesFunction);
}

void GpuSqlDispatcher::addShowTablesFunction()
{
	dispatcherFunctions.push_back(showTablesFunction);
}

void GpuSqlDispatcher::addShowColumnsFunction()
{
	dispatcherFunctions.push_back(showColumnsFunction);
}

void GpuSqlDispatcher::addCreateDatabaseFunction()
{
	dispatcherFunctions.push_back(createDatabaseFunction);
}

void GpuSqlDispatcher::addDropDatabaseFunction()
{
	dispatcherFunctions.push_back(dropDatabaseFunction);
}

void GpuSqlDispatcher::addCreateTableFunction()
{
	dispatcherFunctions.push_back(createTableFunction);
}

void GpuSqlDispatcher::addDropTableFunction()
{
	dispatcherFunctions.push_back(dropTableFunction);
}

void GpuSqlDispatcher::addAlterTableFunction()
{
	dispatcherFunctions.push_back(alterTableFunction);
}

void GpuSqlDispatcher::addCreateIndexFunction()
{
	dispatcherFunctions.push_back(createIndexFunction);
}

void GpuSqlDispatcher::addInsertIntoFunction(DataType type)
{
	dispatcherFunctions.push_back(insertIntoFunctions[type]);
}

void GpuSqlDispatcher::addInsertIntoDoneFunction()
{
	dispatcherFunctions.push_back(insertIntoDoneFunction);
}

void GpuSqlDispatcher::addGreaterFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(greaterFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addLessFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(lessFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addGreaterEqualFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(greaterEqualFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addLessEqualFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(lessEqualFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addEqualFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(equalFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addNotEqualFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(notEqualFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addLogicalAndFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(logicalAndFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addLogicalOrFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(logicalOrFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addMulFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(mulFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addDivFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(divFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addAddFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(addFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}


void GpuSqlDispatcher::addSubFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(subFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addModFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(modFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addBitwiseOrFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(bitwiseOrFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addBitwiseAndFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(bitwiseAndFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addBitwiseXorFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(bitwiseXorFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addBitwiseLeftShiftFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(bitwiseLeftShiftFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addBitwiseRightShiftFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(bitwiseRightShiftFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addPointFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(pointFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addLogarithmFunction(DataType number, DataType base)
{
	dispatcherFunctions.push_back(logarithmFunctions[DataType::DATA_TYPE_SIZE * number + base]);
}

void GpuSqlDispatcher::addArctangent2Function(DataType y, DataType x)
{
	dispatcherFunctions.push_back(arctangent2Functions[DataType::DATA_TYPE_SIZE * y + x]);
}

void GpuSqlDispatcher::addConcatFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(concatFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addLeftFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(leftFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addRightFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(rightFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addPowerFunction(DataType base, DataType exponent)
{
	dispatcherFunctions.push_back(powerFunctions[DataType::DATA_TYPE_SIZE * base + exponent]);
}

void GpuSqlDispatcher::addRootFunction(DataType base, DataType exponent)
{
	dispatcherFunctions.push_back(rootFunctions[DataType::DATA_TYPE_SIZE * base + exponent]);
}

void GpuSqlDispatcher::addContainsFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(containsFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addIntersectFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(intersectFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addUnionFunction(DataType left, DataType right)
{
    dispatcherFunctions.push_back(unionFunctions[DataType::DATA_TYPE_SIZE * left + right]);
}

void GpuSqlDispatcher::addLogicalNotFunction(DataType type)
{
    dispatcherFunctions.push_back(logicalNotFunctions[type]);
}

void GpuSqlDispatcher::addIsNullFunction()
{
	dispatcherFunctions.push_back(isNullFunction);
}

void GpuSqlDispatcher::addIsNotNullFunction()
{
	dispatcherFunctions.push_back(isNotNullFunction);
}

void GpuSqlDispatcher::addMinusFunction(DataType type)
{
    dispatcherFunctions.push_back(minusFunctions[type]);
}

void GpuSqlDispatcher::addYearFunction(DataType type)
{
	dispatcherFunctions.push_back(yearFunctions[type]);
}

void GpuSqlDispatcher::addMonthFunction(DataType type)
{
	dispatcherFunctions.push_back(monthFunctions[type]);
}

void GpuSqlDispatcher::addDayFunction(DataType type)
{
	dispatcherFunctions.push_back(dayFunctions[type]);
}

void GpuSqlDispatcher::addHourFunction(DataType type)
{
	dispatcherFunctions.push_back(hourFunctions[type]);
}

void GpuSqlDispatcher::addMinuteFunction(DataType type)
{
	dispatcherFunctions.push_back(minuteFunctions[type]);
}

void GpuSqlDispatcher::addSecondFunction(DataType type)
{
	dispatcherFunctions.push_back(secondFunctions[type]);
}

void GpuSqlDispatcher::addAbsoluteFunction(DataType type)
{
	dispatcherFunctions.push_back(absoluteFunctions[type]);
}

void GpuSqlDispatcher::addSineFunction(DataType type)
{
	dispatcherFunctions.push_back(sineFunctions[type]);
}

void GpuSqlDispatcher::addCosineFunction(DataType type)
{
	dispatcherFunctions.push_back(cosineFunctions[type]);
}

void GpuSqlDispatcher::addTangentFunction(DataType type)
{
	dispatcherFunctions.push_back(tangentFunctions[type]);
}

void GpuSqlDispatcher::addCotangentFunction(DataType type)
{
	dispatcherFunctions.push_back(cotangentFunctions[type]);
}

void GpuSqlDispatcher::addArcsineFunction(DataType type)
{
	dispatcherFunctions.push_back(arcsineFunctions[type]);
}

void GpuSqlDispatcher::addArccosineFunction(DataType type)
{
	dispatcherFunctions.push_back(arccosineFunctions[type]);
}

void GpuSqlDispatcher::addArctangentFunction(DataType type)
{
	dispatcherFunctions.push_back(arctangentFunctions[type]);
}

void GpuSqlDispatcher::addLogarithm10Function(DataType type)
{
	dispatcherFunctions.push_back(logarithm10Functions[type]);
}

void GpuSqlDispatcher::addLogarithmNaturalFunction(DataType type)
{
	dispatcherFunctions.push_back(logarithmNaturalFunctions[type]);
}

void GpuSqlDispatcher::addExponentialFunction(DataType type)
{
	dispatcherFunctions.push_back(exponentialFunctions[type]);
}

void GpuSqlDispatcher::addSquareFunction(DataType type)
{
	dispatcherFunctions.push_back(squareFunctions[type]);
}

void GpuSqlDispatcher::addSquareRootFunction(DataType type)
{
	dispatcherFunctions.push_back(squareRootFunctions[type]);
}

void GpuSqlDispatcher::addSignFunction(DataType type)
{
	dispatcherFunctions.push_back(signFunctions[type]);
}

void GpuSqlDispatcher::addRoundFunction(DataType type)
{
	dispatcherFunctions.push_back(roundFunctions[type]);
}

void GpuSqlDispatcher::addFloorFunction(DataType type)
{
	dispatcherFunctions.push_back(floorFunctions[type]);
}

void GpuSqlDispatcher::addCeilFunction(DataType type)
{
	dispatcherFunctions.push_back(ceilFunctions[type]);
}

void GpuSqlDispatcher::addLtrimFunction(DataType type)
{
	dispatcherFunctions.push_back(ltrimFunctions[type]);
}

void GpuSqlDispatcher::addRtrimFunction(DataType type)
{
	dispatcherFunctions.push_back(rtrimFunctions[type]);
}

void GpuSqlDispatcher::addLowerFunction(DataType type)
{
	dispatcherFunctions.push_back(lowerFunctions[type]);
}

void GpuSqlDispatcher::addUpperFunction(DataType type)
{
	dispatcherFunctions.push_back(upperFunctions[type]);
}

void GpuSqlDispatcher::addReverseFunction(DataType type)
{
	dispatcherFunctions.push_back(reverseFunctions[type]);
}

void GpuSqlDispatcher::addLenFunction(DataType type)
{
	dispatcherFunctions.push_back(lenFunctions[type]);
}

void GpuSqlDispatcher::addMinFunction(DataType key, DataType value, GroupByType groupByType)
{
	GpuSqlDispatcher::DispatchFunction fun;
	switch (groupByType)
	{
	case GroupByType::NO_GROUP_BY:
		fun = minAggregationFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::SINGLE_KEY_GROUP_BY:
		fun = minGroupByFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::MULTI_KEY_GROUP_BY:
		fun = minGroupByMultiKeyFunctions[value];
		break;
	default:
		break;
	}
    dispatcherFunctions.push_back(fun);
}

void GpuSqlDispatcher::addMaxFunction(DataType key, DataType value, GroupByType groupByType)
{
	GpuSqlDispatcher::DispatchFunction fun;
	switch (groupByType)
	{
	case GroupByType::NO_GROUP_BY:
		fun = maxAggregationFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::SINGLE_KEY_GROUP_BY:
		fun = maxGroupByFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::MULTI_KEY_GROUP_BY:
		fun = maxGroupByMultiKeyFunctions[value];
		break;
	default:
		break;
	}
	dispatcherFunctions.push_back(fun);
}

void GpuSqlDispatcher::addSumFunction(DataType key, DataType value, GroupByType groupByType)
{
	GpuSqlDispatcher::DispatchFunction fun;
	switch (groupByType)
	{
	case GroupByType::NO_GROUP_BY:
		fun = sumAggregationFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::SINGLE_KEY_GROUP_BY:
		fun = sumGroupByFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::MULTI_KEY_GROUP_BY:
		fun = sumGroupByMultiKeyFunctions[value];
		break;
	default:
		break;
	}
	dispatcherFunctions.push_back(fun);
}

void GpuSqlDispatcher::addCountFunction(DataType key, DataType value, GroupByType groupByType)
{
	GpuSqlDispatcher::DispatchFunction fun;
	switch (groupByType)
	{
	case GroupByType::NO_GROUP_BY:
		fun = countAggregationFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::SINGLE_KEY_GROUP_BY:
		fun = countGroupByFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::MULTI_KEY_GROUP_BY:
		fun = countGroupByMultiKeyFunctions[value];
		break;
	default:
		break;
	}
	dispatcherFunctions.push_back(fun);
}

void GpuSqlDispatcher::addAvgFunction(DataType key, DataType value, GroupByType groupByType)
{
	GpuSqlDispatcher::DispatchFunction fun;
	switch (groupByType)
	{
	case GroupByType::NO_GROUP_BY:
		fun = avgAggregationFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::SINGLE_KEY_GROUP_BY:
		fun = avgGroupByFunctions[DataType::DATA_TYPE_SIZE * key + value];
		break;
	case GroupByType::MULTI_KEY_GROUP_BY:
		fun = avgGroupByMultiKeyFunctions[value];
		break;
	default:
		break;
	}
	dispatcherFunctions.push_back(fun);
}

void GpuSqlDispatcher::addGroupByFunction(DataType type)
{
    dispatcherFunctions.push_back(groupByFunctions[type]);
}

void GpuSqlDispatcher::addBetweenFunction(DataType op1, DataType op2, DataType op3)
{
    //TODO: Between
}

void GpuSqlDispatcher::fillPolygonRegister(GPUMemory::GPUPolygon& polygonColumn, const std::string & reg, int32_t size, bool useCache, int8_t* nullMaskPtr)
{
	allocatedPointers.insert({ reg + "_polyPoints", PointerAllocation{reinterpret_cast<uintptr_t>(polygonColumn.polyPoints), size, !useCache, reinterpret_cast<uintptr_t>(nullMaskPtr)} });
	allocatedPointers.insert({ reg + "_pointIdx", PointerAllocation{reinterpret_cast<uintptr_t>(polygonColumn.pointIdx), size, !useCache, reinterpret_cast<uintptr_t>(nullMaskPtr)} });
	allocatedPointers.insert({ reg + "_polyIdx", PointerAllocation{reinterpret_cast<uintptr_t>(polygonColumn.polyIdx), size, !useCache, reinterpret_cast<uintptr_t>(nullMaskPtr)} });
}


std::string GpuSqlDispatcher::getAllocatedRegisterName(const std::string & reg)
{
	if (usingJoin && reg.front() != '$')
	{
		std::string joinReg = reg + "_join";
		for (auto& joinTable : *joinIndices)
		{
			joinReg += "_" + joinTable.first;
		}
		return joinReg;
	}
	return reg;
}

void GpuSqlDispatcher::fillStringRegister(GPUMemory::GPUString & stringColumn, const std::string & reg, int32_t size, bool useCache, int8_t* nullMaskPtr)
{
	allocatedPointers.insert({ reg + "_stringIndices", PointerAllocation{reinterpret_cast<uintptr_t>(stringColumn.stringIndices), size, !useCache, reinterpret_cast<uintptr_t>(nullMaskPtr)} });
	allocatedPointers.insert({ reg + "_allChars", PointerAllocation{reinterpret_cast<uintptr_t>(stringColumn.allChars), size, !useCache, reinterpret_cast<uintptr_t>(nullMaskPtr)} });

}

int32_t GpuSqlDispatcher::loadColNullMask(std::string & colName)
{
	if (allocatedPointers.find(colName + NULL_SUFFIX) == allocatedPointers.end() && !colName.empty() && colName.front() != '$')
	{
		std::cout << "LoadNullMask: " << colName << std::endl;

		// split colName to table and column name
		const size_t endOfPolyIdx = colName.find(".");
		const std::string table = colName.substr(0, endOfPolyIdx);
		const std::string column = colName.substr(endOfPolyIdx + 1);

		const int32_t blockCount = database->GetTables().at(table).GetColumns().at(column).get()->GetBlockCount();
		GpuSqlDispatcher::deviceCountLimit_ = std::min(Context::getInstance().getDeviceCount() - 1, blockCount - 1);
		if (blockIndex >= blockCount)
		{
			return 1;
		}
		if (blockIndex >= blockCount - Context::getInstance().getDeviceCount())
		{
			isLastBlockOfDevice = true;
		}
		if (blockIndex == blockCount - 1)
		{
			isOverallLastBlock = true;
		}

		auto blockNullMask = database->GetTables().at(table).GetColumns().at(column)->GetNullBitMaskForBlock(blockIndex);
		size_t blockNullMaskSize = (std::get<1>(blockNullMask) + 8 * sizeof(int8_t) - 1) / (8 * sizeof(int8_t));

		auto cacheEntry = Context::getInstance().getCacheForCurrentDevice().getColumn<int8_t>(
			database->GetName(), colName + NULL_SUFFIX, blockIndex, blockNullMaskSize);
		if (!std::get<2>(cacheEntry))
		{
			GPUMemory::copyHostToDevice(std::get<0>(cacheEntry), std::get<0>(blockNullMask), blockNullMaskSize);
		}
		addCachedRegister(colName + NULL_SUFFIX, std::get<0>(cacheEntry), std::get<1>(blockNullMask));

		noLoad = false;
	}
	return 0;
}

GPUMemory::GPUPolygon GpuSqlDispatcher::insertComplexPolygon(const std::string& databaseName, const std::string& colName, const std::vector<ColmnarDB::Types::ComplexPolygon>& polygons, int32_t size, bool useCache, int8_t* nullMaskPtr)
{
	if (useCache)
	{
		if (Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_polyPoints", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_pointIdx", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_polyIdx", blockIndex))
		{
			GPUMemoryCache& cache = Context::getInstance().getCacheForCurrentDevice();
			GPUMemory::GPUPolygon polygon;
			polygon.polyPoints = std::get<0>(cache.getColumn<NativeGeoPoint>(databaseName, colName + "_polyPoints", blockIndex, size));
			polygon.pointIdx = std::get<0>(cache.getColumn<int32_t>(databaseName, colName + "_pointIdx", blockIndex, size));
			polygon.polyIdx = std::get<0>(cache.getColumn<int32_t>(databaseName, colName + "_polyIdx", blockIndex, size));

			fillPolygonRegister(polygon, colName, size, useCache, nullMaskPtr);

			return polygon;
		}
		else
		{
			GPUMemory::GPUPolygon polygon = ComplexPolygonFactory::PrepareGPUPolygon(polygons, databaseName, colName, blockIndex);
			fillPolygonRegister(polygon, colName, size, useCache, nullMaskPtr);
			return polygon;
		}
	}
	else
	{
		GPUMemory::GPUPolygon polygon = ComplexPolygonFactory::PrepareGPUPolygon(polygons);
		fillPolygonRegister(polygon, colName, size, useCache, nullMaskPtr);
		return polygon;
	}
}

GPUMemory::GPUString GpuSqlDispatcher::insertString(const std::string& databaseName, const std::string& colName, const std::vector<std::string>& strings, int32_t size, bool useCache, int8_t* nullMaskPtr)
{
	if (useCache)
	{
		if (Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_stringIndices", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_allChars", blockIndex))
		{
			GPUMemoryCache& cache = Context::getInstance().getCacheForCurrentDevice();
			GPUMemory::GPUString gpuString;
			gpuString.stringIndices = std::get<0>(cache.getColumn<int64_t>(databaseName, colName + "_stringIndices", blockIndex, size));
			gpuString.allChars = std::get<0>(cache.getColumn<char>(databaseName, colName + "_allChars", blockIndex, size));
			fillStringRegister(gpuString, colName, size, useCache, nullMaskPtr);
			return gpuString;
		}
		else
		{
			GPUMemory::GPUString gpuString = StringFactory::PrepareGPUString(strings, databaseName, colName, blockIndex);
			fillStringRegister(gpuString, colName, size, useCache, nullMaskPtr);
			return gpuString;
		}
	}
	else
	{
		GPUMemory::GPUString gpuString = StringFactory::PrepareGPUString(strings);
		fillStringRegister(gpuString, colName, size, useCache, nullMaskPtr);
		return gpuString;
	}
}

std::tuple<GPUMemory::GPUPolygon, int32_t, int8_t*> GpuSqlDispatcher::findComplexPolygon(std::string colName)
{
	GPUMemory::GPUPolygon polygon;
	int32_t size = allocatedPointers.at(colName + "_polyPoints").elementCount;

	polygon.polyPoints = reinterpret_cast<NativeGeoPoint*>(allocatedPointers.at(colName + "_polyPoints").gpuPtr);
	polygon.pointIdx = reinterpret_cast<int32_t*>(allocatedPointers.at(colName + "_pointIdx").gpuPtr);
	polygon.polyIdx = reinterpret_cast<int32_t*>(allocatedPointers.at(colName + "_polyIdx").gpuPtr);

	return std::make_tuple(polygon, size, reinterpret_cast<int8_t*>(allocatedPointers.at(colName + "_polyPoints").gpuNullMaskPtr));
}

std::tuple<GPUMemory::GPUString, int32_t, int8_t*> GpuSqlDispatcher::findStringColumn(const std::string & colName)
{
	GPUMemory::GPUString gpuString;
	int32_t size = allocatedPointers.at(colName + "_stringIndices").elementCount;
	gpuString.stringIndices = reinterpret_cast<int64_t*>(allocatedPointers.at(colName + "_stringIndices").gpuPtr);
	gpuString.allChars = reinterpret_cast<char*>(allocatedPointers.at(colName + "_allChars").gpuPtr);
	return std::make_tuple(gpuString, size, reinterpret_cast<int8_t*>(allocatedPointers.at(colName + "_stringIndices").gpuNullMaskPtr));
}

GPUMemory::GPUString GpuSqlDispatcher::insertConstStringGpu(const std::string& str)
{
	std::string name = "constString" + std::to_string(constStringCounter);
	constStringCounter++;
	return insertString(database->GetName(), name, { str }, 1);
}

/// Clears all allocated buffers
/// Resets memory stream reading index to prepare for execution on the next block of data
void GpuSqlDispatcher::cleanUpGpuPointers()
{
	usingGroupBy = false;
	arguments.reset();
	for (auto& ptr : allocatedPointers)
	{
		if (ptr.second.gpuPtr != 0 && ptr.second.shouldBeFreed)
		{
			GPUMemory::free(reinterpret_cast<void*>(ptr.second.gpuPtr));
		}
	}
	usedRegisterMemory = 0;
	aggregatedRegisters.clear();
	allocatedPointers.clear();
}


/// Implementation of FIL operation
/// Marks WHERE clause result register as the filtering register
/// <returns name="statusCode">Finish status code of the operation</returns>
int32_t GpuSqlDispatcher::fil()
{
    auto reg = arguments.read<std::string>();
    std::cout << "Filter: " << reg << std::endl;
	filter_ = allocatedPointers.at(reg).gpuPtr;
	return 0;
}

int32_t GpuSqlDispatcher::whereEvaluation()
{
	loadNecessary = usingJoin ? 1 : cpuDispatcher.execute(blockIndex);
	std::cout << "Where load evaluation: " << loadNecessary << std::endl;
	return 0;
}


/// Implementation of JMP operation
/// Determines next block index to process by this instance of dispatcher based on CUDA device count
/// <returns name="statusCode">Finish status code of the operation</returns>
int32_t GpuSqlDispatcher::jmp()
{
	Context& context = Context::getInstance();

	if (noLoad && loadNecessary != 0)
	{
		cleanUpGpuPointers();
		return 0;
	}

	if (!isLastBlockOfDevice)
	{
		blockIndex += context.getDeviceCount();
		context.getCacheForCurrentDevice().setCurrentBlockIndex(blockIndex);
		instructionPointer = 0;
		cleanUpGpuPointers();
		return 0;
	}

	std::cout << "Jump" << std::endl;
	return 0;
}


/// Implementation of DONE operation
/// Clears all allocated temporary result buffers
/// <returns name="statusCode">Finish status code of the operation</returns>
int32_t GpuSqlDispatcher::done()
{
	cleanUpGpuPointers();
	std::cout << "Done" << std::endl;
	return 1;
}

/// Implementation of SHOW DATABASES operation
/// Inserts database names to the response message
/// <returns name="statusCode">Finish status code of the operation</returns>
int32_t GpuSqlDispatcher::showDatabases()
{
	auto databases_map = Database::GetDatabaseNames();
	std::unique_ptr<std::string[]> outData(new std::string[databases_map.size()]);
	
	int i = 0;
	for (auto& database : databases_map) {
		outData[i++] = database;
	}
	
	ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
	insertIntoPayload(payload, outData, databases_map.size());
	MergePayloadToSelfResponse("Databases", payload);

	return 2;
}


/// Implementation of SHOW TABLES operation
/// Inserts table names to the response message
/// <returns name="statusCode">Finish status code of the operation</returns>
int32_t GpuSqlDispatcher::showTables()
{
	std::string db = arguments.read<std::string>();
	std::shared_ptr<Database> database = Database::GetDatabaseByName(db);

	std::unique_ptr<std::string[]> outData(new std::string[database->GetTables().size()]);
	auto& tables_map = database->GetTables();

	int i = 0;
	for (auto& tableName : tables_map) {
		outData[i++] = tableName.first;
	}

	ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
	insertIntoPayload(payload, outData, tables_map.size());
	MergePayloadToSelfResponse(db, payload);

	return 3;
}

/// Implementation of SHOW COLUMN operation
/// Inserts column names and their types to the response message
/// <returns name="statusCode">Finish status code of the operation</returns>
int32_t GpuSqlDispatcher::showColumns()
{
	std::string db = arguments.read<std::string>();
	std::string tab = arguments.read<std::string>();

	std::shared_ptr<Database> database = Database::GetDatabaseByName(db);
	auto& table = database->GetTables().at(tab);

	auto& columns_map = table.GetColumns();
	//std::vector<std::string> columns;
	std::unique_ptr<std::string[]> outDataName(new std::string[table.GetColumns().size()]);
	std::unique_ptr<std::string[]> outDataType(new std::string[table.GetColumns().size()]);

	int i = 0;
	for (auto& column : columns_map) {
		outDataName[i] = column.first;
		outDataType[i] = std::to_string(column.second.get()->GetColumnType());
		i++;
	}

	ColmnarDB::NetworkClient::Message::QueryResponsePayload payloadName;
	ColmnarDB::NetworkClient::Message::QueryResponsePayload payloadType;
	insertIntoPayload(payloadName, outDataName, columns_map.size());
	insertIntoPayload(payloadType, outDataType, columns_map.size());
	MergePayloadToSelfResponse(tab + "_columns", payloadName);
	MergePayloadToSelfResponse(tab + "_types", payloadType);
	return 4;
}

int32_t GpuSqlDispatcher::createDatabase()
{
	std::string newDbName = arguments.read<std::string>();
	int32_t newDbBlockSize = arguments.read<int32_t>();
	std::shared_ptr<Database> newDb = std::make_shared<Database>(newDbName.c_str(), newDbBlockSize);
	Database::AddToInMemoryDatabaseList(newDb);
	return 6;
}

int32_t GpuSqlDispatcher::dropDatabase()
{
	std::string dbName = arguments.read<std::string>();
	Database::GetDatabaseByName(dbName)->DeleteDatabaseFromDisk();
	Database::RemoveFromInMemoryDatabaseList(dbName.c_str());
	return 7;
}

int32_t GpuSqlDispatcher::createTable()
{
	std::unordered_map<std::string, DataType> newColumns;
	std::unordered_map<std::string, std::vector<std::string>> newIndices;

	std::string newTableName = arguments.read<std::string>();

	int32_t newColumnsCount = arguments.read<int32_t>();
	for (int32_t i = 0; i < newColumnsCount; i++)
	{
		std::string newColumnName = arguments.read<std::string>();
		int32_t newColumnDataType = arguments.read<int32_t>();
		newColumns.insert({ newColumnName, static_cast<DataType>(newColumnDataType) });
	}

	std::vector<std::string> allIndexColumns;

	int32_t newIndexCount = arguments.read<int32_t>();
	for (int32_t i = 0; i < newIndexCount; i++)
	{
		std::string newIndexName = arguments.read<std::string>();
		int32_t newIndexColumnCount = arguments.read<int32_t>();
		std::vector<std::string> newIndexColumns;

		for (int32_t j = 0; j < newIndexColumnCount; j++)
		{
			std::string newIndexColumn = arguments.read<std::string>();
			newIndexColumns.push_back(newIndexColumn);
			if (std::find(allIndexColumns.begin(), allIndexColumns.end(), newIndexColumn) == allIndexColumns.end())
			{
				allIndexColumns.push_back(newIndexColumn);
			}
		}
		newIndices.insert({ newIndexName, newIndexColumns });
	}

	database->CreateTable(newColumns, newTableName.c_str()).SetSortingColumns(allIndexColumns);
	return 8;
}

int32_t GpuSqlDispatcher::dropTable()
{
	std::string tableName = arguments.read<std::string>();
	database->GetTables().erase(tableName);
	database->DeleteTableFromDisk(tableName.c_str());
	return 9;
}

int32_t GpuSqlDispatcher::alterTable()
{
	std::string tableName = arguments.read<std::string>();

	int32_t addColumnsCount = arguments.read<int32_t>();
	for (int32_t i = 0; i < addColumnsCount; i++)
	{
		std::string addColumnName = arguments.read<std::string>();
		int32_t addColumnDataType = arguments.read<int32_t>();
		database->GetTables().at(tableName).CreateColumn(addColumnName.c_str(), static_cast<DataType>(addColumnDataType));
		int64_t tableSize = database->GetTables().at(tableName).GetSize();
		database->GetTables().at(tableName).GetColumns().at(addColumnName)->InsertNullData(tableSize);
	}

	int32_t dropColumnsCount = arguments.read<int32_t>();
	for (int32_t i = 0; i < dropColumnsCount; i++)
	{
		std::string dropColumnName = arguments.read<std::string>();
		database->GetTables().at(tableName).EraseColumn(dropColumnName);
		database->DeleteColumnFromDisk(tableName.c_str(), dropColumnName.c_str());
	}
	return 10;
}

int32_t GpuSqlDispatcher::createIndex()
{
	std::string	indexName = arguments.read<std::string>();
	std::string tableName = arguments.read<std::string>();
	std::vector<std::string> sortingColumns;

	for (auto& column : database->GetTables().at(tableName).GetSortingColumns())
	{
		sortingColumns.push_back(column);
	}

	int32_t indexColumnCount = arguments.read<int32_t>();
	for (int i = 0; i < indexColumnCount; i++)
	{
		std::string indexColumn = arguments.read<std::string>();
		if (std::find(sortingColumns.begin(), sortingColumns.end(), indexColumn) == sortingColumns.end())
		{
			sortingColumns.push_back(indexColumn);
		}
	}

	database->GetTables().at(tableName).SetSortingColumns(sortingColumns);

	return 11;
}


void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<int32_t[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_intpayload()->add_intdata(data[i]);
	}
}

void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<int64_t[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_int64payload()->add_int64data(data[i]);
	}
}

void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<float[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_floatpayload()->add_floatdata(data[i]);
	}
}

void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<double[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_doublepayload()->add_doubledata(data[i]);
	}
}

void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<std::string[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_stringpayload()->add_stringdata(data[i]);
	}
}

void GpuSqlDispatcher::MergePayloadBitmask(const std::string &key, ColmnarDB::NetworkClient::Message::QueryResponseMessage * responseMessage, const std::string& nullMask)
{
	if (responseMessage->nullbitmasks().find(key) == responseMessage->nullbitmasks().end())
	{
		responseMessage->mutable_nullbitmasks()->insert({ key, nullMask });
	}
	else    // If there is payload with existing key, merge or aggregate according to key
	{
		responseMessage->mutable_nullbitmasks()->at(key) += nullMask;
	}

}

void GpuSqlDispatcher::MergePayload(const std::string &trimmedKey, ColmnarDB::NetworkClient::Message::QueryResponseMessage * responseMessage,
	ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload)
{
	// If there is payload with new key
	if (responseMessage->payloads().find(trimmedKey) == responseMessage->payloads().end())
	{
		responseMessage->mutable_payloads()->insert({ trimmedKey, payload });
	}
	else    // If there is payload with existing key, merge or aggregate according to key
	{
		// Find index of parenthesis (for finding out if it is aggregation function)
		size_t keyParensIndex = trimmedKey.find('(');

		bool aggregationOperationFound = false;
		// If no function is used
		if (keyParensIndex == std::string::npos)
		{
			aggregationOperationFound = false;
		}
		else
		{
			// Get operation name
			std::string operation = trimmedKey.substr(0, keyParensIndex);
			// To upper case
			for (auto &c : operation)
			{
				c = toupper(c);
			}
			// Switch according to data type of payload (=column)
			switch (payload.payload_case())
			{
			case ColmnarDB::NetworkClient::Message::QueryResponsePayload::PayloadCase::kIntPayload:
			{
				std::pair<bool, int32_t> result =
					AggregateOnCPU<int32_t>(operation, payload.intpayload().intdata()[0],
						responseMessage->mutable_payloads()->at(trimmedKey).intpayload().intdata()[0]);
				aggregationOperationFound = result.first;
				if (aggregationOperationFound)
				{
					responseMessage->mutable_payloads()->at(trimmedKey).mutable_intpayload()->set_intdata(0, result.second);
				}
				break;
			}
			case ColmnarDB::NetworkClient::Message::QueryResponsePayload::PayloadCase::kInt64Payload:
			{
				std::pair<bool, int64_t> result =
					AggregateOnCPU<int64_t>(operation, payload.int64payload().int64data()[0],
						responseMessage->payloads().at(trimmedKey).int64payload().int64data()[0]);
				aggregationOperationFound = result.first;
				if (aggregationOperationFound)
				{
					responseMessage->mutable_payloads()->at(trimmedKey).mutable_int64payload()->set_int64data(0, result.second);
				}
				break;
			}
			case ColmnarDB::NetworkClient::Message::QueryResponsePayload::PayloadCase::kFloatPayload:
			{
				std::pair<bool, float> result =
					AggregateOnCPU<float>(operation, payload.floatpayload().floatdata()[0],
						responseMessage->mutable_payloads()->at(trimmedKey).floatpayload().floatdata()[0]);
				aggregationOperationFound = result.first;
				if (aggregationOperationFound)
				{
					responseMessage->mutable_payloads()->at(trimmedKey).mutable_floatpayload()->set_floatdata(0, result.second);
				}
				break;
			}
			case ColmnarDB::NetworkClient::Message::QueryResponsePayload::PayloadCase::kDoublePayload:
			{
				std::pair<bool, double> result =
					AggregateOnCPU<double>(operation, payload.doublepayload().doubledata()[0],
						responseMessage->mutable_payloads()->at(trimmedKey).doublepayload().doubledata()[0]);
				aggregationOperationFound = result.first;
				if (aggregationOperationFound)
				{
					responseMessage->mutable_payloads()->at(trimmedKey).mutable_doublepayload()->set_doubledata(0, result.second);
				}
				break;
			}
			default:
				// This case is taken even without aggregation functions, because Points are considered functions 
				// for some reason
				if(aggregationOperationFound)
				{
					throw std::out_of_range("Unsupported aggregation type result");
				}
				break;
			}
		}

		if (!aggregationOperationFound)
		{
			responseMessage->mutable_payloads()->at(trimmedKey).MergeFrom(payload);
		}
	}
}

void GpuSqlDispatcher::MergePayloadToSelfResponse(const std::string& key, ColmnarDB::NetworkClient::Message::QueryResponsePayload& payload, const std::string& nullBitMaskString)
{
	std::string trimmedKey = key.substr(0, std::string::npos);
	if (!key.empty() && key.front() == '$')
	{
		trimmedKey = key.substr(1, std::string::npos);
	}
	MergePayload(trimmedKey, &responseMessage, payload);
	if(!nullBitMaskString.empty())
	{
		MergePayloadBitmask(trimmedKey,&responseMessage, nullBitMaskString);
	}
}

bool GpuSqlDispatcher::isRegisterAllocated(const std::string& reg)
{
	return allocatedPointers.find(reg) != allocatedPointers.end();
}

std::pair<std::string, std::string> GpuSqlDispatcher::splitColumnName(const std::string& colName)
{
	const size_t splitIdx = colName.find(".");
	const std::string table = colName.substr(0, splitIdx);
	const std::string column = colName.substr(splitIdx + 1);
	return {table, column};
}
