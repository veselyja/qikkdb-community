#pragma once
#include <cstdint>

enum DataType {
	CONST_ERROR = -1,
	CONST_INT = 0,
	CONST_LONG = 1,
	CONST_FLOAT = 2,
	CONST_DOUBLE = 3,
	CONST_POINT = 4,
	CONST_POLYGON = 5,
	CONST_STRING = 6,
	CONST_INT8_T = 7,
	COLUMN_INT = 8,
	COLUMN_LONG = 9,
	COLUMN_FLOAT = 10,
	COLUMN_DOUBLE = 11,
	COLUMN_POINT = 12,
	COLUMN_POLYGON = 13,
	COLUMN_STRING = 14,
	COLUMN_INT8_T = 15,
	DATA_TYPE_SIZE = 16
};

constexpr int32_t GetDataTypeSize(DataType type)
{
	switch(type)
	{
		case COLUMN_INT:
		case CONST_INT:
			return sizeof(int32_t);
		case COLUMN_LONG:
		case CONST_LONG:
			return sizeof(int64_t);
		case COLUMN_DOUBLE:
		case CONST_DOUBLE:
			return sizeof(double);
		case COLUMN_FLOAT:
		case CONST_FLOAT:
			return sizeof(float);
		default:
			return sizeof(int8_t);
	}
}