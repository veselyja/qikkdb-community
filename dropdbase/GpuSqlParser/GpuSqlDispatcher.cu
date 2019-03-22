//
// Created by Martin Staňo on 2019-01-15.
//

#include "GpuSqlDispatcher.h"
#include "GpuSqlDispatcherFriends.h"
#include "../QueryEngine/Context.h"
#include "../Types/ComplexPolygon.pb.h"
#include "../Types/Point.pb.h"
#include "ParserExceptions.h"
#include <regex>

int32_t GpuSqlDispatcher::groupByDoneCounter_ = 0;
std::mutex GpuSqlDispatcher::groupByMutex_;
std::condition_variable GpuSqlDispatcher::groupByCV_;
int32_t GpuSqlDispatcher::groupByDoneLimit_;
//TODO:Dispatch implementation

GpuSqlDispatcher::GpuSqlDispatcher(const std::shared_ptr<Database> &database, std::vector<std::unique_ptr<IGroupBy>>& groupByTables, int dispatcherThreadId) :
	database(database),
	blockIndex(dispatcherThreadId),
	instructionPointer(0),
	constPointCounter(0),
	constPolygonCounter(0),
	filter_(0),
	usedRegisterMemory(0),
	maxRegisterMemory(0), // TODO value from config e.g.
	groupByTables(groupByTables),
	dispatcherThreadId(dispatcherThreadId),
	usingGroupBy(false),
	isLastBlockOfDevice(false),
	isOverallLastBlock(false),
	noLoad(true)
{

}

GpuSqlDispatcher::~GpuSqlDispatcher()
{
	cleanUpGpuPointers();
}


void GpuSqlDispatcher::copyExecutionDataTo(GpuSqlDispatcher & other)
{
	other.dispatcherFunctions = dispatcherFunctions;
	other.arguments = arguments;
}

void GpuSqlDispatcher::execute(std::unique_ptr<google::protobuf::Message>& result, std::exception_ptr& exception)
{
	try
	{
		Context& context = Context::getInstance();
		context.bindDeviceToContext(dispatcherThreadId);
		int32_t err = 0;

		while (err == 0)
		{
			err = this->(*dispatcherFunctions[instructionPointer++]());
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
				break;
			}
		}
		result = std::make_unique<ColmnarDB::NetworkClient::Message::QueryResponseMessage>(std::move(responseMessage));
	}
	catch (...)
	{
		exception = std::current_exception();
	}
}

const ColmnarDB::NetworkClient::Message::QueryResponseMessage &GpuSqlDispatcher::getQueryResponseMessage()
{
	return responseMessage;
}

void GpuSqlDispatcher::addRetFunction(DataType type)
{
    dispatcherFunctions.push_back(retFunctions[type]);
}

void GpuSqlDispatcher::addFilFunction()
{
    dispatcherFunctions.push_back(filFunction);
}

