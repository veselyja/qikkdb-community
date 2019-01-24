//
// Created by Martin Staňo on 2019-01-15.
//

#include "GpuSqlListener.h"
#include "../Table.h"

GpuSqlListener::GpuSqlListener(const std::shared_ptr<Database> &database,
                               GpuSqlDispatcher &dispatcher) : database(database), dispatcher(dispatcher),
								usingGroupBy(false), insideAgg(false), tempCounter(0)
{
}


void GpuSqlListener::exitBinaryOperation(GpuSqlParser::BinaryOperationContext *ctx)
{
    std::tuple<std::string, DataType> right = stackTopAndPop();
    std::tuple<std::string, DataType> left = stackTopAndPop();

    std::string op = ctx->op->getText();
    stringToUpper(op);

    DataType rightOperandType = std::get<1>(right);
    DataType leftOperandType = std::get<1>(left);

    pushArgument(std::get<0>(right).c_str(), rightOperandType);
    pushArgument(std::get<0>(left).c_str(), leftOperandType);

    if (op == ">")
    {
        dispatcher.addGreaterFunction(leftOperandType, rightOperandType);
    } else if (op == "<")
    {
        dispatcher.addLessFunction(leftOperandType, rightOperandType);
    } else if (op == ">=")
    {
        dispatcher.addGreaterEqualFunction(leftOperandType, rightOperandType);
    } else if (op == "<=")
    {
        dispatcher.addLessEqualFunction(leftOperandType, rightOperandType);
    } else if (op == "=")
    {
        dispatcher.addEqualFunction(leftOperandType, rightOperandType);
    } else if (op == "!=")
    {
        dispatcher.addNotEqualFunction(leftOperandType, rightOperandType);
    } else if (op == "AND")
    {
        dispatcher.addLogicalAndFunction(leftOperandType, rightOperandType);
    } else if (op == "OR")
    {
        dispatcher.addLogicalOrFunction(leftOperandType, rightOperandType);
    } else if (op == "*")
    {
        dispatcher.addMulFunction(leftOperandType, rightOperandType);
    } else if (op == "/")
    {
        dispatcher.addDivFunction(leftOperandType, rightOperandType);
    } else if (op == "+")
    {
        dispatcher.addAddFunction(leftOperandType, rightOperandType);
    } else if (op == "-")
    {
        dispatcher.addSubFunction(leftOperandType, rightOperandType);
    } else if (op == "%")
    {
        dispatcher.addModFunction(leftOperandType, rightOperandType);
    } else if (op == "CONTAINS")
    {
        dispatcher.addContainsFunction(leftOperandType, rightOperandType);
    }
    pushTempResult();
}


void GpuSqlListener::exitTernaryOperation(GpuSqlParser::TernaryOperationContext *ctx)
{
    std::tuple<std::string, DataType> op1 = stackTopAndPop();
    std::tuple<std::string, DataType> op2 = stackTopAndPop();
    std::tuple<std::string, DataType> op3 = stackTopAndPop();

    std::string op = ctx->op->getText();
    stringToUpper(op);

    DataType op1Type = std::get<1>(op1);
    DataType op2Type = std::get<1>(op2);
    DataType op3Type = std::get<1>(op3);

    pushArgument(std::get<0>(op1).c_str(), op1Type);
    pushArgument(std::get<0>(op2).c_str(), op2Type);
    pushArgument(std::get<0>(op3).c_str(), op3Type);


    if (op == "BETWEEN")
    {
        dispatcher.addBetweenFunction(op1Type, op2Type, op3Type);
    }
    pushTempResult();
}

void GpuSqlListener::exitUnaryOperation(GpuSqlParser::UnaryOperationContext *ctx)
{
    std::tuple<std::string, DataType> arg = stackTopAndPop();

    std::string op = ctx->op->getText();
    stringToUpper(op);

    DataType operandType = std::get<1>(arg);

    pushArgument(std::get<0>(arg).c_str(), operandType);

    if (op == "!")
    {
        dispatcher.addLogicalNotFunction(operandType);
    } else if (op == "-")
    {
        dispatcher.addMinusFunction(operandType);
    }
    pushTempResult();
}

