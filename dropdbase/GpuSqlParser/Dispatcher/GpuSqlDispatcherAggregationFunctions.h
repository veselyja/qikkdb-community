#pragma once

#include "../GpuSqlDispatcher.h"
#include "../../QueryEngine/GPUCore/cuda_ptr.h"
#include "../../QueryEngine/GPUCore/GPUGroupBy.cuh"
#include "../../QueryEngine/GPUCore/GPUGroupByString.cuh"
#include "../../QueryEngine/GPUCore/GPUGroupByMultiKey.cuh"
#include "../../QueryEngine/GPUCore/GPUAggregation.cuh"
#include "../../QueryEngine/GPUCore/GPUReconstruct.cuh"
#include "../../QueryEngine/GPUCore/GPUMemory.cuh"

/// Implementation of generic aggregation operation based on functor OP
/// Used when GROUP BY Clause is not present
/// Loads data on demand
/// COUNT operation is handled more efficiently
/// If WHERE clause is present filtering is done before agreggation
/// <returns name="statusCode">Finish status code of the operation</returns>
template <typename OP, typename OUT, typename IN>
int32_t GpuSqlDispatcher::AggregationCol()
{
    auto colName = arguments_.Read<std::string>();
    auto reg = arguments_.Read<std::string>();

    int32_t loadFlag = LoadCol<IN>(colName);
    if (loadFlag)
    {
        return loadFlag;
    }

    std::cout << "AggCol: " << colName << " " << reg << std::endl;

    PointerAllocation& column = allocatedPointers_.at(colName);
    int32_t reconstructOutSize;

    IN* reconstructOutReg = nullptr;
    if (std::is_same<OP, AggregationFunctions::count>::value)
    {
        // If mask is present - count suitable rows
        if (filter_)
        {
            int32_t* indexes = nullptr;
            GPUReconstruct::GenerateIndexesKeep(&indexes, &reconstructOutSize,
                                                reinterpret_cast<int8_t*>(filter_), column.ElementCount);
            if (indexes)
            {
                GPUMemory::free(indexes);
            }
        }
        // If mask is nullptr - count full rows
        else
        {
            reconstructOutSize = column.ElementCount;
        }
    }
    else
    {
        GPUReconstruct::reconstructColKeep<IN>(&reconstructOutReg, &reconstructOutSize,
                                               reinterpret_cast<IN*>(column.GpuPtr),
                                               reinterpret_cast<int8_t*>(filter_), column.ElementCount);
    }

    if (column.ShouldBeFreed)
    {
        GPUMemory::free(reinterpret_cast<void*>(column.GpuPtr));
    }
    else
    {
        column.ShouldBeFreed = true;
    }

    column.GpuPtr = reinterpret_cast<uintptr_t>(reconstructOutReg);
    column.ElementCount = reconstructOutSize;

    if (!IsRegisterAllocated(reg))
    {
        // TODO: if (not COUNT operation and std::get<1>(column) == 0), set result to NaN
        OUT* result = AllocateRegister<OUT>(reg, 1);
        GPUAggregation::col<OP, OUT, IN>(result, reinterpret_cast<IN*>(column.GpuPtr), column.ElementCount);
    }
    FreeColumnIfRegister<IN>(colName);
    filter_ = 0;
    return 0;
}

template <typename OP, typename OUT, typename IN>
int32_t GpuSqlDispatcher::AggregationConst()
{
    std::cout << "AggConst" << std::endl;
    return 0;
}

template <typename OP, typename O, typename K, typename V>
class GpuSqlDispatcher::GroupByHelper
{
public:
    static std::unique_ptr<IGroupBy>
    CreateInstance(int32_t groupByBuckets, const std::vector<std::pair<std::string, DataType>>& groupByColumns)
    {
        return std::make_unique<GPUGroupBy<OP, O, K, V>>(Configuration::GetInstance().GetGroupByBuckets());
    }

