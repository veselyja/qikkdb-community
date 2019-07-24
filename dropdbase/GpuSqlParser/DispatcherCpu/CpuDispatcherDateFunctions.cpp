#include "CpuDispatcherDateFunctions.h"
#include <array>
#include "../../QueryEngine/GPUCore/DateOperations.h"

std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::yearFunctions = { &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, int32_t>, &CpuSqlDispatcher::dateExtractConst<DateOperations::year>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::year, int8_t>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, int32_t>, &CpuSqlDispatcher::dateExtractCol<DateOperations::year>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::year, int8_t> };
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::monthFunctions = { &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, int32_t>, &CpuSqlDispatcher::dateExtractConst<DateOperations::month>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::month, int8_t>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, int32_t>, &CpuSqlDispatcher::dateExtractCol<DateOperations::month>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::month, int8_t> };
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::dayFunctions = { &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, int32_t>, &CpuSqlDispatcher::dateExtractConst<DateOperations::day>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::day, int8_t>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, int32_t>, &CpuSqlDispatcher::dateExtractCol<DateOperations::day>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::day, int8_t> };
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::hourFunctions = { &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, int32_t>, &CpuSqlDispatcher::dateExtractConst<DateOperations::hour>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::hour, int8_t>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, int32_t>, &CpuSqlDispatcher::dateExtractCol<DateOperations::hour>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::hour, int8_t> };
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::minuteFunctions = { &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, int32_t>, &CpuSqlDispatcher::dateExtractConst<DateOperations::minute>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::minute, int8_t>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, int32_t>, &CpuSqlDispatcher::dateExtractCol<DateOperations::minute>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::minute, int8_t> };
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::secondFunctions = { &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, int32_t>, &CpuSqlDispatcher::dateExtractConst<DateOperations::second>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<DateOperations::second, int8_t>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, int32_t>, &CpuSqlDispatcher::dateExtractCol<DateOperations::second>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, float>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, double>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, ColmnarDB::Types::Point>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, ColmnarDB::Types::ComplexPolygon>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, std::string>, &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<DateOperations::second, int8_t> };
