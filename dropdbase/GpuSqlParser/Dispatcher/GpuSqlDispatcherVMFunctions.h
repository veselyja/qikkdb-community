#pragma once
#include "../GpuSqlDispatcher.h"
#include "../../QueryEngine/GPUCore/GPUReconstruct.cuh"
#include "../../QueryEngine/GPUCore/GPUMemory.cuh"
#include "../../QueryEngine/GPUCore/GPUOrderBy.cuh"
#include "../../QueryEngine/GPUCore/GPUJoin.cuh"
#include "../../IVariantArray.h"
#include "../../VariantArray.h"
#include "../../Database.h"
#include "../../Table.h"
#include "../../ColumnBase.h"
#include "../../BlockBase.h"
#include "../../CudaLogBoost.h"

template <typename T>
int32_t GpuSqlDispatcher::RetConst()
{
    T cnst = arguments_.Read<T>();
    std::cout << "RET: cnst" << typeid(T).name() << " " << cnst << std::endl;
    std::string _ = arguments_.Read<std::string>();
    ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
    std::unique_ptr<T[]> outData(new T[1]);
    outData[0] = cnst;
    InsertIntoPayload(payload, outData, 1);
    MergePayloadToSelfResponse(std::to_string(cnst), payload, "");
    return 0;
}