    static void ProcessBlock(const std::vector<std::pair<std::string, DataType>>& groupByColumns,
                             const PointerAllocation& valueColumn,
                             GpuSqlDispatcher& dispatcher)
    {
        std::string groupByColumnName = groupByColumns.begin()->first;
        PointerAllocation groupByColumn = dispatcher.allocatedPointers_.at(groupByColumnName);

        int32_t dataSize = std::min(groupByColumn.ElementCount, valueColumn.ElementCount);

        reinterpret_cast<GPUGroupBy<OP, O, K, V>*>(
            dispatcher.groupByTables_[dispatcher.dispatcherThreadId_].get())
            ->ProcessBlock(reinterpret_cast<K*>(groupByColumn.GpuPtr),
                           reinterpret_cast<V*>(valueColumn.GpuPtr), dataSize,
                           reinterpret_cast<int8_t*>(groupByColumn.GpuNullMaskPtr),
                           reinterpret_cast<int8_t*>(valueColumn.GpuNullMaskPtr));
    }

    static void GetResults(const std::vector<std::pair<std::string, DataType>>& groupByColumns,
                           const std::string& reg,
                           GpuSqlDispatcher& dispatcher)
    {
        std::string groupByColumnName = groupByColumns.begin()->first;
        int32_t outSize;
        K* outKeys = nullptr;
        int8_t* outKeyNullMask = nullptr;
        O* outValues = nullptr;
        int8_t* outValueNullMask = nullptr;
        reinterpret_cast<GPUGroupBy<OP, O, K, V>*>(
            dispatcher.groupByTables_[dispatcher.dispatcherThreadId_].get())
            ->GetResults(&outKeys, &outValues, &outSize, dispatcher.groupByTables_, &outKeyNullMask,
                         &outValueNullMask);
        dispatcher.InsertRegister(groupByColumnName + KEYS_SUFFIX,
                                  PointerAllocation{reinterpret_cast<uintptr_t>(outKeys), outSize, true,
                                                    reinterpret_cast<uintptr_t>(outKeyNullMask)});
        dispatcher.InsertRegister(reg, PointerAllocation{reinterpret_cast<uintptr_t>(outValues), outSize, true,
                                                         reinterpret_cast<uintptr_t>(outValueNullMask)});
    }
};

template <typename OP, typename O, typename V>
class GpuSqlDispatcher::GroupByHelper<OP, O, std::string, V>
{
public:
    static std::unique_ptr<IGroupBy>
    CreateInstance(int32_t groupByBuckets, const std::vector<std::pair<std::string, DataType>>& groupByColumns)
    {
        return std::make_unique<GPUGroupBy<OP, O, std::string, V>>(
            Configuration::GetInstance().GetGroupByBuckets());
    }

    static void ProcessBlock(const std::vector<std::pair<std::string, DataType>>& groupByColumns,
                             const PointerAllocation& valueColumn,
                             GpuSqlDispatcher& dispatcher)
    {
        std::string groupByColumnName = groupByColumns.begin()->first;
        auto groupByColumn = dispatcher.FindStringColumn(groupByColumnName);

        int32_t dataSize = std::min(std::get<1>(groupByColumn), valueColumn.ElementCount);

        reinterpret_cast<GPUGroupBy<OP, O, std::string, V>*>(
            dispatcher.groupByTables_[dispatcher.dispatcherThreadId_].get())
            ->ProcessBlock(std::get<0>(groupByColumn), reinterpret_cast<V*>(valueColumn.GpuPtr),
                           dataSize, std::get<2>(groupByColumn),
                           reinterpret_cast<int8_t*>(valueColumn.GpuNullMaskPtr));
    }

