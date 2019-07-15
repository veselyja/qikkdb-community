#pragma once

#include "../GpuSqlDispatcher.h"
#include "../../QueryEngine/GPUCore/GPUOrderBy.cuh"
#include "../../QueryEngine/GPUCore/GPUReconstruct.cuh"
#include "../../QueryEngine/OrderByType.h"
#include "../../QueryEngine/GPUCore/cuda_ptr.h"
#include "../../VariantArray.h"

template<typename T>
int32_t GpuSqlDispatcher::orderByCol()
{
	auto colName = arguments.read<std::string>();
	OrderBy::Order order = static_cast<OrderBy::Order>(arguments.read<int32_t>());
	int32_t columnIndex = arguments.read<int32_t>();

	orderByColumns.insert({ columnIndex, {colName, order} });

	int32_t loadFlag = loadCol<T>(colName);
	if (loadFlag)
	{
		return loadFlag;
	}
	
	if (usingGroupBy)
	{
		if (isOverallLastBlock)
		{
			std::cout << "Order by: " << colName << std::endl;
			PointerAllocation column = allocatedPointers.at(getAllocatedRegisterName(colName) + (groupByColumns.find(colName) != groupByColumns.end() ? "_keys" : ""));
			int32_t inSize = column.elementCount;

			if (orderByTable == nullptr)
			{
				orderByTable = std::make_unique<GPUOrderBy>(inSize);
				int32_t* orderByIndices = allocateRegister<int32_t>("$orderByIndices", inSize);
				usingOrderBy = true;
			}

			PointerAllocation orderByIndices = allocatedPointers.at("$orderByIndices");
			orderByTable->OrderByColumn(
				reinterpret_cast<int32_t*>(orderByIndices.gpuPtr),
				reinterpret_cast<T*>(column.gpuPtr),
				reinterpret_cast<int8_t*>(column.gpuNullMaskPtr),
				inSize,
				order);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		std::cout << "Order by: " << colName << std::endl;
		PointerAllocation column = allocatedPointers.at(getAllocatedRegisterName(colName));
		int32_t inSize = column.elementCount;

		if (orderByTable == nullptr)
		{
			orderByTable = std::make_unique<GPUOrderBy>(inSize);
			int32_t* orderByIndices = allocateRegister<int32_t>("$orderByIndices", inSize);
			usingOrderBy = true;
		}

		PointerAllocation orderByIndices = allocatedPointers.at("$orderByIndices");
		orderByTable->OrderByColumn(
			reinterpret_cast<int32_t*>(orderByIndices.gpuPtr),
			reinterpret_cast<T*>(column.gpuPtr),
			reinterpret_cast<int8_t*>(column.gpuNullMaskPtr),
			inSize,
			order);
	}	

	return 0;
}

template<typename T>
int32_t GpuSqlDispatcher::orderByConst()
{
	return 0;
}

template<typename T>
int32_t GpuSqlDispatcher::orderByReconstructOrderCol()
{
	auto colName = arguments.read<std::string>();

	if (!usingGroupBy)
	{
		std::cout << "Reordering order by column: " << colName << std::endl;

		int32_t loadFlag = loadCol<T>(colName);
		if (loadFlag)
		{
			return loadFlag;
		}

		PointerAllocation col = allocatedPointers.at(getAllocatedRegisterName(colName));
		size_t inSize = col.elementCount;

		std::unique_ptr<VariantArray<T>> outData = std::make_unique<VariantArray<T>>(inSize);

		cuda_ptr<T> reorderedColumn(inSize);

		PointerAllocation orderByIndices = allocatedPointers.at("$orderByIndices");
		GPUOrderBy::ReOrderByIdx(reorderedColumn.get(), reinterpret_cast<int32_t*>(orderByIndices.gpuPtr), reinterpret_cast<T*>(col.gpuPtr), col.elementCount);

		int32_t outSize;
		GPUReconstruct::reconstructCol(outData->getData(), &outSize, reorderedColumn.get(), reinterpret_cast<int8_t*>(filter_) , inSize);
		outData->resize(outSize);

		orderByBlocks[dispatcherThreadId].reconstructedOrderByOrderColumnBlocks[colName].push_back(std::move(outData));
	}
	return 0;
}

template<typename T>
int32_t GpuSqlDispatcher::orderByReconstructOrderConst()
{
	return 0;
}

template<typename T>
int32_t GpuSqlDispatcher::orderByReconstructRetCol()
{
	auto colName = arguments.read<std::string>();

	if (!usingGroupBy)
	{
		std::cout << "Recostructing order by return column: " << colName << std::endl;

		int32_t loadFlag = loadCol<T>(colName);
		if (loadFlag)
		{
			return loadFlag;
		}

		// !!! NULL reconstruction is needed here only - for returning the results

		PointerAllocation col = allocatedPointers.at(getAllocatedRegisterName(colName));
		int32_t inSize = col.elementCount;
		size_t inNullColSize = (inSize + sizeof(int8_t) * 8 - 1) / (sizeof(int8_t) * 8);

		std::unique_ptr<VariantArray<T>> outData = std::make_unique<VariantArray<T>>(inSize);
		std::unique_ptr<int8_t[]> outNullData = std::make_unique<int8_t[]>(inNullColSize);

		cuda_ptr<T> reorderedColumn(inSize);
		cuda_ptr<int8_t> reorderedNullColumn(inNullColSize);

		PointerAllocation orderByIndices = allocatedPointers.at("$orderByIndices");
		GPUOrderBy::ReOrderByIdx(reorderedColumn.get(), reinterpret_cast<int32_t*>(orderByIndices.gpuPtr), reinterpret_cast<T*>(col.gpuPtr), col.elementCount);
		GPUOrderBy::ReOrderNullValuesByIdx(reorderedNullColumn.get(), 
										   reinterpret_cast<int32_t*>(orderByIndices.gpuPtr),
										   reinterpret_cast<int8_t*>(col.gpuNullMaskPtr), 
										   inSize);

		int32_t outSize;
		GPUReconstruct::reconstructCol(outData->getData(), &outSize, reorderedColumn.get(), reinterpret_cast<int8_t*>(filter_) , inSize, outNullData.get(), reorderedNullColumn.get());
		outData->resize(outSize);

		orderByBlocks[dispatcherThreadId].reconstructedOrderByRetColumnBlocks[colName].push_back(std::move(outData));
	}
	return 0;
}

template<typename T>
int32_t GpuSqlDispatcher::orderByReconstructRetConst()
{
	return 0;
}