void GpuSqlDispatcher::addJmpInstruction()
{
	dispatcherFunctions.push_back(jmpFunction);
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

void GpuSqlDispatcher::addPointFunction(DataType left, DataType right)
{
	dispatcherFunctions.push_back(pointFunctions[DataType::DATA_TYPE_SIZE * left + right]);
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


void GpuSqlDispatcher::addMinFunction(DataType key, DataType value)
{
    dispatcherFunctions.push_back(minFunctions[DataType::DATA_TYPE_SIZE * key + value]);
}


void GpuSqlDispatcher::addMaxFunction(DataType key, DataType value)
{
    dispatcherFunctions.push_back(maxFunctions[DataType::DATA_TYPE_SIZE * key + value]);
}


void GpuSqlDispatcher::addSumFunction(DataType key, DataType value)
{
    dispatcherFunctions.push_back(sumFunctions[DataType::DATA_TYPE_SIZE * key + value]);
}


void GpuSqlDispatcher::addCountFunction(DataType key, DataType value)
{
    dispatcherFunctions.push_back(countFunctions[DataType::DATA_TYPE_SIZE * key + value]);
}


void GpuSqlDispatcher::addAvgFunction(DataType key, DataType value)
{
    dispatcherFunctions.push_back(avgFunctions[DataType::DATA_TYPE_SIZE * key + value]);
}

void GpuSqlDispatcher::addGroupByFunction(DataType type)
{
    dispatcherFunctions.push_back(groupByFunctions[type]);
}

void GpuSqlDispatcher::addBetweenFunction(DataType op1, DataType op2, DataType op3)
{
    //TODO: Between
}

void GpuSqlDispatcher::insertComplexPolygon(const std::string& databaseName, const std::string& colName, const std::vector<ColmnarDB::Types::ComplexPolygon>& polygons, int32_t size, bool useCache)
{
	if (useCache)
	{
		if (Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_polyPoints", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_pointIdx", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_pointCount", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_polyIdx", blockIndex) &&
			Context::getInstance().getCacheForCurrentDevice().containsColumn(databaseName, colName + "_polyCount", blockIndex))
		{
			auto polyPoints = Context::getInstance().getCacheForCurrentDevice().getColumn<NativeGeoPoint>(databaseName, colName + "_polyPoints", blockIndex, size);
			auto pointIdx = Context::getInstance().getCacheForCurrentDevice().getColumn<int32_t>(databaseName, colName + "_pointIdx", blockIndex, size);
			auto pointCount = Context::getInstance().getCacheForCurrentDevice().getColumn<int32_t>(databaseName, colName + "_pointCount", blockIndex, size);
			auto polyIdx = Context::getInstance().getCacheForCurrentDevice().getColumn<int32_t>(databaseName, colName + "_polyIdx", blockIndex, size);
			auto polyCount = Context::getInstance().getCacheForCurrentDevice().getColumn<int32_t>(databaseName, colName + "_polyCount", blockIndex, size);
			allocatedPointers.insert({ colName + "_polyPoints", std::make_tuple(reinterpret_cast<uintptr_t>(std::get<0>(polyPoints)), size, false) });
			allocatedPointers.insert({ colName + "_pointIdx", std::make_tuple(reinterpret_cast<uintptr_t>(std::get<0>(pointIdx)), size, false) });
			allocatedPointers.insert({ colName + "_pointCount", std::make_tuple(reinterpret_cast<uintptr_t>(std::get<0>(pointCount)), size, false) });
			allocatedPointers.insert({ colName + "_polyIdx", std::make_tuple(reinterpret_cast<uintptr_t>(std::get<0>(polyIdx)), size, false) });
			allocatedPointers.insert({ colName + "_polyCount", std::make_tuple(reinterpret_cast<uintptr_t>(std::get<0>(polyCount)), size, false) });
		}
		else
		{
			GPUMemory::GPUPolygon polygon = ComplexPolygonFactory::PrepareGPUPolygon(polygons, databaseName, colName, blockIndex);
			allocatedPointers.insert({ colName + "_polyPoints", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.polyPoints), size, false) });
			allocatedPointers.insert({ colName + "_pointIdx", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.pointIdx), size, false) });
			allocatedPointers.insert({ colName + "_pointCount", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.pointCount), size, false) });
			allocatedPointers.insert({ colName + "_polyIdx", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.polyIdx), size, false) });
			allocatedPointers.insert({ colName + "_polyCount", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.polyCount), size, false) });
		}
	}
	else
	{
		GPUMemory::GPUPolygon polygon = ComplexPolygonFactory::PrepareGPUPolygon(polygons);
		allocatedPointers.insert({ colName + "_polyPoints", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.polyPoints), size, true) });
		allocatedPointers.insert({ colName + "_pointIdx", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.pointIdx), size, true) });
		allocatedPointers.insert({ colName + "_pointCount", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.pointCount), size, true) });
		allocatedPointers.insert({ colName + "_polyIdx", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.polyIdx), size, true) });
		allocatedPointers.insert({ colName + "_polyCount", std::make_tuple(reinterpret_cast<uintptr_t>(polygon.polyCount), size, true) });
	}
}

std::tuple<GPUMemory::GPUPolygon, int32_t> GpuSqlDispatcher::findComplexPolygon(std::string colName)
{
	GPUMemory::GPUPolygon polygon;
	int32_t size = std::get<1>(allocatedPointers.at(colName + "_polyPoints"));

	polygon.polyPoints = reinterpret_cast<NativeGeoPoint*>(std::get<0>(allocatedPointers.at(colName + "_polyPoints")));
	polygon.pointIdx = reinterpret_cast<int32_t*>(std::get<0>(allocatedPointers.at(colName + "_pointIdx")));
	polygon.pointCount = reinterpret_cast<int32_t*>(std::get<0>(allocatedPointers.at(colName + "_pointCount")));
	polygon.polyIdx = reinterpret_cast<int32_t*>(std::get<0>(allocatedPointers.at(colName + "_polyIdx")));
	polygon.polyCount = reinterpret_cast<int32_t*>(std::get<0>(allocatedPointers.at(colName + "_polyCount")));

	return std::make_tuple(polygon, size);
}