void GpuSqlListener::exitAggregation(GpuSqlParser::AggregationContext *ctx)
{
    std::tuple<std::string, DataType> arg = stackTopAndPop();

    std::string op = ctx->AGG()->getText();
    stringToUpper(op);

    DataType operandType = std::get<1>(arg);

    pushArgument(std::get<0>(arg).c_str(), operandType);

    if (op == "MIN")
    {
        dispatcher.addMinFunction(operandType);
    } else if (op == "MAX")
    {
        dispatcher.addMaxFunction(operandType);
    } else if (op == "SUM")
    {
        dispatcher.addSumFunction(operandType);
    } else if (op == "COUNT")
    {
        dispatcher.addCountFunction(operandType);
    } else if (op == "AVG")
    {
        dispatcher.addAvgFunction(operandType);
    }
    pushTempResult();
}

void GpuSqlListener::exitSelectColumns(GpuSqlParser::SelectColumnsContext *ctx)
{
    dispatcher.addDoneFunction();
}

void GpuSqlListener::exitSelectColumn(GpuSqlParser::SelectColumnContext *ctx)
{
    std::tuple<std::string, DataType> arg = stackTopAndPop();
    DataType retType = std::get<1>(arg);
    dispatcher.addRetFunction(retType);
    dispatcher.addArgument<const std::string&>(std::get<0>(arg));
}

void GpuSqlListener::exitFromTables(GpuSqlParser::FromTablesContext *ctx)
{

    for (auto table : ctx->table())
    {
        if (database->GetTables().find(table->getText()) == database->GetTables().end())
        {
            throw TableNotFoundFromException();
        }
        loadedTables.insert(table->getText());
    }

}

void GpuSqlListener::exitWhereClause(GpuSqlParser::WhereClauseContext *ctx)
{
    std::tuple<std::string, DataType> arg = stackTopAndPop();
    dispatcher.addArgument<const std::string&>(std::get<0>(arg));
    dispatcher.addFilFunction();
}

void GpuSqlListener::exitGroupByColumns(GpuSqlParser::GroupByColumnsContext *ctx)
{

    for (auto column : ctx->columnId())
    {
		std::tuple<std::string, DataType> tableColumnData = generateAndValidateColumnName(column);
		const DataType columnType = std::get<1>(tableColumnData);
		const std::string tableColumn = std::get<0>(tableColumnData);

        if (loadedColumns.find(tableColumn) == loadedColumns.end())
        {
            dispatcher.addLoadFunction(columnType);
            dispatcher.addArgument<const std::string&>(tableColumn);
            loadedColumns.insert(tableColumn);
        }
        if (groupByColumns.find(tableColumn) == groupByColumns.end())
        {
            dispatcher.addGroupByFunction(columnType);
            dispatcher.addArgument<const std::string&>(tableColumn);
            groupByColumns.insert(tableColumn);
        }
    }
    usingGroupBy = true;
}

void GpuSqlListener::exitIntLiteral(GpuSqlParser::IntLiteralContext *ctx)
{
    std::string token = ctx->getText();
    if (isLong(token))
    {
        parserStack.push(std::make_tuple(token, DataType::LONG));
    } else
    {
        parserStack.push(std::make_tuple(token, DataType::INT));
    }
}

void GpuSqlListener::exitDecimalLiteral(GpuSqlParser::DecimalLiteralContext *ctx)
{
    std::string token = ctx->getText();
    if (isDouble(token))
    {
        parserStack.push(std::make_tuple(token, DataType::DOUBLE));
    } else
    {
        parserStack.push(std::make_tuple(token, DataType::FLOAT));
    }
}

void GpuSqlListener::exitStringLiteral(GpuSqlParser::StringLiteralContext *ctx)
{
    parserStack.push(std::make_tuple(ctx->getText(), DataType::STRING));
}


void GpuSqlListener::exitBooleanLiteral(GpuSqlParser::BooleanLiteralContext *ctx)
{
    parserStack.push(std::make_tuple(ctx->getText(), DataType::BOOLEAN));
}

void GpuSqlListener::exitVarReference(GpuSqlParser::VarReferenceContext *ctx)
{
    std::tuple<std::string, DataType> tableColumnData = generateAndValidateColumnName(ctx->columnId());
    const DataType columnType = std::get<1>(tableColumnData);
	const std::string tableColumn = std::get<0>(tableColumnData);

    if (loadedColumns.find(tableColumn) == loadedColumns.end())
    {
        dispatcher.addLoadFunction(columnType);
        dispatcher.addArgument<const std::string&>(tableColumn);
        loadedColumns.insert(tableColumn);
    }
    parserStack.push(std::make_tuple(tableColumn, columnType));
}