    static void GetResults(const std::vector<std::pair<std::string, DataType>>& groupByColumns,
                           const std::string& reg,
                           GpuSqlDispatcher& dispatcher)
    {
        std::string groupByColumnName = groupByColumns.begin()->first;
        int32_t outSize;
        GPUMemory::GPUString outKeys;
        int8_t* outKeyNullMask = nullptr;
        O* outValues = nullptr;
        int8_t* outValueNullMask = nullptr;
        reinterpret_cast<GPUGroupBy<OP, O, std::string, V>*>(
            dispatcher.groupByTables_[dispatcher.dispatcherThreadId_].get())
            ->GetResults(&outKeys, &outValues, &outSize, dispatcher.groupByTables_, &outKeyNullMask,
                         &outValueNullMask);
        dispatcher.FillStringRegister(outKeys, groupByColumnName + KEYS_SUFFIX, outSize, true, outKeyNullMask);
        dispatcher.InsertRegister(reg, PointerAllocation{reinterpret_cast<uintptr_t>(outValues), outSize, true,
                                                         reinterpret_cast<uintptr_t>(outValueNullMask)});
    }
};

template <typename OP, typename O, typename V>
class GpuSqlDispatcher::GroupByHelper<OP, O, std::vector<void*>, V>
{
public:
    static std::unique_ptr<IGroupBy>
    CreateInstance(int32_t groupByBuckets, const std::vector<std::pair<std::string, DataType>>& groupByColumns)
    {
        std::vector<DataType> keyDataTypes;

        for (auto& groupByColumn : groupByColumns)
        {
            keyDataTypes.push_back(groupByColumn.second);
        }

        return std::make_unique<GPUGroupBy<OP, O, std::vector<void*>, V>>(
            Configuration::GetInstance().GetGroupByBuckets(), keyDataTypes);
    }

    static void ProcessBlock(const std::vector<std::pair<std::string, DataType>>& groupByColumns,
                             const PointerAllocation& valueColumn,
                             GpuSqlDispatcher& dispatcher)
    {
        std::vector<void*> keyPtrs;
        std::vector<GPUMemory::GPUString*> stringKeyPtrs;
        int32_t minKeySize = std::numeric_limits<int32_t>::max();

        for (auto& groupByColumn : groupByColumns)
        {
            if (groupByColumn.second == DataType::COLUMN_STRING)
            {
                auto stringColumn = dispatcher.FindStringColumn(groupByColumn.first);
                GPUMemory::GPUString* stringColPtr;
                GPUMemory::alloc<GPUMemory::GPUString>(&stringColPtr, 1);

                GPUMemory::GPUString stringCol = std::get<0>(stringColumn);
                GPUMemory::copyHostToDevice<GPUMemory::GPUString>(stringColPtr, &stringCol, 1);
                keyPtrs.push_back(reinterpret_cast<void*>(stringColPtr));
                stringKeyPtrs.push_back(stringColPtr);

                minKeySize = std::min(std::get<1>(stringColumn), minKeySize);
            }
            else
            {
                PointerAllocation column = dispatcher.allocatedPointers_.at(groupByColumn.first);
                keyPtrs.push_back(reinterpret_cast<void*>(column.GpuPtr));
                minKeySize = std::min(column.ElementCount, minKeySize);
            }
        }

        int32_t dataSize = std::min(minKeySize, valueColumn.ElementCount);

        reinterpret_cast<GPUGroupBy<OP, O, std::vector<void*>, V>*>(
            dispatcher.groupByTables_[dispatcher.dispatcherThreadId_].get())
            ->GroupBy(keyPtrs, reinterpret_cast<V*>(valueColumn.GpuPtr), dataSize);

        for (auto& stringPtr : stringKeyPtrs)
        {
            GPUMemory::free(stringPtr);
        }
    }

