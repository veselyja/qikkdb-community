#include "CpuDispatcherArithmeticUnaryFunctions.h"
#include <array>
#include "../../QueryEngine/GPUCore/ArithmeticUnaryOperations.h"

std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::minusFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::minus, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::minus, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::minus, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::minus, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::minus, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::minus, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::minus, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::minus, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::minus, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::minus, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::minus, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::minus, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::minus, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::minus, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::minus, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::minus, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::absoluteFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::absolute, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::absolute, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::absolute, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::absolute, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::absolute, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::absolute, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::absolute, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::absolute, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::absolute, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::absolute, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::absolute, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::absolute, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::absolute, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::absolute, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::absolute, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::absolute, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::sineFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sine, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sine, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sine, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sine, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::cosineFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cosine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cosine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cosine, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cosine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cosine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cosine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cosine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cosine, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cosine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cosine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cosine, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cosine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cosine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cosine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cosine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cosine, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::tangentFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::tangent, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::tangent, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::tangent, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::tangent, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::tangent, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::tangent, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::tangent, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::tangent, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::tangent, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::tangent, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::tangent, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::tangent, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::tangent, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::tangent, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::tangent, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::tangent, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::cotangentFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cotangent, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cotangent, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cotangent, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::cotangent, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cotangent, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cotangent, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cotangent, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::cotangent, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cotangent, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cotangent, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cotangent, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::cotangent, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cotangent, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cotangent, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cotangent, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::cotangent, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::arcsineFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arcsine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arcsine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arcsine, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arcsine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arcsine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arcsine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arcsine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arcsine, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arcsine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arcsine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arcsine, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arcsine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arcsine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arcsine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arcsine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arcsine, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::arccosineFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arccosine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arccosine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arccosine, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arccosine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arccosine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arccosine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arccosine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arccosine, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arccosine, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arccosine, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arccosine, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arccosine, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arccosine, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arccosine, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arccosine, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arccosine, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::arctangentFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arctangent, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arctangent, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arctangent, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::arctangent, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arctangent, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arctangent, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arctangent, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::arctangent, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arctangent, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arctangent, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arctangent, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::arctangent, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arctangent, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arctangent, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arctangent, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::arctangent, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::logarithm10Functions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithm10, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithm10, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithm10, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithm10, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithm10, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithm10, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithm10, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithm10, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithm10, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithm10, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithm10, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithm10, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithm10, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithm10, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithm10, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithm10, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::logarithmNaturalFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithmNatural, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithmNatural, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithmNatural, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::logarithmNatural, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithmNatural, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithmNatural, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithmNatural, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::logarithmNatural, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithmNatural, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithmNatural, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithmNatural, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::logarithmNatural, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithmNatural, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithmNatural, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithmNatural, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::logarithmNatural, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::exponentialFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::exponential, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::exponential, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::exponential, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::exponential, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::exponential, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::exponential, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::exponential, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::exponential, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::exponential, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::exponential, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::exponential, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::exponential, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::exponential, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::exponential, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::exponential, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::exponential, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::squareRootFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::squareRoot, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::squareRoot, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::squareRoot, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::squareRoot, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::squareRoot, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::squareRoot, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::squareRoot, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::squareRoot, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::squareRoot, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::squareRoot, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::squareRoot, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::squareRoot, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::squareRoot, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::squareRoot, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::squareRoot, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::squareRoot, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::squareFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::square, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::square, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::square, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::square, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::square, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::square, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::square, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::square, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::square, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::square, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::square, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::square, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::square, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::square, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::square, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::square, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::signFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sign, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sign, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sign, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::sign, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sign, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sign, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sign, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::sign, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sign, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sign, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sign, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::sign, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sign, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sign, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sign, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::sign, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::roundFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::round, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::round, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::round, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::round, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::round, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::round, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::round, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::round, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::round, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::round, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::round, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::round, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::round, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::round, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::round, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::round, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::floorFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::floor, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::floor, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::floor, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::floor, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::floor, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::floor, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::floor, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::floor, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::floor, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::floor, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::floor, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::floor, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::floor, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::floor, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::floor, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::floor, int8_t>};
std::array<CpuSqlDispatcher::CpuDispatchFunction, DataType::DATA_TYPE_SIZE> CpuSqlDispatcher::ceilFunctions = {
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::ceil, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::ceil, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::ceil, float>,
    &CpuSqlDispatcher::arithmeticUnaryConst<ArithmeticUnaryOperations::ceil, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::ceil, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::ceil, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::ceil, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerConst<ArithmeticUnaryOperations::ceil, int8_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::ceil, int32_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::ceil, int64_t>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::ceil, float>,
    &CpuSqlDispatcher::arithmeticUnaryCol<ArithmeticUnaryOperations::ceil, double>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::ceil, ColmnarDB::Types::Point>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::ceil, ColmnarDB::Types::ComplexPolygon>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::ceil, std::string>,
    &CpuSqlDispatcher::invalidOperandTypesErrorHandlerCol<ArithmeticUnaryOperations::ceil, int8_t>};
