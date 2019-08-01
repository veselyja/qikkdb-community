#include "CpuWhereListener.h"
#include <boost/algorithm/string.hpp>
#include "../ColumnBase.h"

constexpr float pi() { return 3.1415926f; }

CpuWhereListener::CpuWhereListener(const std::shared_ptr<Database>& database, CpuSqlDispatcher& dispatcher) :
	database(database),
	dispatcher(dispatcher)
{
}

void CpuWhereListener::exitBinaryOperation(GpuSqlParser::BinaryOperationContext * ctx)
{
	std::pair<std::string, DataType> right = stackTopAndPop();
	std::pair<std::string, DataType> left = stackTopAndPop();

	std::string op = ctx->op->getText();
	stringToUpper(op);

	DataType rightOperandType = std::get<1>(right);
	DataType leftOperandType = std::get<1>(left);

	std::string rightOperand = std::get<0>(right);
	std::string leftOperand = std::get<0>(left);

	pushArgument(leftOperand.c_str(), leftOperandType);
	pushArgument(rightOperand.c_str(), rightOperandType);

	std::string reg;
	trimReg(rightOperand);
	trimReg(leftOperand);

	DataType returnDataType;

	if (op == ">")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "<")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == ">=")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "<=")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "=")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "!=" || op == "<>")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "AND")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "OR")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "*")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "/")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "+")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "-")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "%")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "|")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "&")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "^")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "<<")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == ">>")
	{
		reg = "$" + leftOperand + op + rightOperand;
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "POINT")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_POINT;
	}
	else if (op == "GEO_CONTAINS")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "GEO_INTERSECT")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_POLYGON;
	}
	else if (op == "GEO_UNION")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_POLYGON;
	}
	else if (op == "LOG")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "POW")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "ROOT")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = getReturnDataType(leftOperandType, rightOperandType);
	}
	else if (op == "ATAN2")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = getReturnDataType(DataType::COLUMN_FLOAT);
	}
	else if (op == "LEFT")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	else if (op == "RIGHT")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	else if (op == "CONCAT")
	{
		reg = "$" + op + "(" + leftOperand + "," + rightOperand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	dispatcher.addBinaryOperation(leftOperandType, rightOperandType, op);

	pushArgument(reg.c_str(), returnDataType);
	pushTempResult(reg, returnDataType);
}

void CpuWhereListener::exitTernaryOperation(GpuSqlParser::TernaryOperationContext * ctx)
{
}

void CpuWhereListener::exitUnaryOperation(GpuSqlParser::UnaryOperationContext * ctx)
{
	std::pair<std::string, DataType> arg = stackTopAndPop();

	std::string op = ctx->op->getText();
	stringToUpper(op);

	std::string operand = std::get<0>(arg);
	DataType operandType = std::get<1>(arg);

	pushArgument(operand.c_str(), operandType);

	DataType returnDataType;

	std::string reg;
	trimReg(operand);

	if (op == "!")
	{
		reg = "$" + op + operand;
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "IS NULL")
	{
		reg = "$" + op + operand;
		if (operandType < DataType::COLUMN_INT)
		{
			throw NullMaskOperationInvalidOperandException();
		}
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "IS NOT NULL")
	{
		reg = "$" + op + operand;
		if (operandType < DataType::COLUMN_INT)
		{
			throw NullMaskOperationInvalidOperandException();
		}
		returnDataType = DataType::COLUMN_INT8_T;
	}
	else if (op == "-")
	{
		reg = "$" + op + operand;
		returnDataType = getReturnDataType(operandType);
	}
	else if (op == "YEAR")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = COLUMN_INT;
	}
	else if (op == "MONTH")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = COLUMN_INT;
	}
	else if (op == "DAY")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = COLUMN_INT;
	}
	else if (op == "HOUR")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = COLUMN_INT;
	}
	else if (op == "MINUTE")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = COLUMN_INT;
	}
	else if (op == "SECOND")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = COLUMN_INT;
	}
	else if (op == "ABS")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = getReturnDataType(operandType);
	}
	else if (op == "SIN")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "COS")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "TAN")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "COT")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "ASIN")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "ACOS")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "ATAN")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "LOG10")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "LOG")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "EXP")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "SQRT")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "SQUARE")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "SIGN")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_INT;
	}
	else if (op == "ROUND")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "FLOOR")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "CEIL")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_FLOAT;
	}
	else if (op == "LTRIM")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	else if (op == "RTRIM")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	else if (op == "LOWER")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	else if (op == "UPPER")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_STRING;
	}
	else if (op == "LEN")
	{
		reg = "$" + op + "(" + operand + ")";
		returnDataType = DataType::COLUMN_INT;
	}
	dispatcher.addUnaryOperation(operandType, op);

	pushArgument(reg.c_str(), returnDataType);
	pushTempResult(reg, returnDataType);
}

