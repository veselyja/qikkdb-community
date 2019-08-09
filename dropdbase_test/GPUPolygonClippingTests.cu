#include <cstdio>
#include <cstdlib>
#include <memory>
#include <random>
#include <vector>

#include "../dropdbase/QueryEngine/Context.h"
#include "../dropdbase/QueryEngine/GPUCore/GPUMemory.cuh"
#include "../dropdbase/QueryEngine/GPUCore/GPUPolygonClipping.cuh"

#include "gtest/gtest.h"

TEST(GPUPolygonClippingTests, PoygonTest)
{
	int32_t dataElementCount = 3;

	std::vector<int32_t> polyApolyIdx = {3, 4, 7};
	std::vector<int32_t> polyApointsIdx = {4, 8, 12, 15, 22, 27, 31};
	std::vector<NativeGeoPoint> polyApolyPoints = {
		{4.5, 5.5},
		{6.0, 5.5},
		{6.0, 4.5},
		{4.5, 4.5},

		{10.0, 0.0},
		{0.0, 0.0},
		{0.0, 10.0},
		{10.0, 10.0},

		{7.0, 7.0},
		{3.0, 7.0},
		{3.0, 3.0},
		{7.0, 3.0},

		{0.0, 0.0},
		{1.0, 0.0},
		{0.5, 1.0},

		{-6.31, -1.49},
		{-4, 5},
		{2.13, 6.03},
		{4.9, 2.23}, 
		{-0.52, -0.49},
		{3.88, -3.45},
		{-4.33, -3.89},

		{-3.77, 2.88},
		{1.12, 5.24},
		{3.52, 2.73},
		{-0.92, 0.45},
		{-2.82, -2.57},

		{-2.52, 1.91},
		{0.96, 4.25},
		{2.16, 2.81},
		{-1.98, 0.43}
	};

	std::vector<int32_t> polyBpolyIdx = {2, 3, 5};
	std::vector<int32_t> polyBpointsIdx = {3, 7, 11, 17, 23};
	std::vector<NativeGeoPoint> polyBpolyPoints = {
		{13.0, 5.5},
		{13.0, 4.5},
		{5.0, 5.0},

		{4.0, 4.0},
		{15.0, 4.0},
		{15.0, 6.0},
		{4.0, 6.0},

		{0.0, 0.4},
		{1.0, 0.4},
		{1.0, 0.6},
		{0.0, 0.6},

		{-5.12, 4.59},
		{0.42, -5.63},
		{3.86, 0.41},
		{2.06, 3.75},
		{1.22, 6.83},
		{-4.6, 6.45},

		{-3.32, 4.11},
		{0.92, 3.69},
		{2.26, 0.21},
		{0, -3}, 
		{0.48, 1.65}, 
		{-2.94, 2.27}
	};

	std::vector<int32_t> outPolyIdxCorrect = {};
	std::vector<int32_t> outPointIdxCorrect = {};
	std::vector<NativeGeoPoint> outPolyPointsCorrect = {};

	std::vector<int32_t> outPolyIdx;
	std::vector<int32_t> outPointIdx;
	std::vector<NativeGeoPoint> outPolyPoints;

	// Alloc the GPU structures
	GPUMemory::GPUPolygon polygonA;
	GPUMemory::GPUPolygon polygonB;

	GPUMemory::alloc(&polygonA.polyIdx, polyApolyIdx.size());
	GPUMemory::alloc(&polygonA.pointIdx, polyApointsIdx.size());
	GPUMemory::alloc(&polygonA.polyPoints, polyApolyPoints.size());

	GPUMemory::alloc(&polygonB.polyIdx, polyBpolyIdx.size());
	GPUMemory::alloc(&polygonB.pointIdx, polyBpointsIdx.size());
	GPUMemory::alloc(&polygonB.polyPoints, polyBpolyPoints.size());

	// Copy the input data to the GPU
	GPUMemory::copyHostToDevice(polygonA.polyIdx, &polyApolyIdx[0], polyApolyIdx.size());
	GPUMemory::copyHostToDevice(polygonA.pointIdx, &polyApointsIdx[0], polyApointsIdx.size());
	GPUMemory::copyHostToDevice(polygonA.polyPoints, &polyApolyPoints[0], polyApolyPoints.size());

	GPUMemory::copyHostToDevice(polygonB.polyIdx, &polyBpolyIdx[0], polyBpolyIdx.size());
	GPUMemory::copyHostToDevice(polygonB.pointIdx, &polyBpointsIdx[0], polyBpointsIdx.size());
	GPUMemory::copyHostToDevice(polygonB.polyPoints, &polyBpolyPoints[0], polyBpolyPoints.size());

	// Perform the polygon intersect
	GPUMemory::GPUPolygon polygonOut;	// This needs to be empty
	GPUPolygonClipping::ColCol<PolygonFunctions::polyIntersect>(polygonOut, polygonA, polygonB, dataElementCount);

	// Copy back the results
	outPolyIdx.resize(dataElementCount);
	GPUMemory::copyDeviceToHost(&outPolyIdx[0], polygonOut.polyIdx, outPolyIdx.size());

	outPointIdx.resize(outPolyIdx[dataElementCount - 1]);
	GPUMemory::copyDeviceToHost(&outPointIdx[0], polygonOut.pointIdx, outPointIdx.size());

	outPolyPoints.resize(outPointIdx[outPolyIdx[dataElementCount - 1] - 1]);
	GPUMemory::copyDeviceToHost(&outPolyPoints[0], polygonOut.polyPoints, outPolyPoints.size());

	// Free the polygons
	GPUMemory::free(polygonA);
	GPUMemory::free(polygonB);
	GPUMemory::free(polygonOut);

	// Check the results

	// DEBUG
	for(int32_t i = 0; i < outPolyIdx.size(); i++)
	{
		printf("%d : %d\n", i, outPolyIdx[i]);
	}
	printf("\n");

	for(int32_t i = 0; i < outPointIdx.size(); i++)
	{
		printf("%d : %d\n", i, outPointIdx[i]);
	}
	printf("\n");

	for(int32_t i = 0; i < outPolyPoints.size(); i++)
	{
		printf("%d : %.2f %.2f\n", i, outPolyPoints[i].latitude,outPolyPoints[i].longitude);
	}
	printf("\n");

}