NativeGeoPoint* GpuSqlDispatcher::insertConstPointGpu(ColmnarDB::Types::Point& point)
{
	NativeGeoPoint nativePoint;
	nativePoint.latitude = point.geopoint().latitude();
	nativePoint.longitude = point.geopoint().longitude();

	NativeGeoPoint *gpuPointer = allocateRegister<NativeGeoPoint>("constPoint" + std::to_string(constPointCounter), 1);
	constPointCounter++;

	GPUMemory::copyHostToDevice(gpuPointer, reinterpret_cast<NativeGeoPoint*>(&nativePoint), 1);
	return gpuPointer;
}

std::string GpuSqlDispatcher::insertConstPolygonGpu(ColmnarDB::Types::ComplexPolygon& polygon)
{
	std::string ret = "constPolygon" + std::to_string(constPolygonCounter);
	insertComplexPolygon(database->GetName(), ret, { polygon }, 1);
	constPolygonCounter++;
	return ret;
}

void GpuSqlDispatcher::cleanUpGpuPointers()
{
	usingGroupBy = false;
	arguments.reset();
	for (auto& ptr : allocatedPointers)
	{
		if (std::get<2>(ptr.second))
		{
			GPUMemory::free(reinterpret_cast<void*>(std::get<0>(ptr.second)));
		}
	}
	usedRegisterMemory = 0;
	allocatedPointers.clear();
}

template <>
int32_t GpuSqlDispatcher::loadCol<ColmnarDB::Types::ComplexPolygon>(std::string& colName)
{
    if (allocatedPointers.find(colName) == allocatedPointers.end() && !colName.empty() && colName.front() != '$')
	{
		std::cout << "Load: " << colName << " " << typeid(ColmnarDB::Types::ComplexPolygon).name() << std::endl;

		// split colName to table and column name
		const size_t endOfPolyIdx = colName.find(".");
		const std::string table = colName.substr(0, endOfPolyIdx);
		const std::string column = colName.substr(endOfPolyIdx + 1);

		const int32_t blockCount = database->GetTables().at(table).GetColumns().at(column).get()->GetBlockCount();
		GpuSqlDispatcher::groupByDoneLimit_ = std::min(Context::getInstance().getDeviceCount() -1, blockCount - 1);
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

		auto col = dynamic_cast<const ColumnBase<ColmnarDB::Types::ComplexPolygon>*>(database->GetTables().at(table).GetColumns().at(column).get());
		auto block = dynamic_cast<BlockBase<ColmnarDB::Types::ComplexPolygon>*>(col->GetBlocksList()[blockIndex].get());
        insertComplexPolygon(database->GetName(), colName,
                             std::vector<ColmnarDB::Types::ComplexPolygon>(block->GetData(),
                                                                           block->GetData() + block->GetSize()),
                             block->GetSize());
		noLoad = false;
	}
	return 0;
}