    static void GetResults(const std::vector<std::pair<std::string, DataType>>& groupByColumns,
                           const std::string& reg,
                           GpuSqlDispatcher& dispatcher)
    {
        int32_t outSize;
        std::vector<void*> outKeys;
        O* outValues = nullptr;
        reinterpret_cast<GPUGroupBy<OP, O, std::vector<void*>, V>*>(
            dispatcher.groupByTables_[dispatcher.dispatcherThreadId_].get())
            ->GetResults(&outKeys, &outValues, &outSize, dispatcher.groupByTables_);

        for (int32_t i = 0; i < groupByColumns.size(); i++)
        {
            switch (groupByColumns[i].second)
            {
            case DataType::COLUMN_INT:
                dispatcher.InsertRegister(groupByColumns[i].first + KEYS_SUFFIX,
                                          PointerAllocation{reinterpret_cast<uintptr_t>(
                                                                reinterpret_cast<int32_t*>(outKeys[i])),
                                                            outSize, true, 0});
                break;
            case DataType::COLUMN_LONG:
                dispatcher.InsertRegister(groupByColumns[i].first + KEYS_SUFFIX,
                                          PointerAllocation{reinterpret_cast<uintptr_t>(
                                                                reinterpret_cast<int64_t*>(outKeys[i])),
                                                            outSize, true, 0});
                break;
            case DataType::COLUMN_FLOAT:
                dispatcher.InsertRegister(groupByColumns[i].first + KEYS_SUFFIX,
                                          PointerAllocation{reinterpret_cast<uintptr_t>(
                                                                reinterpret_cast<float*>(outKeys[i])),
                                                            outSize, true, 0});
                break;
            case DataType::COLUMN_DOUBLE:
                dispatcher.InsertRegister(groupByColumns[i].first + KEYS_SUFFIX,
                                          PointerAllocation{reinterpret_cast<uintptr_t>(
                                                                reinterpret_cast<double*>(outKeys[i])),
                                                            outSize, true, 0});
                break;
            case DataType::COLUMN_STRING:
                dispatcher.FillStringRegister(*(reinterpret_cast<GPUMemory::GPUString*>(outKeys[i])),
                                              groupByColumns[i].first + KEYS_SUFFIX, outSize, true);
                delete reinterpret_cast<GPUMemory::GPUString*>(outKeys[i]); // delete just pointer to struct
                break;
            case DataType::COLUMN_INT8_T:
                dispatcher.InsertRegister(groupByColumns[i].first + KEYS_SUFFIX,
                                          PointerAllocation{reinterpret_cast<uintptr_t>(
                                                                reinterpret_cast<int8_t*>(outKeys[i])),
                                                            outSize, true, 0});
                break;
            default:
                throw std::runtime_error("GROUP BY operation does not support data type " +
                                         std::to_string(groupByColumns[i].second));
                break;
            }
        }
        dispatcher.InsertRegister(reg, PointerAllocation{reinterpret_cast<uintptr_t>(outValues),
                                                         outSize, true, 0});
    }
};