void CpuWhereListener::exitCastOperation(GpuSqlParser::CastOperationContext * ctx)
{
	std::pair<std::string, DataType> arg = stackTopAndPop();

	std::string operand = std::get<0>(arg);
	DataType operandType = std::get<1>(arg);

	pushArgument(operand.c_str(), operandType);
	std::string castTypeStr = ctx->DATATYPE()->getText();
	stringToUpper(castTypeStr);
	DataType castType = getDataTypeFromString(castTypeStr);

	dispatcher.addCastOperation(operandType, castType, castTypeStr);

	trimReg(operand);
	std::string reg = "$CAST(" + operand + "AS" + castTypeStr + ")";

	pushArgument(reg.c_str(), castType);
	pushTempResult(reg, castType);
}

void CpuWhereListener::exitIntLiteral(GpuSqlParser::IntLiteralContext * ctx)
{
	std::string token = ctx->getText();
	if (isLong(token))
	{
		parserStack.push(std::make_pair(token, DataType::CONST_LONG));
	}
	else
	{
		parserStack.push(std::make_pair(token, DataType::CONST_INT));
	}
}

void CpuWhereListener::exitDecimalLiteral(GpuSqlParser::DecimalLiteralContext * ctx)
{
	std::string token = ctx->getText();
	if (isDouble(token))
	{
		parserStack.push(std::make_pair(token, DataType::CONST_DOUBLE));
	}
	else
	{
		parserStack.push(std::make_pair(token, DataType::CONST_FLOAT));
	}
}

void CpuWhereListener::exitStringLiteral(GpuSqlParser::StringLiteralContext * ctx)
{
	parserStack.push(std::make_pair(ctx->getText(), DataType::CONST_STRING));
}

void CpuWhereListener::exitBooleanLiteral(GpuSqlParser::BooleanLiteralContext * ctx)
{
	parserStack.push(std::make_pair(ctx->getText(), DataType::CONST_INT8_T));
}

void CpuWhereListener::exitGeoReference(GpuSqlParser::GeoReferenceContext * ctx)
{
	auto start = ctx->start->getStartIndex();
	auto stop = ctx->stop->getStopIndex();
	antlr4::misc::Interval interval(start, stop);
	std::string geoValue = ctx->geometry()->start->getInputStream()->getText(interval);

	if (isPolygon(geoValue))
	{
		parserStack.push(std::make_pair(geoValue, DataType::CONST_POLYGON));
	}
	else if (isPoint(geoValue))
	{
		parserStack.push(std::make_pair(geoValue, DataType::CONST_POINT));
	}
}

void CpuWhereListener::exitVarReference(GpuSqlParser::VarReferenceContext * ctx)
{
	std::string colName = ctx->columnId()->getText();

	if (columnAliasContexts.find(colName) != columnAliasContexts.end())
	{
		walkAliasExpression(colName);
		return;
	}

	std::pair<std::string, DataType> tableColumnData = generateAndValidateColumnName(ctx->columnId());
	const DataType columnType = std::get<1>(tableColumnData);
	const std::string tableColumn = std::get<0>(tableColumnData);

	parserStack.push(std::make_pair(tableColumn, columnType));
}

