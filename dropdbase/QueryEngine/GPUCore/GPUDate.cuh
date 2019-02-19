#pragma once

#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include <cstdint>

#include "../Context.h"
#include "../QueryEngineError.h"
#include "MaybeDeref.cuh"

namespace DateOperations
{
	constexpr int64_t SECONDS_BETWEEN_1600_1970 = 11676096000i64;			// number of seconds between years 1600 and 1970
	constexpr int64_t DAYS_IN_400_YEARS = 146097i64;						// days in 400 years cycle (e.g. between 1600 and 2000)
	constexpr int64_t SECONDS_IN_400_YEARS = DAYS_IN_400_YEARS * 86400i64;	// seconds in 400 years cycle (e.g. between 1600 and 2000)
	
	__constant__ int64_t START_DAYS_OF_LEAP_YEARS[] = { 0, 1461, 2922, 4383, 5844, 7305, 8766, 10227, 11688, 13149, 14610, 16071, 17532, 18993, 20454, 21915, 23376, 24837, 26298, 27759, 29220, 30681, 32142, 33603, 35064,
		37985, 39446, 40907, 42368, 43829, 45290, 46751, 48212, 49673, 51134, 52595, 54056, 55517, 56978, 58439, 59900, 61361, 62822, 64283, 65744, 67205, 68666, 70127, 71588,
		74509, 75970, 77431, 78892, 80353, 81814, 83275, 84736, 86197, 87658, 89119, 90580, 92041, 93502, 94963, 96424, 97885, 99346, 100807, 102268, 103729, 105190, 106651, 108112,
		111033, 112494, 113955, 115416, 116877, 118338, 119799, 121260, 122721, 124182, 125643, 127104, 128565, 130026, 131487, 132948, 134409, 135870, 137331, 138792, 140253, 141714, 143175, 144636 };

	__constant__ int64_t START_DAYS_OF_MONTHS[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335,
		366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700,
		731, 762, 790, 821, 851, 882, 912, 943, 974, 1004, 1035, 1065,
		1096, 1127, 1155, 1186, 1216, 1247, 1277, 1308, 1339, 1369, 1400, 1430,
		1461, 1492, 1520, 1551, 1581, 1612, 1642, 1673, 1704, 1734, 1765, 1795,
		1826, 1857, 1885, 1916, 1946, 1977, 2007, 2038, 2069, 2099, 2130, 2160,
		2191, 2222, 2250, 2281, 2311, 2342, 2372, 2403, 2434, 2464, 2495, 2525,
		2556, 2587, 2615, 2646, 2676, 2707, 2737, 2768, 2799, 2829, 2860, 2890 };

	struct year
	{
		__device__ int32_t operator()(int64_t dateTime) const
		{
			int32_t year400;
			int64_t value = dateTime + SECONDS_BETWEEN_1600_1970;
			// value is now seconds from 1.1.1600 0:00:00
			
			if (value < 0)
			{
				year400 = static_cast<int32_t>((value - SECONDS_IN_400_YEARS + 1i64) / SECONDS_IN_400_YEARS);
				value = (((value + 1i64) % SECONDS_IN_400_YEARS) + SECONDS_IN_400_YEARS - 1i64) / 86400i64;
			}
			else
			{
				year400 = static_cast<int32_t>(value / SECONDS_IN_400_YEARS);
				value = (value / 86400i64) % DAYS_IN_400_YEARS;
			}
			// value is now count of days from start of some 400 years cycle

			// Heuristic for finding index to leap years table
			int32_t index = min(96, static_cast<int32_t>(value * 97i64 / DAYS_IN_400_YEARS + 1i64));
			while (START_DAYS_OF_LEAP_YEARS[index] > value)	// this cycle will run max. 2 iterations
			{
				--index;
			}
			int32_t year4 = static_cast<int32_t>((START_DAYS_OF_LEAP_YEARS[index] + 730i64) / 1461i64);
			value -= START_DAYS_OF_LEAP_YEARS[index];
			// value is now count of days from start of some 4 or 8 year cycle between 2 leap years

			// Find index to months table (this one is exact)
			index = min(7, static_cast<int32_t>(value * 8i64 / 2921i64));

			return 1600 + year400 * 400 + year4 * 4 + index;
		}
	};

