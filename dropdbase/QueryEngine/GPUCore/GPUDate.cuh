#pragma once

#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include <cstdint>

#include "../Context.h"
#include "../GPUError.h"
#include "MaybeDeref.cuh"
#include "DateOperations.h"
#include "cuda_ptr.h"
#include "GPUReconstruct.cuh"
#include "GPUMemory.cuh"
#include "GPUArithmeticUnary.cuh"

__global__ void kernel_fill_date_string(GPUMemory::GPUString outCol,
                                        int32_t* years,
                                        int32_t* months,
                                        int32_t* days,
                                        int32_t* hours,
                                        int32_t* minutes,
                                        int32_t* seconds,
                                        int32_t dataElementCount);

/// GPUDate class is for extracting (conversion) variables (e.g. days, hours)
/// from datetime column or constant
class GPUDate
{
public:
    template <typename T>
    static void DateToString(GPUMemory::GPUString* output, T dateTimeCol, int32_t dataElementCount)
    {
        static_assert(std::is_same<typename std::remove_pointer<T>::type, int64_t>::value,
                      "DateTime can only be extracted from int64 columns");

        if (dataElementCount > 0)
        {

            // Length of date format "YYYY-MM-DD HH:MM:SS" = 19

            const int32_t dateFormatLength = 19;

            GPUMemory::alloc(&(output->stringIndices), dataElementCount);

            cuda_ptr<int32_t> lengths(dataElementCount);
            kernel_fill_array<<<Context::getInstance().calcGridDim(dataElementCount),
                                Context::getInstance().getBlockDim()>>>(lengths.get(), dateFormatLength,
                                                                        dataElementCount);

            GPUReconstruct::PrefixSum(output->stringIndices, lengths.get(), dataElementCount);

            int64_t totalCharCount;
            GPUMemory::copyDeviceToHost(&totalCharCount, output->stringIndices + dataElementCount - 1, 1);
            GPUMemory::alloc(&(output->allChars), totalCharCount);

            cuda_ptr<int32_t> years(dataElementCount);
            cuda_ptr<int32_t> months(dataElementCount);
            cuda_ptr<int32_t> days(dataElementCount);
            cuda_ptr<int32_t> hours(dataElementCount);
            cuda_ptr<int32_t> minutes(dataElementCount);
            cuda_ptr<int32_t> seconds(dataElementCount);


            kernel_arithmetic_unary<DateOperations::year, int32_t, T>
                <<<Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim()>>>(
                    years.get(), dateTimeCol, dataElementCount);

            kernel_arithmetic_unary<DateOperations::month, int32_t, T>
                <<<Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim()>>>(
                    months.get(), dateTimeCol, dataElementCount);

            kernel_arithmetic_unary<DateOperations::day, int32_t, T>
                <<<Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim()>>>(
                    days.get(), dateTimeCol, dataElementCount);

            kernel_arithmetic_unary<DateOperations::hour, int32_t, T>
                <<<Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim()>>>(
                    hours.get(), dateTimeCol, dataElementCount);

            kernel_arithmetic_unary<DateOperations::minute, int32_t, T>
                <<<Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim()>>>(
                    minutes.get(), dateTimeCol, dataElementCount);

            kernel_arithmetic_unary<DateOperations::second, int32_t, T>
                <<<Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim()>>>(
                    seconds.get(), dateTimeCol, dataElementCount);

            kernel_fill_date_string<<<Context::getInstance().calcGridDim(dataElementCount),
                                      Context::getInstance().getBlockDim()>>>(*output, years.get(),
                                                                              months.get(), days.get(),
                                                                              hours.get(), minutes.get(),
                                                                              seconds.get(), dataElementCount);
            cudaDeviceSynchronize();
            CheckCudaError(cudaGetLastError());
        }
        else
        {
            output->allChars = nullptr;
            output->allChars = nullptr;
        }
    }
};
