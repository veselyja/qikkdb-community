#include "ComplexPolygonFactory.h"
#include <sstream>
#include <stdexcept>
#include <regex>
#include <algorithm>
#include <iomanip>
#include "QueryEngine/Context.h"
#include "Types/ComplexPolygon.pb.h"
#include "NativeGeoPoint.h"


/// <summary>
/// Converts polygons to GPU representation.
/// </summary>
/// <param name="polygons">Polygons to convert.</param>
/// <returns>Tuple of array for the GPU.</returns>
GPUMemory::GPUPolygon
ComplexPolygonFactory::PrepareGPUPolygon(const std::vector<QikkDB::Types::ComplexPolygon>& polygons)
{
    // Points of polygons
    std::vector<NativeGeoPoint> polyPoints;

    // Start indexes of each polygon in point array
    std::vector<int32_t> pointIdx;

    // Start indexes of each complex polygon in polygon array
    std::vector<int32_t> polyIdx;

    for (const auto& complPoly : polygons)
    {
        const int subpolyCount = complPoly.polygons_size();
        for (int i = 0; i < subpolyCount; i++)
        {
            const auto& subpoly = complPoly.polygons(i);
            const int subpPointCount =
                subpoly.geopoints_size() - 1; // -1 because we need to ignore the last element from the WKT format

            for (int j = 0; j < subpPointCount; j++)
            {
                const auto& geopoint = subpoly.geopoints(j);
                polyPoints.push_back({geopoint.latitude(), geopoint.longitude()});
            }

            pointIdx.push_back(polyPoints.size());
        }
        polyIdx.push_back(pointIdx.size());
    }

    GPUMemory::GPUPolygon retPointers = {nullptr, nullptr, nullptr};

    try
    {
        GPUMemory::alloc(&retPointers.pointIdx, pointIdx.size());
        GPUMemory::copyHostToDevice(retPointers.pointIdx, pointIdx.data(), pointIdx.size());

        GPUMemory::alloc(&retPointers.polyIdx, polyIdx.size());
        GPUMemory::copyHostToDevice(retPointers.polyIdx, polyIdx.data(), polyIdx.size());

        GPUMemory::alloc(&retPointers.polyPoints, polyPoints.size());
        GPUMemory::copyHostToDevice(retPointers.polyPoints, polyPoints.data(), polyPoints.size());
    }
    catch (...)
    {
        if (retPointers.pointIdx)
        {
            GPUMemory::free(retPointers.pointIdx);
        }

        if (retPointers.polyIdx)
        {
            GPUMemory::free(retPointers.polyIdx);
        }

        if (retPointers.polyPoints)
        {
            GPUMemory::free(retPointers.polyPoints);
        }

        throw;
    }
    return retPointers;
}

GPUMemory::GPUPolygon
ComplexPolygonFactory::PrepareGPUPolygon(const std::vector<QikkDB::Types::ComplexPolygon>& polygons,
                                         const std::string& databaseName,
                                         const std::string& columnName,
                                         size_t blockIndex,
                                         int64_t loadSize,
                                         int64_t loadOffset)
{
    // Points of polygons
    std::vector<NativeGeoPoint> polyPoints;

    // Start indexes of each polygon in point array
    std::vector<int32_t> pointIdx;

    // Start indexes of each complex polygon in polygon array
    std::vector<int32_t> polyIdx;

    for (const auto& complPoly : polygons)
    {
        const int subpolyCount = complPoly.polygons_size();

        for (int i = 0; i < subpolyCount; i++)
        {
            const auto& subpoly = complPoly.polygons(i);
            const int subpPointCount =
                subpoly.geopoints_size() - 1; // -1 because we need to ignore the last element from the WKT format

            for (int j = 0; j < subpPointCount; j++)
            {
                const auto& geopoint = subpoly.geopoints(j);
                polyPoints.push_back({geopoint.latitude(), geopoint.longitude()});
            }

            pointIdx.push_back(polyPoints.size());
        }
        polyIdx.push_back(pointIdx.size());
    }
    GPUMemory::GPUPolygon retPointers;

    retPointers.pointIdx = std::get<0>(Context::getInstance().getCacheForCurrentDevice().GetColumn<int32_t>(
        databaseName, columnName + "_pointIdx", blockIndex, pointIdx.size(), loadSize, loadOffset));
    GPUMemory::copyHostToDevice(retPointers.pointIdx, pointIdx.data(), pointIdx.size());

    retPointers.polyIdx = std::get<0>(Context::getInstance().getCacheForCurrentDevice().GetColumn<int32_t>(
        databaseName, columnName + "_polyIdx", blockIndex, polyIdx.size(), loadSize, loadOffset));
    GPUMemory::copyHostToDevice(retPointers.polyIdx, polyIdx.data(), polyIdx.size());

    retPointers.polyPoints = std::get<0>(Context::getInstance().getCacheForCurrentDevice().GetColumn<NativeGeoPoint>(
        databaseName, columnName + "_polyPoints", blockIndex, polyPoints.size(), loadSize, loadOffset));
    GPUMemory::copyHostToDevice(retPointers.polyPoints, polyPoints.data(), polyPoints.size());

    return retPointers;
}