void CpuWhereListener::exitDateTimeLiteral(GpuSqlParser::DateTimeLiteralContext * ctx)
{
	auto start = ctx->start->getStartIndex();
	auto stop = ctx->stop->getStopIndex();
	antlr4::misc::Interval interval(start, stop);
	std::string dateValue = ctx->start->getInputStream()->getText(interval);
	dateValue = dateValue.substr(1, dateValue.size() - 2);

	if (dateValue.size() <= 10)
	{
		dateValue += " 00:00:00";
	}

	std::tm t;
	std::istringstream ss(dateValue);
	ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
	std::time_t epochTime = std::mktime(&t);

	parserStack.push(std::make_pair(std::to_string(epochTime), DataType::CONST_LONG));
}

void CpuWhereListener::exitPiLiteral(GpuSqlParser::PiLiteralContext * ctx)
{
	parserStack.push(std::make_pair(std::to_string(pi()), DataType::CONST_FLOAT));
}

void CpuWhereListener::exitNowLiteral(GpuSqlParser::NowLiteralContext * ctx)
{
	std::time_t epochTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	parserStack.push(std::make_pair(std::to_string(epochTime), DataType::CONST_LONG));
}

void CpuWhereListener::exitWhereClause(GpuSqlParser::WhereClauseContext * ctx)
{
	std::pair<std::string, DataType> arg = stackTopAndPop();
	dispatcher.addArgument<const std::string&>(std::get<0>(arg));
	dispatcher.addWhereResultFunction(std::get<1>(arg));
}

void CpuWhereListener::exitFromTables(GpuSqlParser::FromTablesContext *ctx)
{
	for (auto fromTable : ctx->fromTable())
	{
		std::string table = fromTable->table()->getText();
		trimDelimitedIdentifier(table);
		if (database->GetTables().find(table) == database->GetTables().end())
		{
			throw TableNotFoundFromException();
		}
		loadedTables.insert(table);

		if (fromTable->alias())
		{
			std::string alias = fromTable->alias()->getText();
			trimDelimitedIdentifier(alias);
			if (tableAliases.find(alias) != tableAliases.end())
			{
				throw AliasRedefinitionException();
			}
			tableAliases.insert({ alias, table });
		}
	}
}

void CpuWhereListener::ExtractColumnAliasContexts(GpuSqlParser::SelectColumnsContext * ctx)
{
	for (auto& selectColumn : ctx->selectColumn())
	{
		if (selectColumn->alias())
		{
			std::string alias = selectColumn->alias()->getText();
			if (columnAliasContexts.find(alias) != columnAliasContexts.end())
			{
				throw AliasRedefinitionException();
			}
			columnAliasContexts.insert({ alias, selectColumn->expression() });
		}
	}
}

void CpuWhereListener::pushArgument(const char * token, DataType dataType)
{
	switch (dataType)
	{
	case DataType::CONST_INT:
		dispatcher.addArgument<int32_t>(std::stoi(token));
		break;
	case DataType::CONST_LONG:
		dispatcher.addArgument<int64_t>(std::stoll(token));
		break;
	case DataType::CONST_FLOAT:
		dispatcher.addArgument<float>(std::stof(token));
		break;
	case DataType::CONST_DOUBLE:
		dispatcher.addArgument<double>(std::stod(token));
		break;
	case DataType::CONST_POINT:
	case DataType::CONST_POLYGON:
	case DataType::CONST_STRING:
	case DataType::COLUMN_INT:
	case DataType::COLUMN_LONG:
	case DataType::COLUMN_FLOAT:
	case DataType::COLUMN_DOUBLE:
	case DataType::COLUMN_POINT:
	case DataType::COLUMN_POLYGON:
	case DataType::COLUMN_STRING:
	case DataType::COLUMN_INT8_T:
		dispatcher.addArgument<const std::string&>(token);
		break;
	case DataType::DATA_TYPE_SIZE:
	case DataType::CONST_ERROR:
		break;
	}
}

std::pair<std::string, DataType> CpuWhereListener::stackTopAndPop()
{
	std::pair<std::string, DataType> value = parserStack.top();
	parserStack.pop();
	return value;
}

void CpuWhereListener::stringToUpper(std::string & str)
{
	for (auto &c : str)
	{
		c = toupper(c);
	}
}