/// Implementation of generic aggregation operation based on functor OP
/// Used when GROUP BY Clause is present
/// Loads data on demand
/// If WHERE clause is present filtering is done before agreggation
/// For each block it updates group by hash table
/// To handle multi-gpu functionality - each dipatcher instance signals when it processes its last block
/// The dispatcher instance handling the overall last block waits for all other dispatcher instances to finish their last blocks
/// and saves the result of group by
/// <returns name="statusCode">Finish status code of the operation</returns>
template <typename OP, typename O, typename K, typename V>
int32_t GpuSqlDispatcher::AggregationGroupBy()
{
    auto colTableName = arguments_.Read<std::string>();
    auto reg = arguments_.Read<std::string>();

    int32_t loadFlag = LoadCol<V>(colTableName);
    if (loadFlag)
    {
        return loadFlag;
    }

    std::cout << "AggGroupBy: " << colTableName << " " << reg << ", thread: " << dispatcherThreadId_
              << std::endl;


    PointerAllocation& column = allocatedPointers_.at(colTableName);
    int32_t reconstructOutSize;

    // Reconstruct column only if it is not group by column (if it is group by column it was already reconstructed in GroupByCol)
    if (std::find_if(groupByColumns_.begin(), groupByColumns_.end(), StringDataTypeComp(colTableName)) ==
        groupByColumns_.end())
    {
        V* reconstructOutReg;
        GPUReconstruct::reconstructColKeep<V>(&reconstructOutReg, &reconstructOutSize,
                                              reinterpret_cast<V*>(column.GpuPtr),
                                              reinterpret_cast<int8_t*>(filter_), column.ElementCount);

        if (column.ShouldBeFreed)
        {
            GPUMemory::free(reinterpret_cast<void*>(column.GpuPtr));
        }
        else
        {
            column.ShouldBeFreed = true;
        }
        column.GpuPtr = reinterpret_cast<uintptr_t>(reconstructOutReg);
        column.ElementCount = reconstructOutSize;
    }

    // TODO void param
    if (groupByTables_[dispatcherThreadId_] == nullptr)
    {
        groupByTables_[dispatcherThreadId_] = GpuSqlDispatcher::GroupByHelper<OP, O, K, V>::CreateInstance(
            Configuration::GetInstance().GetGroupByBuckets(), groupByColumns_);
    }

    if (aggregatedRegisters_.find(reg) == aggregatedRegisters_.end())
    {
        std::cout << "Processed block in AggGroupBy." << std::endl;
        GpuSqlDispatcher::GroupByHelper<OP, O, K, V>::ProcessBlock(groupByColumns_, column, *this);

        // If last block was processed, reconstruct group by table
        if (isLastBlockOfDevice_)
        {
            if (isOverallLastBlock_)
            {
                // Wait until all threads finished work
                std::unique_lock<std::mutex> lock(GpuSqlDispatcher::groupByMutex_);
                GpuSqlDispatcher::groupByCV_.wait(lock,
                                                  [] { return GpuSqlDispatcher::IsGroupByDone(); });

                std::cout << "Reconstructing group by in thread: " << dispatcherThreadId_ << std::endl;

                GpuSqlDispatcher::GroupByHelper<OP, O, K, V>::GetResults(groupByColumns_, reg, *this);
            }
            else
            {
                std::cout << "Group by all blocks done in thread: " << dispatcherThreadId_ << std::endl;
                // Increment counter and notify threads
                std::unique_lock<std::mutex> lock(GpuSqlDispatcher::groupByMutex_);
                GpuSqlDispatcher::IncGroupByDoneCounter();
                GpuSqlDispatcher::groupByCV_.notify_all();
            }
        }
        aggregatedRegisters_.insert(reg);
    }

    FreeColumnIfRegister<V>(colTableName);
    return 0;
}

/// This executes first (dor each block) when GROUP BY clause is used
/// It loads the group by column (if it is firt encountered reference to the column)
/// and filters it according to WHERE clause
/// <returns name="statusCode">Finish status code of the operation</returns>
template <typename T>
int32_t GpuSqlDispatcher::GroupByCol()
{
    std::string columnName = arguments_.Read<std::string>();

    int32_t loadFlag = LoadCol<T>(columnName);
    if (loadFlag)
    {
        return loadFlag;
    }

    std::cout << "GroupBy: " << columnName << std::endl;

    PointerAllocation& column = allocatedPointers_.at(columnName);

    int32_t reconstructOutSize;
    T* reconstructOutReg;
    GPUReconstruct::reconstructColKeep<T>(&reconstructOutReg, &reconstructOutSize,
                                          reinterpret_cast<T*>(column.GpuPtr),
                                          reinterpret_cast<int8_t*>(filter_), column.ElementCount);
    // TODO add null values to reconstruct

    if (column.ShouldBeFreed) // should be freed if it is not cached - if it is temp register like "YEAR(col)"
    {
        GPUMemory::free(reinterpret_cast<void*>(column.GpuPtr));
    }
    else
    {
        column.ShouldBeFreed = true; // enable future free in cleanupGpuPointers
    }

    // Now rewrite the pointer in the register (correct because the pointer is freed or stored in chache)
    column.GpuPtr = reinterpret_cast<uintptr_t>(reconstructOutReg);
    column.ElementCount = reconstructOutSize;

    if (std::find_if(groupByColumns_.begin(), groupByColumns_.end(), StringDataTypeComp(columnName)) ==
        groupByColumns_.end())
    {
        groupByColumns_.push_back({columnName, ::GetColumnType<T>()});
    }
    usingGroupBy_ = true;
    return 0;
}

template <typename T>
int32_t GpuSqlDispatcher::GroupByConst()
{
    return 0;
}