	struct month
	{
		__device__ int32_t operator()(int64_t dateTime) const
		{
			int64_t value = dateTime + SECONDS_BETWEEN_1600_1970;
			// value is now seconds from 1.1.1600 0:00:00

			if (value < 0)
			{
				value = (((value + 1i64) % SECONDS_IN_400_YEARS) + SECONDS_IN_400_YEARS - 1i64) / 86400i64;
			}
			else
			{
				value = (value / 86400i64) % DAYS_IN_400_YEARS;
			}
			// value is now count of days from start of some 400 years cycle

			// Heuristic for finding index to leap years table
			int32_t index = min(96, static_cast<int32_t>(value * 97i64 / DAYS_IN_400_YEARS + 1i64));
			while (START_DAYS_OF_LEAP_YEARS[index] > value)	// this cycle will run max. 2 iterations
			{
				--index;
			}
			value -= START_DAYS_OF_LEAP_YEARS[index];
			// value is now count of days from start of some 4 or 8 year cycle between 2 leap years

			// Heuristic for finding index to months table
			index = min(95, static_cast<int32_t>(value * 96i64 / 2921i64 + 1i64));
			while (START_DAYS_OF_MONTHS[index] > value)		// this cycle will run max. 2 iterations
			{
				--index;
			}
			return index % 12 + 1;
		}
	};

	struct day
	{
		__device__ int32_t operator()(int64_t dateTime) const
		{
			int64_t value = dateTime + SECONDS_BETWEEN_1600_1970;
			// value is now seconds from 1.1.1600 0:00:00

			if (value < 0)
			{
				value = (((value + 1i64) % SECONDS_IN_400_YEARS) + SECONDS_IN_400_YEARS - 1i64) / 86400i64;
			}
			else
			{
				value = (value / 86400i64) % DAYS_IN_400_YEARS;
			}
			// value is now count of days from start of some 400 years cycle

			// Heuristic for finding index to leap years table
			int32_t index = min(96, static_cast<int32_t>(value * 97i64 / DAYS_IN_400_YEARS + 1i64));
			while (START_DAYS_OF_LEAP_YEARS[index] > value)	// this cycle will run max. 2 iterations
			{
				--index;
			}
			value -= START_DAYS_OF_LEAP_YEARS[index];
			// value is now count of days from start of some 4 or 8 year cycle between 2 leap years

			// Heuristic for finding index to months table
			index = min(95, static_cast<int32_t>(value * 96i64 / 2921i64 + 1i64));
			while (START_DAYS_OF_MONTHS[index] > value)		// this cycle will run max. 2 iterations
			{
				--index;
			}
			value -= START_DAYS_OF_MONTHS[index];
			return static_cast<int32_t>(value) + 1;
		}
	};

	struct hour
	{
		__device__ int32_t operator()(int64_t dateTime) const
		{
			if (dateTime < 0)
			{
				return static_cast<int32_t>((((dateTime + 1i64) % 86400i64) + 86399i64) / 3600i64);
			}
			else
			{
				return static_cast<int32_t>((dateTime / 3600i64) % 24i64);
			}
		}
	};

	struct minute
	{
		__device__ int32_t operator()(int64_t dateTime) const
		{
			if (dateTime < 0)
			{
				return static_cast<int32_t>((((dateTime + 1i64) % 3600i64) + 3599i64) / 60i64);
			}
			else
			{
				return static_cast<int32_t>((dateTime / 60i64) % 60i64);
			}
		}
	};

	struct second
	{
		__device__ int32_t operator()(int64_t dateTime) const
		{
			if (dateTime < 0)
			{
				return static_cast<int32_t>(((dateTime + 1i64) % 60i64) + 59i64);
			}
			else
			{
				return static_cast<int32_t>(dateTime % 60i64);
			}
		}
	};
}


/// <summary>
/// Kernel for extracting date or time variable (e.g. days, hours)
/// from datetime column or constant
/// </summary>
/// <param name="output">block of the result data</param>
/// <param name="dateTimeCol">input timestamp (column or constant)</param>
/// <param name="dataElementCount">the count of elements in the input block
/// (or of output block if input is constant)</param>
template<typename OP, typename T>
__global__ void kernel_extract(int32_t * output, T dateTimeCol, int32_t dataElementCount)
{
	const int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
	const int32_t stride = blockDim.x * gridDim.x;

	for (int32_t i = idx; i < dataElementCount; i += stride)
	{
		output[i] = OP{}.operator()(maybe_deref(dateTimeCol, i));
	}
}

/// <summary>
/// GPUDate class is for extracting (conversion) variables (e.g. days, hours)
/// from datetime column or constant
/// </summary>
class GPUDate
{
public:
	template<typename OP>
	static void extractCol(int32_t * output, int64_t * dateTimeCol, int32_t dataElementCount)
	{
		kernel_extract <OP> << < Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim() >> >
			(output, dateTimeCol, dataElementCount);
		cudaDeviceSynchronize();
		QueryEngineError::setCudaError(cudaGetLastError());
	}

	template<typename OP>
	static void extractConst(int32_t * output, int64_t dateTimeConst, int32_t dataElementCount)
	{
		kernel_extract <OP> << < Context::getInstance().calcGridDim(dataElementCount), Context::getInstance().getBlockDim() >> >
			(output, dateTimeConst, dataElementCount);
		cudaDeviceSynchronize();
		QueryEngineError::setCudaError(cudaGetLastError());
	}

};