void GpuSqlListener::exitGeoReference(GpuSqlParser::GeoReferenceContext *ctx)
{

    auto start = ctx->start->getStartIndex();
    auto stop = ctx->stop->getStopIndex();
    antlr4::misc::Interval interval(start, stop);
    std::string geoValue = ctx->geometry()->start->getInputStream()->getText(interval);

    if (isPolygon(geoValue))
    {
        parserStack.push(std::make_tuple(geoValue, DataType::POLYGON));
    } else if (isPoint(geoValue))
    {
        parserStack.push(std::make_tuple(geoValue, DataType::POINT));
    }
}


std::tuple<std::string, DataType> GpuSqlListener::generateAndValidateColumnName(GpuSqlParser::ColumnIdContext *ctx)
{
    std::string table;
    std::string column;

    std::string col = ctx->column()->getText();

    if (ctx->table())
    {
        table = ctx->table()->getText();
        column = ctx->column()->getText();

        if (loadedTables.find(table) == loadedTables.end())
        {
            throw TableNotFoundFromException();
        }
        if (database->GetTables().at(table).GetColumns().find(column) == database->GetTables().at(table).GetColumns().end())
        {
            throw ColumnNotFoundException();
        }
    } else
    {
        int uses = 0;
        column = ctx->column()->getText();
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
    }

    std::string tableColumn = table + "." + column;
	DataType columnType = database->GetTables().at(table).GetColumns().at(column)->GetColumnType();

    if (usingGroupBy && !insideAgg && groupByColumns.find(tableColumn) == groupByColumns.end())
    {
        throw ColumnGroupByException();
    }

    return std::make_tuple(tableColumn, columnType);
}

std::tuple<std::string, DataType> GpuSqlListener::stackTopAndPop()
{
    std::tuple<std::string, DataType> value = parserStack.top();
    parserStack.pop();
    return value;
}

void GpuSqlListener::pushTempResult()
{
    std::string reg = std::string("R") + std::to_string(tempCounter);
    tempCounter++;
    parserStack.push(std::make_tuple(reg, REG));
}

void GpuSqlListener::pushArgument(const char *token, DataType dataType)
{
    switch (dataType)
    {
        case DataType::INT:
            dispatcher.addArgument<int32_t>(std::stoi(token));
            break;
        case DataType::LONG:
            dispatcher.addArgument<int64_t>(std::stol(token));
            break;
        case DataType::FLOAT:
            dispatcher.addArgument<float>(std::stof(token));
            break;
        case DataType::DOUBLE:
            dispatcher.addArgument<double>(std::stod(token));
            break;
        case DataType::POINT:
        case DataType::POLYGON:
        case DataType::STRING:
        case DataType::BOOLEAN:
        case DataType::COLUMN_INT:
        case DataType::COLUMN_LONG:
        case DataType::COLUMN_FLOAT:
        case DataType::COLUMN_DOUBLE:
        case DataType::COLUMN_POINT:
        case DataType::COLUMN_POLYGON:
        case DataType::COLUMN_STRING:
        case DataType::COLUMN_BOOL:
        case DataType::REG:
            dispatcher.addArgument<const std::string&>(token);
            break;
        case DataType::DATA_TYPE_SIZE:
        case DataType::ERROR:
            break;
    }

}

bool GpuSqlListener::isLong(const std::string &value)
{
    try
    {
        std::stoi(value);
    }
    catch (std::out_of_range &e)
    {
        std::stol(value);
        return true;
    }
    return false;
}

bool GpuSqlListener::isDouble(const std::string &value)
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

bool GpuSqlListener::isPoint(const std::string &value)
{
    try
    {
        ColmnarDB::Types::Point point = PointFactory::FromWkt(value);
        return true;
    }
    catch (std::invalid_argument &e)
    {
        return false;
    }
}

bool GpuSqlListener::isPolygon(const std::string &value)
{
    try
    {
        ColmnarDB::Types::ComplexPolygon polygon = ComplexPolygonFactory::FromWkt(value);
        return true;
    }
    catch (std::invalid_argument &e)
    {
        return false;
    }
}

void GpuSqlListener::stringToUpper(std::string &str)
{
    for (auto &c : str)
    {
        c = toupper(c);
    }
}
