#include "CpuDispatcherDateFunctions.h"
#include <array>
#include "../../QueryEngine/GPUCore/DateOperations.h"

std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::yearFunctions_ = {
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, int32_t>,
    &CpuSqlDispatcher::DateExtractConst<DateOperations::year>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::year, int8_t>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, int32_t>,
    &CpuSqlDispatcher::DateExtractCol<DateOperations::year>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::year, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::monthFunctions_ = {
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, int32_t>,
    &CpuSqlDispatcher::DateExtractConst<DateOperations::month>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::month, int8_t>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, int32_t>,
    &CpuSqlDispatcher::DateExtractCol<DateOperations::month>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::month, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::dayFunctions_ = {
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, int32_t>,
    &CpuSqlDispatcher::DateExtractConst<DateOperations::day>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::day, int8_t>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, int32_t>,
    &CpuSqlDispatcher::DateExtractCol<DateOperations::day>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::day, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::hourFunctions_ = {
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, int32_t>,
    &CpuSqlDispatcher::DateExtractConst<DateOperations::hour>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::hour, int8_t>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, int32_t>,
    &CpuSqlDispatcher::DateExtractCol<DateOperations::hour>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::hour, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::minuteFunctions_ = {
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, int32_t>,
    &CpuSqlDispatcher::DateExtractConst<DateOperations::minute>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::minute, int8_t>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, int32_t>,
    &CpuSqlDispatcher::DateExtractCol<DateOperations::minute>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::minute, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::secondFunctions_ = {
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, int32_t>,
    &CpuSqlDispatcher::DateExtractConst<DateOperations::second>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerConst<DateOperations::second, int8_t>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, int32_t>,
    &CpuSqlDispatcher::DateExtractCol<DateOperations::second>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, float>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, double>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, std::string>,
    &CpuSqlDispatcher::InvalidOperandTypesErrorHandlerCol<DateOperations::second, int8_t>};