template <>
int32_t GpuSqlDispatcher::loadCol<ColmnarDB::Types::Point>(std::string& colName)
{
    if (allocatedPointers.find(colName) == allocatedPointers.end() && !colName.empty() && colName.front() != '$')
	{
		std::cout << "Load: " << colName << " " << typeid(ColmnarDB::Types::Point).name() << std::endl;

		// split colName to table and column name
		const size_t endOfPolyIdx = colName.find(".");
		const std::string table = colName.substr(0, endOfPolyIdx);
		const std::string column = colName.substr(endOfPolyIdx + 1);

		const int32_t blockCount = database->GetTables().at(table).GetColumns().at(column).get()->GetBlockCount();
		GpuSqlDispatcher::groupByDoneLimit_ = std::min(Context::getInstance().getDeviceCount() - 1, blockCount - 1);
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

		auto col = dynamic_cast<const ColumnBase<ColmnarDB::Types::Point>*>(database->GetTables().at(table).GetColumns().at(column).get());
		auto block = dynamic_cast<BlockBase<ColmnarDB::Types::Point>*>(col->GetBlocksList()[blockIndex].get());

		std::vector<NativeGeoPoint> nativePoints;
		std::transform(block->GetData(), block->GetData() + block->GetSize(), std::back_inserter(nativePoints), [](const ColmnarDB::Types::Point& point) -> NativeGeoPoint { return NativeGeoPoint{ point.geopoint().latitude(), point.geopoint().longitude() }; });

        auto cacheEntry =
            Context::getInstance().getCacheForCurrentDevice().getColumn<NativeGeoPoint>(database->GetName(), colName, blockIndex,
                                                                                        nativePoints.size());
        if (!std::get<2>(cacheEntry))
        {
            GPUMemory::copyHostToDevice(std::get<0>(cacheEntry),
                                        reinterpret_cast<NativeGeoPoint*>(nativePoints.data()),
                                        nativePoints.size());
        }
        addCachedRegister(colName, std::get<0>(cacheEntry), nativePoints.size());
		noLoad = false;
	}
	return 0;
}


template <>
int32_t GpuSqlDispatcher::retCol<ColmnarDB::Types::ComplexPolygon>()
{
	if (usingGroupBy)
	{
		throw RetPolygonGroupByException();
	}
	else
	{
		auto colName = arguments.read<std::string>();
		std::cout << "RetPolygonCol: " << colName << ", thread: " << dispatcherThreadId << std::endl;

		const size_t endOfPolyIdx = colName.find(".");
		const std::string table = colName.substr(0, endOfPolyIdx);
		const std::string column = colName.substr(endOfPolyIdx + 1);

		auto col = dynamic_cast<const ColumnBase<ColmnarDB::Types::ComplexPolygon>*>(database->GetTables().at(table).GetColumns().at(column).get());
		auto block = dynamic_cast<BlockBase<ColmnarDB::Types::ComplexPolygon>*>(col->GetBlocksList()[blockIndex].get());
		
		noLoad = false;
		const int32_t blockCount = database->GetTables().at(table).GetColumns().at(column).get()->GetBlockCount();

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

		if (reinterpret_cast<int8_t*>(filter_))
		{
			std::unique_ptr<int32_t[]> outIndexes(new int32_t[block->GetSize()]);
			int32_t outSize;

			GPUReconstruct::GenerateIndexes<int32_t, int8_t>(outIndexes.get(), &outSize, reinterpret_cast<int8_t*>(filter_), block->GetSize());

			std::unique_ptr<std::string[]> outData(new std::string[outSize]);

			for (int i = 0; i < outSize; i++)
			{
				outData[i] = ComplexPolygonFactory::WktFromPolygon(block->GetData()[outIndexes.get()[i]]);
			}

			std::cout << "dataSize: " << outSize << std::endl;
			ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
			insertIntoPayload<std::string>(payload, outData, outSize);
			mergePayloadToResponse(colName, payload);
		}
		else
		{
			std::unique_ptr<std::string[]> outData(new std::string[block->GetSize()]);

			for (int i = 0; i < block->GetSize(); i++)
			{
				outData[i] = ComplexPolygonFactory::WktFromPolygon(block->GetData()[i]);
			}

			std::cout << "dataSize: " << block->GetSize() << std::endl;
			ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
			insertIntoPayload<std::string>(payload, outData, block->GetSize());
			mergePayloadToResponse(colName, payload);
		}
	}
	return 0;
}