void CpuWhereListener::pushTempResult(std::string reg, DataType type)
{
	parserStack.push(std::make_pair(reg, type));
}

bool CpuWhereListener::isLong(const std::string & value)
{
	try
	{
		std::stoi(value);
	}
	catch (std::out_of_range &e)
	{
		std::stoll(value);
		return true;
	}
	return false;
}

bool CpuWhereListener::isDouble(const std::string & value)
{
	try
    {
        std::stof(value);
    }
    catch (std::out_of_range &e)
    {
        std::stod(value);
        return true;
    }
    return false;
}

bool CpuWhereListener::isPoint(const std::string & value)
{
	return (value.find("POINT") == 0);
}

bool CpuWhereListener::isPolygon(const std::string & value)
{
	return (value.find("POLYGON") == 0);
}

void CpuWhereListener::trimDelimitedIdentifier(std::string& str)
{
	if (str.front() == '[' && str.back() == ']' && str.size() > 2)
	{
		str.erase(0, 1);
		str.erase(str.size() - 1);
	}
}

DataType CpuWhereListener::getReturnDataType(DataType left, DataType right)
{
	if (right < DataType::COLUMN_INT)
	{
		right = static_cast<DataType>(right + DataType::COLUMN_INT);
	}
	if (left < DataType::COLUMN_INT)
	{
		left = static_cast<DataType>(left + DataType::COLUMN_INT);
	}
	DataType result = std::max<DataType>(left, right);

	return result;
}

DataType CpuWhereListener::getReturnDataType(DataType operand)
{
	if (operand < DataType::COLUMN_INT)
	{
		return static_cast<DataType>(operand + DataType::COLUMN_INT);
	}
	return operand;
}

DataType CpuWhereListener::getDataTypeFromString(const std::string & dataType)
{
	return ::GetColumnDataTypeFromString(dataType);
}

std::pair<std::string, DataType> CpuWhereListener::generateAndValidateColumnName(GpuSqlParser::ColumnIdContext * ctx)
{
	std::string table;
	std::string column;
	
	std::string col = ctx->column()->getText();
	
	if (ctx->table())
	{
		table = ctx->table()->getText();
		trimDelimitedIdentifier(table);
		column = ctx->column()->getText();
		trimDelimitedIdentifier(column);
	
		if (tableAliases.find(table) != tableAliases.end())
		{
			table = tableAliases.at(table);
		}
	
		if (loadedTables.find(table) == loadedTables.end())
		{
			throw TableNotFoundFromException();
		}
		if (database->GetTables().at(table).GetColumns().find(column) == database->GetTables().at(table).GetColumns().end())
		{
			throw ColumnNotFoundException();
		}

		shortColumnNames.insert({ table + "." + column, table + "." + column });
	}
	else
	{
		int uses = 0;
		for (auto &tab : loadedTables)
		{
			if (database->GetTables().at(tab).GetColumns().find(col) != database->GetTables().at(tab).GetColumns().end())
			{
				table = tab;
				column = col;
				uses++;
			}
			if (uses > 1)
			{
				throw ColumnAmbiguityException();
			}
		}
		if (column.empty())
		{
			throw ColumnNotFoundException();
		}

		shortColumnNames.insert({ table + "." + column, column });
	}
	
	std::string tableColumn = table + "." + column;
	DataType columnType = database->GetTables().at(table).GetColumns().at(column)->GetColumnType();
	
	std::pair<std::string, DataType> tableColumnPair = std::make_pair(tableColumn, columnType);
	
	return tableColumnPair;
}

void CpuWhereListener::walkAliasExpression(const std::string & alias)
{
	antlr4::tree::ParseTreeWalker walker;
	walker.walk(this, columnAliasContexts.at(alias));
}

void CpuWhereListener::trimReg(std::string& reg)
{
	if (reg.front() == '$')
	{
		reg.erase(reg.begin());
	}
	else if (shortColumnNames.find(reg) != shortColumnNames.end())
	{
		reg = shortColumnNames.at(reg);
	}
}