/// <summary>
/// Constructor for creating complex polygon and initializing.
/// </summary>
/// <param name="wktPolygon">String of well known text formatted polygon.</param>
/// <param name="spaceBetweenItems">Represents if there is a space between geo points after a comma and between
/// polygons also after a comma. Default value is set to 'false'.</param>
/// <exception cref="FormatException">Format exception with a message that explains the reason why the
/// exception have been thrown.</exception>
QikkDB::Types::ComplexPolygon ComplexPolygonFactory::FromWkt(std::string wkt)
{
    QikkDB::Types::ComplexPolygon ret;

    size_t replaceIdx = wkt.find(", ");
    while (replaceIdx != std::string::npos)
    {
        wkt.erase(replaceIdx + 1, 1);
        replaceIdx = wkt.find(", ");
    }

    std::vector<std::string> polygons;
    size_t startIdx = 0;
    size_t endOfPolyIdx = wkt.find("),(");
    while (endOfPolyIdx != std::string::npos)
    {
        polygons.push_back(wkt.substr(startIdx, endOfPolyIdx - startIdx));
        startIdx = endOfPolyIdx + 3;
        endOfPolyIdx = wkt.find("),(", startIdx);
    }
    polygons.push_back(wkt.substr(startIdx));

    if (polygons.size() > MAX_POLYGONS_NUMBER)
    {
        throw std::invalid_argument("Invalid WKT format - more polygons than allowed maximum");
    }

    std::regex wktRegex("((-?[0-9]+(\\.[0-9]+)?) (-?[0-9]+(\\.[0-9]+)?)(, ?)?)+");

    replaceIdx = polygons[0].find("POLYGON((");
    if (replaceIdx != std::string::npos)
    {
        polygons[0].erase(replaceIdx, 9);
    }

    replaceIdx = polygons[0].find("POLYGON ((");
    if (replaceIdx != std::string::npos)
    {
        polygons[0].erase(replaceIdx, 10);
    }

    replaceIdx = polygons[polygons.size() - 1].find("))");
    if (replaceIdx != std::string::npos)
    {
        polygons[polygons.size() - 1].erase(polygons[polygons.size() - 1].begin() + replaceIdx,
                                            polygons[polygons.size() - 1].end());
    }

    std::smatch wktMatch;
    for (const auto& polygon : polygons)
    {
        if (polygon.find(')') != std::string::npos || polygon.find('(') != std::string::npos)
        {
            throw std::invalid_argument("Invalid WKT format - mismatched brace");
        }

        if (!std::regex_match(polygon, wktMatch, wktRegex))
        {
            throw std::invalid_argument("Invalid WKT format");
        }

        auto newPolygon = ret.add_polygons();
        std::vector<std::string> points;

        startIdx = 0;
        endOfPolyIdx = polygon.find(',');
        while (endOfPolyIdx != std::string::npos)
        {
            points.push_back(polygon.substr(startIdx, endOfPolyIdx - startIdx));
            startIdx = endOfPolyIdx + 1;
            endOfPolyIdx = polygon.find(',', startIdx);
        }

        points.push_back(polygon.substr(startIdx));

        if (points.size() < 2)
        {
            throw std::invalid_argument("Invalid WKT format - too few input points");
        }

        for (const auto& point : points)
        {
            ptrdiff_t coordCount = std::count(point.cbegin(), point.cend(), ' ') + 1;

            if (coordCount != 2)
            {
                throw std::invalid_argument(
                    "Invalid WKT format - too many or not enough coordinates for a point");
            }

            auto newPoint = newPolygon->add_geopoints();
            float latitude, longitude;
            std::istringstream coordStream(point);
            coordStream >> latitude >> longitude;

            if (coordStream.fail())
            {
                throw std::invalid_argument("Invalid WKT format - coordinate parse error");
            }

            newPoint->set_latitude(latitude);
            newPoint->set_longitude(longitude);
        }

        if (std::abs(newPolygon->geopoints(0).latitude() -
                     newPolygon->geopoints(newPolygon->geopoints_size() - 1).latitude()) >= 0.0001f ||
            std::abs(newPolygon->geopoints(0).longitude() -
                     newPolygon->geopoints(newPolygon->geopoints_size() - 1).longitude()) >= 0.0001f)
        {
            throw std::invalid_argument("Invalid WKT format - first and last point do not match");
        }
    }
    return ret;
}

/// <summary>
/// Method that converts class to a string representation.
/// </summary>
/// <returns>ComplexPolygon in format of well known text.</returns>
std::string ComplexPolygonFactory::WktFromPolygon(const QikkDB::Types::ComplexPolygon& complexPolygon,
                                                  bool fixedPrecision)
{
    std::ostringstream wktStream;
    if (fixedPrecision)
    {
        wktStream << std::fixed;
        wktStream << std::setprecision(4);
    }
    wktStream << "POLYGON(";
    int polyCount = complexPolygon.polygons_size();
    for (int i = 0; i < polyCount; i++)
    {
        wktStream << "(";

        int geopointCount = complexPolygon.polygons(i).geopoints_size();
        const auto& polygon = complexPolygon.polygons(i);
        for (int j = 0; j < geopointCount; j++)
        {
            const auto& geopoint = polygon.geopoints(j);
            wktStream << geopoint.latitude() << " " << geopoint.longitude();
            if (j != geopointCount - 1)
            {
                wktStream << ", ";
            }
        }
        wktStream << ")";
        if (i != polyCount - 1)
        {
            wktStream << ", ";
        }
    }
    wktStream << ")";
    return wktStream.str();
}