template<>
int32_t GpuSqlDispatcher::retCol<ColmnarDB::Types::Point>()
{
	if (usingGroupBy)
	{
		throw RetPointGroupByException();
	}
	else
	{
		auto colName = arguments.read<std::string>();

		int32_t loadFlag = loadCol<ColmnarDB::Types::Point>(colName);
		if (loadFlag)
		{
			return loadFlag;
		}

		std::cout << "RetPointCol: " << colName << ", thread: " << dispatcherThreadId << std::endl;

		std::unique_ptr<NativeGeoPoint[]> outPoints(new NativeGeoPoint[database->GetBlockSize()]);
		int32_t outSize;
		//ToDo: Podmienene zapnut podla velkost buffera
		//GPUMemory::hostPin(outData.get(), database->GetBlockSize());
		std::tuple<uintptr_t, int32_t, bool> ACol = allocatedPointers.at(colName);
		GPUReconstruct::reconstructCol(outPoints.get(), &outSize, reinterpret_cast<NativeGeoPoint*>(std::get<0>(ACol)), reinterpret_cast<int8_t*>(filter_), std::get<1>(ACol));
		//GPUMemory::hostUnregister(outData.get());

		std::unique_ptr<std::string[]> outData(new std::string[outSize]);

		for (int i = 0; i < outSize; i++)
		{
			outData[i] = PointFactory::WktFromPoint(outPoints[i]);
		}

		std::cout << "dataSize: " << outSize << std::endl;
		ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
		insertIntoPayload<std::string>(payload, outData, outSize);
		mergePayloadToResponse(colName, payload);
	}
	return 0;
}

template <>
int32_t GpuSqlDispatcher::retCol<std::string>()
{
	if (usingGroupBy)
	{
		throw RetStringGroupByException();
	}
	else
	{
		auto colName = arguments.read<std::string>();
		std::cout << "RetStringCol: " << colName << ", thread: " << dispatcherThreadId << std::endl;

		const size_t endOfPolyIdx = colName.find(".");
		const std::string table = colName.substr(0, endOfPolyIdx);
		const std::string column = colName.substr(endOfPolyIdx + 1);

		auto col = dynamic_cast<const ColumnBase<std::string>*>(database->GetTables().at(table).GetColumns().at(column).get());
		auto block = dynamic_cast<BlockBase<std::string>*>(col->GetBlocksList()[blockIndex].get());
		
		noLoad = false;
		const int32_t blockCount = database->GetTables().at(table).GetColumns().at(column).get()->GetBlockCount();

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

		if (reinterpret_cast<int8_t*>(filter_))
		{
			std::unique_ptr<int32_t[]> outIndexes(new int32_t[block->GetSize()]);
			int32_t outSize;

			GPUReconstruct::GenerateIndexes<int32_t, int8_t>(outIndexes.get(), &outSize, reinterpret_cast<int8_t*>(filter_), block->GetSize());

			std::unique_ptr<std::string[]> outData(new std::string[outSize]);

			for (int i = 0; i < outSize; i++)
			{
				outData[i] = block->GetData()[outIndexes.get()[i]];
			}

			std::cout << "dataSize: " << outSize << std::endl;
			ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
			insertIntoPayload<std::string>(payload, outData, outSize);
			mergePayloadToResponse(colName, payload);
		}
		else
		{
			std::unique_ptr<std::string[]> outData(new std::string[block->GetSize()]);
			
			// this can be moved or smth but dont know how
			for (int i = 0; i < block->GetSize(); i++)
			{
				outData[i] = block->GetData()[i];
			}

			std::cout << "dataSize: " << block->GetSize() << std::endl;
			ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
			insertIntoPayload<std::string>(payload, outData, block->GetSize());
			mergePayloadToResponse(colName, payload);
		}
	}
	return 0;
}

int32_t GpuSqlDispatcher::fil()
{
    auto reg = arguments.read<std::string>();
    std::cout << "Filter: " << reg << std::endl;
	filter_ = std::get<0>(allocatedPointers.at(reg));
	return 0;
}

int32_t GpuSqlDispatcher::jmp()
{
	Context& context = Context::getInstance();

	if (noLoad)
	{
		cleanUpGpuPointers();
		return 0;
	}

	if (!isLastBlockOfDevice)
	{
		blockIndex += context.getDeviceCount();
		instructionPointer = 0;
		cleanUpGpuPointers();
		return 0;
	}

	std::cout << "Jump" << std::endl;
	return 0;
}

int32_t GpuSqlDispatcher::done()
{
	cleanUpGpuPointers();
	std::cout << "Done" << std::endl;
	return 1;
}