/// Implementation of column return from SELECT clause
/// If GROUP BY clause is not present each column block is reconstructed based on the filter mask
/// (generated from WHERE clause) and merged to response message
/// If GROUP BY is present nothing is reconstructed as the filtering was done prior to GROUP BY (in
/// aggregation) If GROUP BY is present the results are only coppied from GPU and merged to response
/// message <returns name="statusCode">Finish status code of the operation</returns>
template <typename T>
int32_t GpuSqlDispatcher::RetCol()
{
    auto colName = arguments_.Read<std::string>();
    auto alias = arguments_.Read<std::string>();

    int32_t loadFlag = LoadCol<T>(colName);
    if (loadFlag)
    {
        return loadFlag;
    }

    CudaLogBoost::getInstance(CudaLogBoost::info)
        << "RetCol: " << colName << ", thread: " << dispatcherThreadId_ << '\n';

    int32_t outSize;
    std::unique_ptr<T[]> outData;
    std::string nullMaskString = "";
    if (usingGroupBy_)
    {
        if (isOverallLastBlock_)
        {
            PointerAllocation col = allocatedPointers_.at(
                colName + (std::find_if(groupByColumns_.begin(), groupByColumns_.end(),
                                        StringDataTypeComp(colName)) != groupByColumns_.end() ?
                               KEYS_SUFFIX :
                               ""));
            outSize = col.ElementCount;
            if (usingOrderBy_)
            {
                CudaLogBoost::getInstance(CudaLogBoost::info) << "Reordering result block." << '\n';
                PointerAllocation orderByIndices = allocatedPointers_.at("$orderByIndices");
                GPUOrderBy::ReOrderByIdxInplace(reinterpret_cast<T*>(col.GpuPtr),
                                                reinterpret_cast<int32_t*>(orderByIndices.GpuPtr), outSize);
            }

            outData = std::make_unique<T[]>(outSize);
            GPUMemory::copyDeviceToHost(outData.get(), reinterpret_cast<T*>(col.GpuPtr), outSize);
            if (col.GpuNullMaskPtr)
            {
                size_t bitMaskSize = (outSize + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                std::unique_ptr<int8_t[]> nullMask = std::unique_ptr<int8_t[]>(new int8_t[bitMaskSize]);
                GPUMemory::copyDeviceToHost(nullMask.get(),
                                            reinterpret_cast<int8_t*>(col.GpuNullMaskPtr), bitMaskSize);
                nullMaskString = std::string(reinterpret_cast<char*>(nullMask.get()), bitMaskSize);
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (usingOrderBy_)
        {
            if (isOverallLastBlock_)
            {
                VariantArray<T>* reconstructedColumn =
                    dynamic_cast<VariantArray<T>*>(reconstructedOrderByColumnsMerged_.at(colName).get());
                outData = std::move(reconstructedColumn->getDataRef());
                outSize = reconstructedColumn->GetSize();

                size_t bitMaskSize = (outSize + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                nullMaskString = std::string(reinterpret_cast<char*>(
                                                 reconstructedOrderByColumnsNullMerged_.at(colName).get()),
                                             bitMaskSize);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            PointerAllocation col = allocatedPointers_.at(colName);
            int32_t inSize = col.ElementCount;
            outData = std::make_unique<T[]>(inSize);
            // ToDo: Podmienene zapnut podla velkost buffera
            // GPUMemory::hostPin(outData.get(), inSize);
            if (col.GpuNullMaskPtr)
            {
                size_t bitMaskSize = (database_->GetBlockSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                std::unique_ptr<int8_t[]> nullMask = std::unique_ptr<int8_t[]>(new int8_t[bitMaskSize]);
                GPUReconstruct::reconstructCol(outData.get(), &outSize, reinterpret_cast<T*>(col.GpuPtr),
                                               reinterpret_cast<int8_t*>(filter_), col.ElementCount,
                                               nullMask.get(), reinterpret_cast<int8_t*>(col.GpuNullMaskPtr));
                bitMaskSize = (outSize + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                nullMaskString = std::string(reinterpret_cast<char*>(nullMask.get()), bitMaskSize);
            }
            else
            {
                GPUReconstruct::reconstructCol(outData.get(), &outSize, reinterpret_cast<T*>(col.GpuPtr),
                                               reinterpret_cast<int8_t*>(filter_), col.ElementCount);
            }
            // GPUMemory::hostUnregister(outData.get());
            CudaLogBoost::getInstance(CudaLogBoost::info) << "dataSize: " << outSize << '\n';
        }
    }

    if (outSize > 0)
    {
        ColmnarDB::NetworkClient::Message::QueryResponsePayload payload;
        InsertIntoPayload(payload, outData, outSize);
        MergePayloadToSelfResponse(alias, payload, nullMaskString);
    }
    return 0;
}

/// Implementation of the LOAD operation
/// Loads the current block of given column
/// Sets the last block (for current dispatcher instance and overall) flags
/// <returns name="statusCode">Finish status code of the operation</returns>
template <typename T>
int32_t GpuSqlDispatcher::LoadCol(std::string& colName)
{
    if (allocatedPointers_.find(colName) == allocatedPointers_.end() && !colName.empty() && colName.front() != '$')
    {
        CudaLogBoost::getInstance(CudaLogBoost::info) << "Load: " << colName << " " << typeid(T).name() << '\n';

        std::string table;
        std::string column;

        std::tie(table, column) = SplitColumnName(colName);

        const int32_t blockCount =
            usingJoin_ ? joinIndices_->at(table).size() :
                         database_->GetTables().at(table).GetColumns().at(column).get()->GetBlockCount();
        GpuSqlDispatcher::deviceCountLimit_ =
            std::min(Context::getInstance().getDeviceCount() - 1, blockCount - 1);
        if (blockIndex_ >= blockCount)
        {
            return 1;
        }
        if (blockIndex_ >= blockCount - Context::getInstance().getDeviceCount())
        {
            isLastBlockOfDevice_ = true;
        }
        if (blockIndex_ == blockCount - 1)
        {
            isOverallLastBlock_ = true;
        }

        noLoad_ = false;

        if (loadNecessary_ == 0)
        {
            instructionPointer_ = jmpInstructionPosition_;
            return 12;
        }

        const ColumnBase<T>* col = dynamic_cast<const ColumnBase<T>*>(
            database_->GetTables().at(table).GetColumns().at(column).get());

        if (!usingJoin_)
        {
            int8_t* nullMaskPtr = nullptr;
            auto block = dynamic_cast<BlockBase<T>*>(col->GetBlocksList()[blockIndex_]);
            size_t realSize;
            std::tuple<T*, size_t, bool> cacheEntry;
            if (block->IsCompressed())
            {
                size_t uncompressedSize = Compression::GetUncompressedDataElementsCount(block->GetData());
                size_t compressedSize = block->GetSize();
                cacheEntry =
                    Context::getInstance().getCacheForCurrentDevice().getColumn<T>(database_->GetName(),
                                                                                   colName, blockIndex_,
                                                                                   uncompressedSize);
                if (!std::get<2>(cacheEntry))
                {
                    T* deviceCompressed;
                    GPUMemory::alloc(&deviceCompressed, compressedSize);
                    GPUMemory::copyHostToDevice(deviceCompressed, block->GetData(), compressedSize);
                    bool isDecompressed;
                    Compression::Decompress(col->GetColumnType(), deviceCompressed,
                                            Compression::GetCompressedDataElementsCount(block->GetData()),
                                            std::get<0>(cacheEntry),
                                            Compression::GetUncompressedDataElementsCount(block->GetData()),
                                            Compression::GetCompressionBlocksCount(block->GetData()),
                                            block->GetMin(), block->GetMax(), isDecompressed, true);
                    GPUMemory::free(deviceCompressed);
                }

                realSize = uncompressedSize;
            }
            else
            {
                cacheEntry =
                    Context::getInstance().getCacheForCurrentDevice().getColumn<T>(database_->GetName(),
                                                                                   colName, blockIndex_,
                                                                                   block->GetSize());
                if (!std::get<2>(cacheEntry))
                {
                    GPUMemory::copyHostToDevice(std::get<0>(cacheEntry), block->GetData(), block->GetSize());
                }

                realSize = block->GetSize();
            }

            if (block->GetNullBitmask())
            {
                if (allocatedPointers_.find(colName + NULL_SUFFIX) == allocatedPointers_.end())
                {
                    int32_t bitMaskCapacity = ((realSize + sizeof(int8_t) * 8 - 1) / (8 * sizeof(int8_t)));
                    auto cacheMaskEntry = Context::getInstance().getCacheForCurrentDevice().getColumn<int8_t>(
                        database_->GetName(), colName + NULL_SUFFIX, blockIndex_, bitMaskCapacity);
                    nullMaskPtr = std::get<0>(cacheMaskEntry);
                    if (!std::get<2>(cacheMaskEntry))
                    {
                        GPUMemory::copyHostToDevice(std::get<0>(cacheMaskEntry),
                                                    block->GetNullBitmask(), bitMaskCapacity);
                    }
                    AddCachedRegister(colName + NULL_SUFFIX, std::get<0>(cacheMaskEntry), bitMaskCapacity);
                }
                else
                {
                    nullMaskPtr =
                        reinterpret_cast<int8_t*>(allocatedPointers_.at(colName + NULL_SUFFIX).GpuPtr);
                }
            }
            AddCachedRegister(colName, std::get<0>(cacheEntry), realSize, nullMaskPtr);
            noLoad_ = false;
        }

        else
        {
            CudaLogBoost::getInstance(CudaLogBoost::info) << "Loading joined block." << '\n';
            int32_t loadSize = joinIndices_->at(table)[blockIndex_].size();
            std::string joinCacheId = colName + "_join";
            for (auto& joinTable : *joinIndices_)
            {
                joinCacheId += "_" + joinTable.first;
            }

            auto cacheEntry =
                Context::getInstance().getCacheForCurrentDevice().getColumn<T>(database_->GetName(), joinCacheId,
                                                                               blockIndex_, loadSize);
            int8_t* nullMaskPtr = nullptr;

            if (!std::get<2>(cacheEntry))
            {
                int32_t outDataSize;
                GPUJoin::reorderByJoinTableCPU<T>(std::get<0>(cacheEntry), outDataSize, *col, blockIndex_,
                                                  joinIndices_->at(table), database_->GetBlockSize());
            }

            if (col->GetIsNullable())
            {
                if (allocatedPointers_.find(colName + NULL_SUFFIX) == allocatedPointers_.end())
                {
                    int32_t bitMaskCapacity = ((loadSize + sizeof(int8_t) * 8 - 1) / (8 * sizeof(int8_t)));
                    auto cacheMaskEntry = Context::getInstance().getCacheForCurrentDevice().getColumn<int8_t>(
                        database_->GetName(), joinCacheId + NULL_SUFFIX, blockIndex_, bitMaskCapacity);
                    nullMaskPtr = std::get<0>(cacheMaskEntry);

                    if (!std::get<2>(cacheMaskEntry))
                    {
                        int32_t outMaskSize;
                        GPUJoin::reorderNullMaskByJoinTableCPU<T>(std::get<0>(cacheMaskEntry), outMaskSize,
                                                                  *col, blockIndex_, joinIndices_->at(table),
                                                                  database_->GetBlockSize());
                    }
                    AddCachedRegister(colName + NULL_SUFFIX, std::get<0>(cacheMaskEntry), bitMaskCapacity);
                }
                else
                {
                    nullMaskPtr =
                        reinterpret_cast<int8_t*>(allocatedPointers_.at(colName + NULL_SUFFIX).GpuPtr);
                }
            }
            AddCachedRegister(colName, std::get<0>(cacheEntry), loadSize, nullMaskPtr);
            noLoad_ = false;
        }
    }
    return 0;
}