int32_t GpuSqlDispatcher::showDatabases()
{
	auto databases_map = Database::GetDatabaseNames();
	std::unique_ptr<std::string[]> outData(new std::string[databases_map.size()]);
	
	int i = 0;
	for (auto& database : databases_map) {
		outData[i++] = database;
	}
	
	ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
	insertIntoPayload<std::string>(payload, outData, databases_map.size());
	mergePayloadToResponse("Databases", payload);

	return 2;
}

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
	insertIntoPayload<std::string>(payload, outData, tables_map.size());
	mergePayloadToResponse(db, payload);

	return 3;
}

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
	insertIntoPayload<std::string>(payloadName, outDataName, columns_map.size());
	insertIntoPayload<std::string>(payloadType, outDataType, columns_map.size());
	mergePayloadToResponse(tab + "_columns", payloadName);
	mergePayloadToResponse(tab + "_types", payloadType);
	return 4;
}

template<>
int32_t GpuSqlDispatcher::insertInto<ColmnarDB::Types::Point>()
{
	std::string table = arguments.read<std::string>();
	std::string column = arguments.read<std::string>();
	bool isReferencedColumn = arguments.read<bool>();

	if (isReferencedColumn)
	{
		std::string args = arguments.read<std::string>();
		ColmnarDB::Types::Point point = PointFactory::FromWkt(args);
		
		dynamic_cast<ColumnBase<ColmnarDB::Types::Point>*>(database->GetTables().at(table).GetColumns().at(column).get())->InsertData({ point });
	}
	else
	{
		dynamic_cast<ColumnBase<ColmnarDB::Types::Point>*>(database->GetTables().at(table).GetColumns().at(column).get())->InsertNullData(1);
	}
	return 0;
}

template<>
int32_t GpuSqlDispatcher::insertInto<ColmnarDB::Types::ComplexPolygon>()
{
	std::string table = arguments.read<std::string>();
	std::string column = arguments.read<std::string>();
	bool isReferencedColumn = arguments.read<bool>();

	if (isReferencedColumn)
	{
		std::string args = arguments.read<std::string>();
		ColmnarDB::Types::ComplexPolygon polygon = ComplexPolygonFactory::FromWkt(args);

		dynamic_cast<ColumnBase<ColmnarDB::Types::ComplexPolygon>*>(database->GetTables().at(table).GetColumns().at(column).get())->InsertData({ polygon });
	}
	else
	{
		dynamic_cast<ColumnBase<ColmnarDB::Types::ComplexPolygon>*>(database->GetTables().at(table).GetColumns().at(column).get())->InsertNullData(1);
	}
	return 0;
}

int32_t GpuSqlDispatcher::insertIntoDone()
{
	return 5;
}

template<>
void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<int32_t[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_intpayload()->add_intdata(data[i]);
	}
}

template<>
void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<int64_t[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_int64payload()->add_int64data(data[i]);
	}
}


template<>
void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<float[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_floatpayload()->add_floatdata(data[i]);
	}
}


template<>
void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<double[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_doublepayload()->add_doubledata(data[i]);
	}
}

template<>
void GpuSqlDispatcher::insertIntoPayload(ColmnarDB::NetworkClient::Message::QueryResponsePayload &payload, std::unique_ptr<std::string[]> &data, int32_t dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		payload.mutable_stringpayload()->add_stringdata(data[i]);
	}
}

void GpuSqlDispatcher::mergePayloadToResponse(const std::string& key, ColmnarDB::NetworkClient::Message::QueryResponsePayload& payload)
{
	std::string trimmedKey = key.substr(0, std::string::npos);
	if (std::regex_match(key, std::regex("^(\\$).*")))
	{
		trimmedKey = key.substr(1, std::string::npos);
	}

	if (responseMessage.payloads().find(trimmedKey) == responseMessage.payloads().end())
	{
		responseMessage.mutable_payloads()->insert({ trimmedKey, payload });
	}
	else
	{
		responseMessage.mutable_payloads()->at(trimmedKey).MergeFrom(payload);
	}
}

template <typename T>
void GpuSqlDispatcher::freeColumnIfRegister(const std::string & col)
{
	if (usedRegisterMemory > maxRegisterMemory && !col.empty() && col.front() == '$') 
	{
		GPUMemory::free(reinterpret_cast<void*>(std::get<0>(allocatedPointers.at(col))));
		usedRegisterMemory -= std::get<1>(allocatedPointers.at(col))*sizeof(T);
		allocatedPointers.erase(col);
		std::cout << "Free: " << col << std::endl;
	}
}

bool GpuSqlDispatcher::isRegisterAllocated(std::string & reg)
{
	return allocatedPointers.find(reg) != allocatedPointers.end();
}