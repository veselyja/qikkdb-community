#include <boost/filesystem.hpp>

#include "gtest/gtest.h"
#include "../qikkDB/Database.h"
#include "../qikkDB/Table.h"
#include "../qikkDB/ColumnBase.h"
#include "../qikkDB/Configuration.h"
#include "../qikkDB/ComplexPolygonFactory.h"
#include "../qikkDB/PointFactory.h"

class DatabaseTests : public ::testing::Test
{
protected:
    const std::string path = Configuration::GetInstance().GetTestDatabaseDir();
    const std::string dbName = "TestDatabase";
    const int32_t blockSize = 4; // length of a block

    std::shared_ptr<Database> database;
    std::shared_ptr<Database> renameDatabase;
    virtual void SetUp()
    {
        database = std::make_shared<Database>(dbName.c_str(), blockSize);

        // clear directory to make sure, there are no old database files, but do not remove directory:
        boost::filesystem::path path_to_remove(path);
        for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it)
        {
            boost::filesystem::remove_all(it->path());
        }
    }

    virtual void TearDown()
    {
        // clean up occurs when test completes or an exception is thrown
        Database::RemoveFromInMemoryDatabaseList(dbName.c_str());

        // clear directory to make sure, there are no old database files, but do not remove directory:
        boost::filesystem::path path_to_remove(path);
        for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it)
        {
            boost::filesystem::remove_all(it->path());
        }
    }
};


/// Integration test - tests the following fucntions and procedures:
///  - Persist()
///  - SaveAllToDisk()
///  - SaveModifiedToDisk()
///  - LoadDatabasesFromDisk()
///  - LoadDatabase()
///  - LoadColumns()
///  - CreateTable()
///  - AddToInMemoryDatabaseList()
///  - DropColumn()
///  - DropTable()
///  - DropDatabase()
TEST_F(DatabaseTests, IntegrationTest)
{
    int32_t blockNum = 2; // number of blocks

    Database::AddToInMemoryDatabaseList(database);

    // create first table with initialized columns:
    std::unordered_map<std::string, DataType> columnsTable1;
    columnsTable1.insert({"colInteger", COLUMN_INT});
    columnsTable1.insert({"colDouble", COLUMN_DOUBLE});
    columnsTable1.insert({"colString", COLUMN_STRING});
    columnsTable1.insert({"colIntegerEmpty", COLUMN_INT});
    columnsTable1.insert({"colStringEmpty", COLUMN_STRING});
    database->CreateTable(columnsTable1, "TestTable1");

    // create second table with initialized columns:
    std::unordered_map<std::string, DataType> columnsTable2;
    std::unordered_map<std::string, bool> columnsTable2Nullabilities;
    columnsTable2.insert({"colInteger", COLUMN_INT});
    columnsTable2.insert({"colDouble", COLUMN_DOUBLE});
    columnsTable2.insert({"colString", COLUMN_STRING});
    columnsTable2.insert({"colFloat", COLUMN_FLOAT});
    columnsTable2.insert({"colLong", COLUMN_LONG});
    columnsTable2.insert({"colPolygon", COLUMN_POLYGON});
    columnsTable2.insert({"colPoint", COLUMN_POINT});
    columnsTable2.insert({"colBool", COLUMN_INT8_T});
    columnsTable2Nullabilities.insert({"colInteger", true});
    columnsTable2Nullabilities.insert({"colDouble", false});
    columnsTable2Nullabilities.insert({"colString", false});
    columnsTable2Nullabilities.insert({"colFloat", false});
    columnsTable2Nullabilities.insert({"colLong", false});
    columnsTable2Nullabilities.insert({"colPolygon", false});
    columnsTable2Nullabilities.insert({"colPoint", false});
    columnsTable2Nullabilities.insert({"colBool", false});
    database->CreateTable(columnsTable2, "TestTable2", columnsTable2Nullabilities);

    auto& tables = database->GetTables();

    auto& table1 = tables.at("TestTable1");
    auto& colInteger1 = table1.GetColumns().at("colInteger");
    auto& colDouble1 = table1.GetColumns().at("colDouble");
    auto& colString1 = table1.GetColumns().at("colString");

    auto& table2 = tables.at("TestTable2");
    auto& colInteger2 = table2.GetColumns().at("colInteger");
    auto& colDouble2 = table2.GetColumns().at("colDouble");
    auto& colString2 = table2.GetColumns().at("colString");
    auto& colFloat2 = table2.GetColumns().at("colFloat");
    auto& colLong2 = table2.GetColumns().at("colLong");
    auto& colPolygon2 = table2.GetColumns().at("colPolygon");
    auto& colPoint2 = table2.GetColumns().at("colPoint");
    auto& colBool2 = table2.GetColumns().at("colBool");

    for (int i = 0; i < blockNum; i++)
    {
        // insert data to the first table:
        std::vector<int32_t> dataInteger1;
        dataInteger1.push_back(13);
        dataInteger1.push_back(-2);
        dataInteger1.push_back(1399);
        dynamic_cast<ColumnBase<int32_t>*>(colInteger1.get())->AddBlock(dataInteger1);

        std::vector<double> dataDouble1;
        dataDouble1.push_back(45.98924);
        dataDouble1.push_back(999.6665);
        dataDouble1.push_back(1.787985);
        dynamic_cast<ColumnBase<double>*>(colDouble1.get())->AddBlock(dataDouble1);

        std::vector<std::string> dataString1;
        dataString1.push_back("QikkDB");
        dataString1.push_back("FastestDBinTheWorld");
        dataString1.push_back("Speed is my second name");
        dynamic_cast<ColumnBase<std::string>*>(colString1.get())->AddBlock(dataString1);

        // insert data to the second table:
        std::vector<int32_t> dataInteger2;
        dataInteger2.push_back(1893);
        dataInteger2.push_back(-654);
        dataInteger2.push_back(196);
        dynamic_cast<ColumnBase<int32_t>*>(colInteger2.get())->AddBlock(dataInteger2);

        std::vector<double> dataDouble2;
        dataDouble2.push_back(65.77924);
        dataDouble2.push_back(9789.685);
        dataDouble2.push_back(9.797965);
        dynamic_cast<ColumnBase<double>*>(colDouble2.get())->AddBlock(dataDouble2);

        std::vector<std::string> dataString2;
        dataString2.push_back("Drop database_");
        dataString2.push_back("Is this the fastest DB?");
        dataString2.push_back("Speed of electron");
        dynamic_cast<ColumnBase<std::string>*>(colString2.get())->AddBlock(dataString2);

        std::vector<float> dataFloat2;
        dataFloat2.push_back(456.2);
        dataFloat2.push_back(12.45);
        dataFloat2.push_back(8.965);
        dynamic_cast<ColumnBase<float>*>(colFloat2.get())->AddBlock(dataFloat2);

        std::vector<int64_t> dataLong2;
        dataLong2.push_back(489889498840);
        dataLong2.push_back(165648654445);
        dataLong2.push_back(256854586987);
        dynamic_cast<ColumnBase<int64_t>*>(colLong2.get())->AddBlock(dataLong2);

        std::vector<QikkDB::Types::ComplexPolygon> dataPolygon2;
        dataPolygon2.push_back(ComplexPolygonFactory::FromWkt(
            "POLYGON((10 11, 11.11 12.13, 10 11),(21 30, 35.55 36, 30.11 20.26, 21 30),(61 "
            "80.11,90 89.15,112.12 110, 61 80.11))"));
        dataPolygon2.push_back(ComplexPolygonFactory::FromWkt(
            "POLYGON((15 11, 11.11 12.13, 15 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
            "87.11,90 89.15,112.12 110, 61 87.11))"));
        dataPolygon2.push_back(ComplexPolygonFactory::FromWkt(
            "POLYGON((15 18, 11.11 12.13, 15 18),(21 38,35.55 36, 30.11 20.26,21 38), (64 80.11,90 "
            "89.15,112.12 110, 64 80.11))"));
        dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(colPolygon2.get())->AddBlock(dataPolygon2);

        std::vector<QikkDB::Types::Point> dataPoint2;
        dataPoint2.push_back(PointFactory::FromWkt("POINT(10.11 11.1)"));
        dataPoint2.push_back(PointFactory::FromWkt("POINT(12 11.15)"));
        dataPoint2.push_back(PointFactory::FromWkt("POINT(9 8)"));
        dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(colPoint2.get())->AddBlock(dataPoint2);

        std::vector<int8_t> dataBool2;
        dataBool2.push_back(-1);
        dataBool2.push_back(0);
        dataBool2.push_back(1);
        dynamic_cast<ColumnBase<int8_t>*>(colBool2.get())->AddBlock(dataBool2);
    }

    const std::string storePath = path + dbName;
    boost::filesystem::remove_all(storePath);

    Database::SaveModifiedToDisk();

    for (auto& db : Database::GetDatabaseNames())
    {
        Database::RemoveFromInMemoryDatabaseList(db.c_str());
    }

    // load different database_, but with the same data:
    Database::LoadDatabasesFromDisk();

    auto& loadedTables = Database::GetDatabaseByName(dbName)->GetTables();
    auto& firstTableColumns = loadedTables.at("TestTable1").GetColumns();
    auto& secondTableColumns = loadedTables.at("TestTable2").GetColumns();

    // high level stuff:
    ASSERT_EQ(loadedTables.size(), 2);
    ASSERT_EQ(firstTableColumns.size(), 5);
    ASSERT_EQ(secondTableColumns.size(), 8);

    // first table block counts:
    ASSERT_EQ((firstTableColumns.at("colInteger").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((firstTableColumns.at("colDouble").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((firstTableColumns.at("colString").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((firstTableColumns.at("colIntegerEmpty").get())->GetBlockCount(), 0);
    ASSERT_EQ((firstTableColumns.at("colStringEmpty").get())->GetBlockCount(), 0);

    // first table default values:
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colInteger").get())->GetDefaultValue(), 0);
    ASSERT_EQ(dynamic_cast<ColumnBase<double>*>(firstTableColumns.at("colDouble").get())->GetDefaultValue(), 0.000000);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colString").get())->GetDefaultValue(), "");
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colIntegerEmpty").get())->GetDefaultValue(), 0);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colStringEmpty").get())->GetDefaultValue(),
              "");

    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colInteger").get())
                  ->GetBlocksList()
                  .at(0)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colInteger").get())
                  ->GetBlocksList()
                  .at(1)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<double>*>(firstTableColumns.at("colDouble").get())
                  ->GetBlocksList()
                  .at(0)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<double>*>(firstTableColumns.at("colDouble").get())
                  ->GetBlocksList()
                  .at(1)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colString").get())
                  ->GetBlocksList()
                  .at(0)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colString").get())
                  ->GetBlocksList()
                  .at(1)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colIntegerEmpty").get())
                  ->GetBlocksList()
                  .size(),
              0);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colStringEmpty").get())
                  ->GetBlocksList()
                  .size(),
              0);

    // first table nullability of columns:
    ASSERT_TRUE((firstTableColumns.at("colInteger").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns.at("colDouble").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns.at("colString").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns.at("colIntegerEmpty").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns.at("colStringEmpty").get())->GetIsNullable());

    // first table colInteger:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colInteger").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        auto block = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colInteger").get())
                         ->GetBlocksList()
                         .at(i);
        ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns.at("colInteger").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_EQ(data[0], 13);
        ASSERT_EQ(data[1], -2);
        ASSERT_EQ(data[2], 1399);

        ASSERT_EQ(block->GetMin(), -2);
        ASSERT_EQ(block->GetMax(), 1399);
        ASSERT_EQ(block->GetSum(), 1410);
        ASSERT_EQ(block->GetAvg(), 470);
    }

    // first table colDouble:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<double>*>(firstTableColumns.at("colDouble").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        auto block = dynamic_cast<ColumnBase<double>*>(firstTableColumns.at("colDouble").get())
                         ->GetBlocksList()
                         .at(i);
        ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns.at("colDouble").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_DOUBLE_EQ(data[0], 45.98924);
        ASSERT_DOUBLE_EQ(data[1], 999.6665);
        ASSERT_DOUBLE_EQ(data[2], 1.787985);

        ASSERT_DOUBLE_EQ(block->GetMin(), 1.787985);
        ASSERT_DOUBLE_EQ(block->GetMax(), 999.6665);
        ASSERT_FLOAT_EQ(block->GetSum(), 1047.44372f);
        ASSERT_FLOAT_EQ(block->GetAvg(), 349.147908f);
    }

    // first table colString:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colString").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        auto block = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colString").get())
                         ->GetBlocksList()
                         .at(i);
        ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns.at("colString").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_EQ(data[0], "QikkDB");
        ASSERT_EQ(data[1], "FastestDBinTheWorld");
        ASSERT_EQ(data[2], "Speed is my second name");

        ASSERT_TRUE(block->GetMin() == "FastestDBinTheWorld");
        ASSERT_TRUE(block->GetMax() == "Speed is my second name");
        ASSERT_TRUE(block->GetSum() == "");
        ASSERT_FLOAT_EQ(block->GetAvg(), 0.0f);
    }

    // second table block count:
    ASSERT_EQ((secondTableColumns.at("colInteger").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colDouble").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colString").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colFloat").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colLong").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colPolygon").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colPoint").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns.at("colBool").get())->GetBlockCount(), blockNum);

    // second table default values:
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())->GetDefaultValue(), 0);
    ASSERT_DOUBLE_EQ(dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())->GetDefaultValue(),
                     0.000000);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())->GetDefaultValue(), "");
    ASSERT_FLOAT_EQ(dynamic_cast<ColumnBase<float>*>(secondTableColumns.at("colFloat").get())->GetDefaultValue(),
                    0.000000);
    ASSERT_EQ(dynamic_cast<ColumnBase<int64_t>*>(secondTableColumns.at("colLong").get())->GetDefaultValue(), 0);
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                                                        secondTableColumns.at("colPolygon").get())
                                                        ->GetDefaultValue()),
              "POLYGON((0 0, 1 1, 2 2, 0 0))");
    ASSERT_EQ(PointFactory::WktFromPoint(
                  dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns.at("colPoint").get())
                      ->GetDefaultValue()),
              "POINT(0 0)");
    ASSERT_EQ(dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns.at("colBool").get())->GetDefaultValue(), 0);

    // second table nullability of columns:
    ASSERT_TRUE((secondTableColumns.at("colInteger").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colDouble").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colString").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colFloat").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colLong").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colPolygon").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colPoint").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns.at("colBool").get())->GetIsNullable());

    // second table colInteger:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_EQ(data[0], 1893);
        ASSERT_EQ(data[1], -654);
        ASSERT_EQ(data[2], 196);
    }

    // second table colDouble:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        ASSERT_FALSE(dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())
                         ->GetBlocksList()
                         .at(i)
                         ->GetIsNullable());
        ASSERT_DOUBLE_EQ(data[0], 65.77924);
        ASSERT_DOUBLE_EQ(data[1], 9789.685);
        ASSERT_DOUBLE_EQ(data[2], 9.797965);
    }

    // second table colString:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        ASSERT_FALSE(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                         ->GetBlocksList()
                         .at(i)
                         ->GetIsNullable());
        ASSERT_EQ(data[0], "Drop database_");
        ASSERT_EQ(data[1], "Is this the fastest DB?");
        ASSERT_EQ(data[2], "Speed of electron");
    }

    // second table colFloat:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<float>*>(secondTableColumns.at("colFloat").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        ASSERT_FALSE(dynamic_cast<ColumnBase<float>*>(secondTableColumns.at("colFloat").get())
                         ->GetBlocksList()
                         .at(i)
                         ->GetIsNullable());
        ASSERT_FLOAT_EQ(data[0], 456.2);
        ASSERT_FLOAT_EQ(data[1], 12.45);
        ASSERT_FLOAT_EQ(data[2], 8.965);
    }

    // second table colPolygon:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                        secondTableColumns.at("colPolygon").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                         secondTableColumns.at("colPolygon").get())
                         ->GetBlocksList()
                         .at(i)
                         ->GetIsNullable());
        ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data[0]),
                  "POLYGON((10 11, 11.11 12.13, 10 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
                  "80.11, 90 89.15, 112.12 110, 61 80.11))");
        ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data[1]),
                  "POLYGON((15 11, 11.11 12.13, 15 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
                  "87.11, 90 89.15, 112.12 110, 61 87.11))");
        ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data[2]),
                  "POLYGON((15 18, 11.11 12.13, 15 18), (21 38, 35.55 36, 30.11 20.26, 21 38), (64 "
                  "80.11, 90 89.15, 112.12 110, 64 80.11))");
    }

    // second table colPoint:
    for (int i = 0; i < blockNum; i++)
    {
        auto data =
            dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns.at("colPoint").get())
                ->GetBlocksList()
                .at(i)
                ->GetData();
        ASSERT_FALSE(
            dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns.at("colPoint").get())
                ->GetBlocksList()
                .at(i)
                ->GetIsNullable());
        ASSERT_EQ(PointFactory::WktFromPoint(data[0]), "POINT(10.11 11.1)");
        ASSERT_EQ(PointFactory::WktFromPoint(data[1]), "POINT(12 11.15)");
        ASSERT_EQ(PointFactory::WktFromPoint(data[2]), "POINT(9 8)");
    }

    // second table colBool:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns.at("colBool").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        ASSERT_FALSE(dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns.at("colBool").get())
                         ->GetBlocksList()
                         .at(i)
                         ->GetIsNullable());
        ASSERT_EQ(data[0], -1);
        ASSERT_EQ(data[1], 0);
        ASSERT_EQ(data[2], 1);
    }

    // append new data into the second table:
    std::unordered_map<std::string, std::any> dataX;

    std::vector<int32_t> dataInt({1893, -654, 196});
    std::vector<double> dataDouble({65.77924, 9789.685, 9.797965});
    std::vector<std::string> dataString({"Drop database_", "Is this the fastest DB?", "Speed of electron"});
    std::vector<float> dataFloat({456.2, 12.45, 8.965});
    std::vector<int64_t> dataLong({489889498840, 165648654445, 256854586987});
    std::vector<QikkDB::Types::ComplexPolygon> dataPolygon(
        {ComplexPolygonFactory::FromWkt(
             "POLYGON((10 11, 11.11 12.13, 10 11),(21 30, 35.55 36, 30.11 20.26, 21 30),(61 "
             "80.11,90 89.15,112.12 110, 61 80.11))"),
         ComplexPolygonFactory::FromWkt(
             "POLYGON((15 11, 11.11 12.13, 15 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
             "87.11,90 89.15,112.12 110, 61 87.11))"),
         ComplexPolygonFactory::FromWkt("POLYGON((15 18, 11.11 12.13, 15 18),(21 38,35.55 36, "
                                        "30.11 20.26,21 38), (64 80.11,90 "
                                        "89.15,112.12 110, 64 80.11))")});
    std::vector<QikkDB::Types::Point> dataPoint({PointFactory::FromWkt("POINT(10.11 11.1)"),
                                                 PointFactory::FromWkt("POINT(12 11.15)"),
                                                 PointFactory::FromWkt("POINT(9 8)")});
    std::vector<int8_t> dataBool({-1, 0, 1});

    dataX.insert({"colInteger", dataInt});
    dataX.insert({"colLong", dataLong});
    dataX.insert({"colFloat", dataFloat});
    dataX.insert({"colDouble", dataDouble});
    dataX.insert({"colPoint", dataPoint});
    dataX.insert({"colPolygon", dataPolygon});
    dataX.insert({"colString", dataString});
    dataX.insert({"colBool", dataBool});

    loadedTables.at("TestTable2").InsertData(dataX);

    Database::SaveModifiedToDisk();

    Database::LoadDatabasesFromDisk();

    auto& loadedTables2 = Database::GetDatabaseByName(dbName)->GetTables();
    auto& firstTableColumns2 = loadedTables2.at("TestTable1").GetColumns();
    auto& secondTableColumns2 = loadedTables2.at("TestTable2").GetColumns();

    // high level stuff:
    ASSERT_EQ(loadedTables2.size(), 2);
    ASSERT_EQ(firstTableColumns2.size(), 5);
    ASSERT_EQ(secondTableColumns2.size(), 8);

    // first table block counts:
    ASSERT_EQ((firstTableColumns2.at("colInteger").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((firstTableColumns2.at("colDouble").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((firstTableColumns2.at("colString").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((firstTableColumns2.at("colIntegerEmpty").get())->GetBlockCount(), 0);
    ASSERT_EQ((firstTableColumns2.at("colStringEmpty").get())->GetBlockCount(), 0);

    // first table default values:
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())->GetDefaultValue(), 0);
    ASSERT_EQ(dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())->GetDefaultValue(),
              0.000000);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())->GetDefaultValue(), "");
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colIntegerEmpty").get())->GetDefaultValue(),
              0);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colStringEmpty").get())->GetDefaultValue(),
              "");

    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                  ->GetBlocksList()
                  .at(0)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                  ->GetBlocksList()
                  .at(1)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                  ->GetBlocksList()
                  .at(0)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                  ->GetBlocksList()
                  .at(1)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                  ->GetBlocksList()
                  .at(0)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                  ->GetBlocksList()
                  .at(1)
                  ->BlockCapacity(),
              4);
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colIntegerEmpty").get())
                  ->GetBlocksList()
                  .size(),
              0);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colStringEmpty").get())
                  ->GetBlocksList()
                  .size(),
              0);

    // first table nullability of columns:
    ASSERT_TRUE((firstTableColumns2.at("colInteger").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns2.at("colDouble").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns2.at("colString").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns2.at("colIntegerEmpty").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns2.at("colStringEmpty").get())->GetIsNullable());

    // first table colInteger:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        auto block = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                         ->GetBlocksList()
                         .at(i);
        ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_EQ(data[0], 13);
        ASSERT_EQ(data[1], -2);
        ASSERT_EQ(data[2], 1399);

        ASSERT_EQ(block->GetMin(), -2);
        ASSERT_EQ(block->GetMax(), 1399);
        ASSERT_EQ(block->GetSum(), 1410);
        ASSERT_EQ(block->GetAvg(), 470);
    }

    // first table colDouble:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        auto block = dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                         ->GetBlocksList()
                         .at(i);
        ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_DOUBLE_EQ(data[0], 45.98924);
        ASSERT_DOUBLE_EQ(data[1], 999.6665);
        ASSERT_DOUBLE_EQ(data[2], 1.787985);

        ASSERT_DOUBLE_EQ(block->GetMin(), 1.787985);
        ASSERT_DOUBLE_EQ(block->GetMax(), 999.6665);
        ASSERT_FLOAT_EQ(block->GetSum(), 1047.44372f);
        ASSERT_FLOAT_EQ(block->GetAvg(), 349.147908f);
    }

    // first table colString:
    for (int i = 0; i < blockNum; i++)
    {
        auto data = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetData();
        auto block = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                         ->GetBlocksList()
                         .at(i);
        ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                        ->GetBlocksList()
                        .at(i)
                        ->GetIsNullable());
        ASSERT_EQ(data[0], "QikkDB");
        ASSERT_EQ(data[1], "FastestDBinTheWorld");
        ASSERT_EQ(data[2], "Speed is my second name");

        ASSERT_TRUE(block->GetMin() == "FastestDBinTheWorld");
        ASSERT_TRUE(block->GetMax() == "Speed is my second name");
        ASSERT_TRUE(block->GetSum() == "");
        ASSERT_FLOAT_EQ(block->GetAvg(), 0.0f);
    }

    blockNum++;

    // second table block count:
    ASSERT_EQ((secondTableColumns2.at("colInteger").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colDouble").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colString").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colFloat").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colLong").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colPolygon").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colPoint").get())->GetBlockCount(), blockNum);
    ASSERT_EQ((secondTableColumns2.at("colBool").get())->GetBlockCount(), blockNum);

    // second table default values:
    ASSERT_EQ(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())->GetDefaultValue(), 0);
    ASSERT_DOUBLE_EQ(dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())->GetDefaultValue(),
                     0.000000);
    ASSERT_EQ(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())->GetDefaultValue(),
              "");
    ASSERT_FLOAT_EQ(dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())->GetDefaultValue(),
                    0.000000);
    ASSERT_EQ(dynamic_cast<ColumnBase<int64_t>*>(secondTableColumns2.at("colLong").get())->GetDefaultValue(), 0);
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                                                        secondTableColumns2.at("colPolygon").get())
                                                        ->GetDefaultValue()),
              "POLYGON((0 0, 1 1, 2 2, 0 0))");
    ASSERT_EQ(PointFactory::WktFromPoint(dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(
                                             secondTableColumns2.at("colPoint").get())
                                             ->GetDefaultValue()),
              "POINT(0 0)");
    ASSERT_EQ(dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())->GetDefaultValue(), 0);

    // second table nullability of columns:
    ASSERT_TRUE((secondTableColumns2.at("colInteger").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colDouble").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colString").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colFloat").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colLong").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colPolygon").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colPoint").get())->GetIsNullable());
    ASSERT_FALSE((secondTableColumns2.at("colBool").get())->GetIsNullable());

    // second table colInteger:
    auto data = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetData();
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data[0], 1893);
    ASSERT_EQ(data[1], -654);
    ASSERT_EQ(data[2], 196);

    data = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())
               ->GetBlocksList()
               .at(1)
               ->GetData();
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data[0], 1893);
    ASSERT_EQ(data[1], -654);
    ASSERT_EQ(data[2], 196);
    ASSERT_EQ(data[3], 1893);

    data = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())
               ->GetBlocksList()
               .at(2)
               ->GetData();
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns2.at("colInteger").get())
                    ->GetBlocksList()
                    .at(2)
                    ->GetIsNullable());
    ASSERT_EQ(data[0], -654);
    ASSERT_EQ(data[1], 196);

    // second table colDouble:
    auto data2 = dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data2[0], 65.77924);
    ASSERT_DOUBLE_EQ(data2[1], 9789.685);
    ASSERT_DOUBLE_EQ(data2[2], 9.797965);

    data2 = dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data2[0], 65.77924);
    ASSERT_DOUBLE_EQ(data2[1], 9789.685);
    ASSERT_DOUBLE_EQ(data2[2], 9.797965);
    ASSERT_DOUBLE_EQ(data2[3], 65.77924);

    data2 = dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<double>*>(secondTableColumns2.at("colDouble").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data2[0], 9789.685);
    ASSERT_DOUBLE_EQ(data2[1], 9.797965);

    // second table colString:
    auto data3 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetIsNullable());
    ASSERT_EQ(data3[0], "Drop database_");
    ASSERT_EQ(data3[1], "Is this the fastest DB?");
    ASSERT_EQ(data3[2], "Speed of electron");

    data3 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetIsNullable());
    ASSERT_EQ(data3[0], "Drop database_");
    ASSERT_EQ(data3[1], "Is this the fastest DB?");
    ASSERT_EQ(data3[2], "Speed of electron");
    ASSERT_EQ(data3[3], "Drop database_");

    data3 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns2.at("colString").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetIsNullable());
    ASSERT_EQ(data3[0], "Is this the fastest DB?");
    ASSERT_EQ(data3[1], "Speed of electron");

    // second table colFloat:
    auto data4 = dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetIsNullable());
    ASSERT_FLOAT_EQ(data4[0], 456.2);
    ASSERT_FLOAT_EQ(data4[1], 12.45);
    ASSERT_FLOAT_EQ(data4[2], 8.965);

    data4 = dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetIsNullable());
    ASSERT_FLOAT_EQ(data4[0], 456.2);
    ASSERT_FLOAT_EQ(data4[1], 12.45);
    ASSERT_FLOAT_EQ(data4[2], 8.965);
    ASSERT_FLOAT_EQ(data4[3], 456.2);

    data4 = dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<float>*>(secondTableColumns2.at("colFloat").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetIsNullable());
    ASSERT_FLOAT_EQ(data4[0], 12.45);
    ASSERT_FLOAT_EQ(data4[1], 8.965);

    // second table colPolygon:
    auto data5 = dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                     secondTableColumns2.at("colPolygon").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                     secondTableColumns2.at("colPolygon").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetIsNullable());
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[0]),
              "POLYGON((10 11, 11.11 12.13, 10 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
              "80.11, 90 89.15, 112.12 110, 61 80.11))");
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[1]),
              "POLYGON((15 11, 11.11 12.13, 15 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
              "87.11, 90 89.15, 112.12 110, 61 87.11))");
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[2]),
              "POLYGON((15 18, 11.11 12.13, 15 18), (21 38, 35.55 36, 30.11 20.26, 21 38), (64 "
              "80.11, 90 89.15, 112.12 110, 64 80.11))");

    data5 = dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                secondTableColumns2.at("colPolygon").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                     secondTableColumns2.at("colPolygon").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetIsNullable());
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[0]),
              "POLYGON((10 11, 11.11 12.13, 10 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
              "80.11, 90 89.15, 112.12 110, 61 80.11))");
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[1]),
              "POLYGON((15 11, 11.11 12.13, 15 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
              "87.11, 90 89.15, 112.12 110, 61 87.11))");
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[2]),
              "POLYGON((15 18, 11.11 12.13, 15 18), (21 38, 35.55 36, 30.11 20.26, 21 38), (64 "
              "80.11, 90 89.15, 112.12 110, 64 80.11))");
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[3]),
              "POLYGON((10 11, 11.11 12.13, 10 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
              "80.11, 90 89.15, 112.12 110, 61 80.11))");

    data5 = dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                secondTableColumns2.at("colPolygon").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::ComplexPolygon>*>(
                     secondTableColumns2.at("colPolygon").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetIsNullable());
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[0]),
              "POLYGON((15 11, 11.11 12.13, 15 11), (21 30, 35.55 36, 30.11 20.26, 21 30), (61 "
              "87.11, 90 89.15, 112.12 110, 61 87.11))");
    ASSERT_EQ(ComplexPolygonFactory::WktFromPolygon(data5[1]),
              "POLYGON((15 18, 11.11 12.13, 15 18), (21 38, 35.55 36, 30.11 20.26, 21 38), (64 "
              "80.11, 90 89.15, 112.12 110, 64 80.11))");

    // second table colPoint:
    auto data6 = dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns2.at("colPoint").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns2.at("colPoint").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetIsNullable());
    ASSERT_EQ(PointFactory::WktFromPoint(data6[0]), "POINT(10.11 11.1)");
    ASSERT_EQ(PointFactory::WktFromPoint(data6[1]), "POINT(12 11.15)");
    ASSERT_EQ(PointFactory::WktFromPoint(data6[2]), "POINT(9 8)");

    data6 = dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns2.at("colPoint").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns2.at("colPoint").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetIsNullable());
    ASSERT_EQ(PointFactory::WktFromPoint(data6[0]), "POINT(10.11 11.1)");
    ASSERT_EQ(PointFactory::WktFromPoint(data6[1]), "POINT(12 11.15)");
    ASSERT_EQ(PointFactory::WktFromPoint(data6[2]), "POINT(9 8)");
    ASSERT_EQ(PointFactory::WktFromPoint(data6[3]), "POINT(10.11 11.1)");

    data6 = dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns2.at("colPoint").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<QikkDB::Types::Point>*>(secondTableColumns2.at("colPoint").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetIsNullable());
    ASSERT_EQ(PointFactory::WktFromPoint(data6[0]), "POINT(12 11.15)");
    ASSERT_EQ(PointFactory::WktFromPoint(data6[1]), "POINT(9 8)");

    // second table colBool:
    auto data7 = dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetIsNullable());
    ASSERT_EQ(data7[0], -1);
    ASSERT_EQ(data7[1], 0);
    ASSERT_EQ(data7[2], 1);

    data7 = dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetIsNullable());
    ASSERT_EQ(data7[0], -1);
    ASSERT_EQ(data7[1], 0);
    ASSERT_EQ(data7[2], 1);
    ASSERT_EQ(data7[3], -1);

    data7 = dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    ASSERT_FALSE(dynamic_cast<ColumnBase<int8_t>*>(secondTableColumns2.at("colBool").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetIsNullable());
    ASSERT_EQ(data7[0], 0);
    ASSERT_EQ(data7[1], 1);

    Database::SaveAllToDisk();

    // drop column colBool:
    std::string filePath = Configuration::GetInstance().GetDatabaseDir() + dbName + Database::SEPARATOR +
                           "TestTable2" + Database::SEPARATOR + "colBool" + Database::COLUMN_DATA_EXTENSION;
    ASSERT_TRUE(boost::filesystem::exists(filePath)); // should exist before deletion
    database->DeleteColumnFromDisk(std::string("TestTable2").c_str(), std::string("colBool").c_str());
    ASSERT_FALSE(boost::filesystem::exists(filePath)); // should not exist after deletion

    // drop table TestTable2:
    database->DeleteTableFromDisk(std::string("TestTable2").c_str());
    bool deleted = true;

    std::string prefix = Configuration::GetInstance().GetDatabaseDir() + dbName +
                         Database::SEPARATOR + "TestTable2" + Database::SEPARATOR;
    // Replace backslash with slash just to be sure in comparison
    std::replace(prefix.begin(), prefix.end(), '\\', '/');

    for (auto& p : boost::filesystem::directory_iterator(Configuration::GetInstance().GetDatabaseDir()))
    {
        // Replace backslash with slash for comparison with prefix
        std::string columnPath = p.path().string();
        std::replace(columnPath.begin(), columnPath.end(), '\\', '/');

        // Check files which starts with prefix of db name and table name
        if (!columnPath.compare(0, prefix.size(), prefix))
        {
            deleted = false;
        }
    }
    ASSERT_TRUE(deleted) << "DeleteTableFromDisk";

    // drop database_ TestDatabase:
    database->DeleteDatabaseFromDisk();
    deleted = true;

    prefix = Configuration::GetInstance().GetDatabaseDir() + dbName + Database::SEPARATOR;
    // Replace backslash with slash just to be sure in comparison
    std::replace(prefix.begin(), prefix.end(), '\\', '/');

    for (auto& p : boost::filesystem::directory_iterator(Configuration::GetInstance().GetDatabaseDir()))
    {
        // Replace backslash with slash for comparison with prefix
        std::string columnPath = p.path().string();
        std::replace(columnPath.begin(), columnPath.end(), '\\', '/');

        // Check files which starts with prefix of db name
        if (!columnPath.compare(0, prefix.size(), prefix))
        {
            deleted = false;
        }
    }
    ASSERT_TRUE(deleted) << "DeleteDatabaseFromDisk";
    Database::RemoveFromInMemoryDatabaseList("TestDatabase");
}

/// Integration test - tests the following fucntions and procedures:
///  - ChangeTableBlockSize()
///  - ChangeDatabaseBlockSize()
TEST_F(DatabaseTests, ChangeBlockSize)
{
    const int32_t blockNum = 2; // number of blocks

    // create first table with initialized columns:
    std::unordered_map<std::string, DataType> columnsTable1;
    columnsTable1.insert({"colInteger", COLUMN_INT});
    columnsTable1.insert({"colDouble", COLUMN_DOUBLE});
    columnsTable1.insert({"colString", COLUMN_STRING});
    database->CreateTable(columnsTable1, "TestTable1");

    auto& tables1 = database->GetTables();

    auto& table1 = tables1.at("TestTable1");
    auto& colInteger1 = table1.GetColumns().at("colInteger");
    auto& colDouble1 = table1.GetColumns().at("colDouble");
    auto& colString1 = table1.GetColumns().at("colString");

    for (int i = 0; i < blockNum; i++)
    {
        // insert data to the first table:
        std::vector<int32_t> dataInteger1;
        dataInteger1.push_back(13);
        dataInteger1.push_back(-2);
        dataInteger1.push_back(1399);
        dynamic_cast<ColumnBase<int32_t>*>(colInteger1.get())->AddBlock(dataInteger1);

        std::vector<double> dataDouble1;
        dataDouble1.push_back(45.98924);
        dataDouble1.push_back(999.6665);
        dataDouble1.push_back(1.787985);
        dynamic_cast<ColumnBase<double>*>(colDouble1.get())->AddBlock(dataDouble1);

        std::vector<std::string> dataString1;
        dataString1.push_back("QikkDB");
        dataString1.push_back("FastestDBinTheWorld");
        dataString1.push_back("Speed is my second name");
        dynamic_cast<ColumnBase<std::string>*>(colString1.get())->AddBlock(dataString1);
    }

    database->ChangeTableBlockSize("TestTable1", 2);

    auto& loadedTables1 = database->GetTables();
    auto& firstTableColumns1 = loadedTables1.at("TestTable1").GetColumns();

    // first table block counts:
    ASSERT_EQ((firstTableColumns1.at("colInteger").get())->GetBlockCount(), 3);
    ASSERT_EQ((firstTableColumns1.at("colDouble").get())->GetBlockCount(), 3);
    ASSERT_EQ((firstTableColumns1.at("colString").get())->GetBlockCount(), 3);

    // first table nullability of columns:
    ASSERT_TRUE((firstTableColumns1.at("colInteger").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns1.at("colDouble").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns1.at("colString").get())->GetIsNullable());

    // first table colInteger, first block:
    auto data = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetData();
    auto block = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                     ->GetBlocksList()
                     .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data[0], 13);
    ASSERT_EQ(data[1], -2);

    ASSERT_EQ(block->GetMin(), -2);
    ASSERT_EQ(block->GetMax(), 13);
    ASSERT_EQ(block->GetSum(), 11);
    ASSERT_EQ(block->GetAvg(), 5.50);

    // first table colInteger, second block:
    data = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
               ->GetBlocksList()
               .at(1)
               ->GetData();
    block = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                ->GetBlocksList()
                .at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data[0], 1399);
    ASSERT_EQ(data[1], 13);

    ASSERT_EQ(block->GetMin(), 13);
    ASSERT_EQ(block->GetMax(), 1399);
    ASSERT_EQ(block->GetSum(), 1412);
    ASSERT_EQ(block->GetAvg(), 706);

    // first table colInteger, third block:
    data = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
               ->GetBlocksList()
               .at(2)
               ->GetData();
    block = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                ->GetBlocksList()
                .at(2);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns1.at("colInteger").get())
                    ->GetBlocksList()
                    .at(2)
                    ->GetIsNullable());
    ASSERT_EQ(data[0], -2);
    ASSERT_EQ(data[1], 1399);

    ASSERT_EQ(block->GetMin(), -2);
    ASSERT_EQ(block->GetMax(), 1399);
    ASSERT_EQ(block->GetSum(), 1397);
    ASSERT_EQ(block->GetAvg(), 698.50);

    // first table colDouble, first block:
    auto data2 = dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block2 =
        dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())->GetBlocksList().at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data2[0], 45.98924);
    ASSERT_DOUBLE_EQ(data2[1], 999.6665);

    ASSERT_DOUBLE_EQ(block2->GetMin(), 45.98924);
    ASSERT_DOUBLE_EQ(block2->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block2->GetSum(), 1045.6558f);
    ASSERT_FLOAT_EQ(block2->GetAvg(), 522.82787f);

    // first table colDouble, second block:
    data2 = dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())
                ->GetBlocksList()
                .at(1)
                ->GetData();
    block2 =
        dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())->GetBlocksList().at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data2[0], 1.787985);
    ASSERT_DOUBLE_EQ(data2[1], 45.98924);

    ASSERT_DOUBLE_EQ(block2->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block2->GetMax(), 45.98924);
    ASSERT_FLOAT_EQ(block2->GetSum(), 47.777225f);
    ASSERT_FLOAT_EQ(block2->GetAvg(), 23.8886125f);

    // first table colDouble, third block:
    data2 = dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())
                ->GetBlocksList()
                .at(2)
                ->GetData();
    block2 =
        dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())->GetBlocksList().at(2);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns1.at("colDouble").get())
                    ->GetBlocksList()
                    .at(2)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data2[0], 999.6665);
    ASSERT_DOUBLE_EQ(data2[1], 1.787985);

    ASSERT_DOUBLE_EQ(block2->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block2->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block2->GetSum(), 1001.454485f);
    ASSERT_FLOAT_EQ(block2->GetAvg(), 500.7272425f);

    // first table colString, first block:
    auto data3 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block3 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                      ->GetBlocksList()
                      .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data3[0], "QikkDB");
    ASSERT_EQ(data3[1], "FastestDBinTheWorld");

    ASSERT_EQ(block3->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block3->GetMax(), "QikkDB");
    ASSERT_EQ(block3->GetSum(), "");
    ASSERT_FLOAT_EQ(block3->GetAvg(), 0.0f);

    // first table colString, second block:
    auto data4 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                     ->GetBlocksList()
                     .at(1)
                     ->GetData();
    auto block4 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                      ->GetBlocksList()
                      .at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data4[0], "Speed is my second name");
    ASSERT_EQ(data4[1], "QikkDB");

    ASSERT_EQ(block4->GetMin(), "QikkDB");
    ASSERT_EQ(block4->GetMax(), "Speed is my second name");
    ASSERT_EQ(block4->GetSum(), "");
    ASSERT_FLOAT_EQ(block4->GetAvg(), 0.0f);

    // first table colString, third block:
    auto data5 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                     ->GetBlocksList()
                     .at(2)
                     ->GetData();
    auto block5 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                      ->GetBlocksList()
                      .at(2);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns1.at("colString").get())
                    ->GetBlocksList()
                    .at(2)
                    ->GetIsNullable());
    ASSERT_EQ(data5[0], "FastestDBinTheWorld");
    ASSERT_EQ(data5[1], "Speed is my second name");

    ASSERT_EQ(block5->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block5->GetMax(), "Speed is my second name");
    ASSERT_EQ(block5->GetSum(), "");
    ASSERT_FLOAT_EQ(block5->GetAvg(), 0.0f);

    // test of resizing the whole database:
    database->ChangeDatabaseBlockSize(6);

    auto& loadedTables2 = database->GetTables();
    auto& firstTableColumns2 = loadedTables2.at("TestTable1").GetColumns();

    // first table block counts:
    ASSERT_EQ((firstTableColumns2.at("colInteger").get())->GetBlockCount(), 1);
    ASSERT_EQ((firstTableColumns2.at("colDouble").get())->GetBlockCount(), 1);
    ASSERT_EQ((firstTableColumns2.at("colString").get())->GetBlockCount(), 1);

    // first table nullability of columns:
    ASSERT_TRUE((firstTableColumns2.at("colInteger").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns2.at("colDouble").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns2.at("colString").get())->GetIsNullable());

    // first table colInteger, first block:
    auto data6 = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block6 = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                      ->GetBlocksList()
                      .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns2.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data6[0], 13);
    ASSERT_EQ(data6[1], -2);
    ASSERT_EQ(data6[2], 1399);
    ASSERT_EQ(data6[3], 13);
    ASSERT_EQ(data6[4], -2);
    ASSERT_EQ(data6[5], 1399);

    ASSERT_EQ(block6->GetMin(), -2);
    ASSERT_EQ(block6->GetMax(), 1399);
    ASSERT_EQ(block6->GetSum(), 2820);
    ASSERT_EQ(block6->GetAvg(), 470);

    // first table colDouble, first block:
    auto data7 = dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block7 =
        dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())->GetBlocksList().at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns2.at("colDouble").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data7[0], 45.98924);
    ASSERT_DOUBLE_EQ(data7[1], 999.6665);
    ASSERT_DOUBLE_EQ(data7[2], 1.787985);
    ASSERT_DOUBLE_EQ(data7[3], 45.98924);
    ASSERT_DOUBLE_EQ(data7[4], 999.6665);
    ASSERT_DOUBLE_EQ(data7[5], 1.787985);

    ASSERT_DOUBLE_EQ(block7->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block7->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block7->GetSum(), 2094.88745f);
    ASSERT_FLOAT_EQ(block7->GetAvg(), 349.1479083f);

    // first table colString, first block:
    auto data8 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block8 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                      ->GetBlocksList()
                      .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns2.at("colString").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data8[0], "QikkDB");
    ASSERT_EQ(data8[1], "FastestDBinTheWorld");
    ASSERT_EQ(data8[2], "Speed is my second name");
    ASSERT_EQ(data8[3], "QikkDB");
    ASSERT_EQ(data8[4], "FastestDBinTheWorld");
    ASSERT_EQ(data8[5], "Speed is my second name");

    ASSERT_EQ(block8->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block8->GetMax(), "Speed is my second name");
    ASSERT_EQ(block8->GetSum(), "");
    ASSERT_FLOAT_EQ(block8->GetAvg(), 0.0f);

    // add second table and change the whole database block size
    // create second table with initialized columns:
    std::unordered_map<std::string, DataType> columnsTable2;
    columnsTable2.insert({"colInteger", COLUMN_INT});
    columnsTable2.insert({"colDouble", COLUMN_DOUBLE});
    columnsTable2.insert({"colString", COLUMN_STRING});
    database->CreateTable(columnsTable2, "TestTable2");

    auto& tables2 = database->GetTables();

    auto& table2 = tables2.at("TestTable2");
    auto& colInteger2 = table2.GetColumns().at("colInteger");
    auto& colDouble2 = table2.GetColumns().at("colDouble");
    auto& colString2 = table2.GetColumns().at("colString");

    for (int i = 0; i < blockNum; i++)
    {
        // insert data to the first table:
        std::vector<int32_t> dataInteger2;
        dataInteger2.push_back(13);
        dataInteger2.push_back(-2);
        dataInteger2.push_back(1399);
        dynamic_cast<ColumnBase<int32_t>*>(colInteger2.get())->AddBlock(dataInteger2);

        std::vector<double> dataDouble2;
        dataDouble2.push_back(45.98924);
        dataDouble2.push_back(999.6665);
        dataDouble2.push_back(1.787985);
        dynamic_cast<ColumnBase<double>*>(colDouble2.get())->AddBlock(dataDouble2);

        std::vector<std::string> dataString2;
        dataString2.push_back("QikkDB");
        dataString2.push_back("FastestDBinTheWorld");
        dataString2.push_back("Speed is my second name");
        dynamic_cast<ColumnBase<std::string>*>(colString2.get())->AddBlock(dataString2);
    }

    database->ChangeDatabaseBlockSize(4);

    auto& loadedTables3 = database->GetTables();
    auto& firstTableColumns3 = loadedTables3.at("TestTable1").GetColumns();

    // first table block counts:
    ASSERT_EQ((firstTableColumns3.at("colInteger").get())->GetBlockCount(), 2);
    ASSERT_EQ((firstTableColumns3.at("colDouble").get())->GetBlockCount(), 2);
    ASSERT_EQ((firstTableColumns3.at("colString").get())->GetBlockCount(), 2);

    // first table nullability of columns:
    ASSERT_TRUE((firstTableColumns3.at("colInteger").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns3.at("colDouble").get())->GetIsNullable());
    ASSERT_TRUE((firstTableColumns3.at("colString").get())->GetIsNullable());

    // first table colInteger, first block:
    auto data9 = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns3.at("colInteger").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block9 = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns3.at("colInteger").get())
                      ->GetBlocksList()
                      .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns3.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data9[0], 13);
    ASSERT_EQ(data9[1], -2);
    ASSERT_EQ(data9[2], 1399);
    ASSERT_EQ(data9[3], 13);

    ASSERT_EQ(block9->GetMin(), -2);
    ASSERT_EQ(block9->GetMax(), 1399);
    ASSERT_EQ(block9->GetSum(), 1423);
    ASSERT_FLOAT_EQ(block9->GetAvg(), 355.75);

    // first table colInteger, second block:
    auto data10 = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns3.at("colInteger").get())
                      ->GetBlocksList()
                      .at(1)
                      ->GetData();
    auto block10 = dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns3.at("colInteger").get())
                       ->GetBlocksList()
                       .at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(firstTableColumns3.at("colInteger").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data10[0], -2);
    ASSERT_EQ(data10[1], 1399);

    ASSERT_EQ(block10->GetMin(), -2);
    ASSERT_EQ(block10->GetMax(), 1399);
    ASSERT_EQ(block10->GetSum(), 1397);
    ASSERT_FLOAT_EQ(block10->GetAvg(), 698.5);

    // first table colDouble, first block:
    auto data11 = dynamic_cast<ColumnBase<double>*>(firstTableColumns3.at("colDouble").get())
                      ->GetBlocksList()
                      .at(0)
                      ->GetData();
    auto block11 =
        dynamic_cast<ColumnBase<double>*>(firstTableColumns3.at("colDouble").get())->GetBlocksList().at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns3.at("colDouble").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data11[0], 45.98924);
    ASSERT_DOUBLE_EQ(data11[1], 999.6665);
    ASSERT_DOUBLE_EQ(data11[2], 1.787985);
    ASSERT_DOUBLE_EQ(data11[3], 45.98924);

    ASSERT_DOUBLE_EQ(block11->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block11->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block11->GetSum(), 1093.432965f);
    ASSERT_FLOAT_EQ(block11->GetAvg(), 273.35824125f);

    // first table colDouble, second block:
    auto data12 = dynamic_cast<ColumnBase<double>*>(firstTableColumns3.at("colDouble").get())
                      ->GetBlocksList()
                      .at(1)
                      ->GetData();
    auto block12 =
        dynamic_cast<ColumnBase<double>*>(firstTableColumns3.at("colDouble").get())->GetBlocksList().at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(firstTableColumns3.at("colDouble").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data12[0], 999.6665);
    ASSERT_DOUBLE_EQ(data12[1], 1.787985);

    ASSERT_DOUBLE_EQ(block12->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block12->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block12->GetSum(), 1001.454485f);
    ASSERT_FLOAT_EQ(block12->GetAvg(), 500.7272425f);

    // first table colString, first block:
    auto data13 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns3.at("colString").get())
                      ->GetBlocksList()
                      .at(0)
                      ->GetData();
    auto block13 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns3.at("colString").get())
                       ->GetBlocksList()
                       .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns3.at("colString").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data13[0], "QikkDB");
    ASSERT_EQ(data13[1], "FastestDBinTheWorld");
    ASSERT_EQ(data13[2], "Speed is my second name");
    ASSERT_EQ(data13[3], "QikkDB");

    ASSERT_EQ(block13->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block13->GetMax(), "Speed is my second name");
    ASSERT_EQ(block13->GetSum(), "");
    ASSERT_FLOAT_EQ(block13->GetAvg(), 0.0f);

    // first table colString, second block:
    auto data14 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns3.at("colString").get())
                      ->GetBlocksList()
                      .at(1)
                      ->GetData();
    auto block14 = dynamic_cast<ColumnBase<std::string>*>(firstTableColumns3.at("colString").get())
                       ->GetBlocksList()
                       .at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(firstTableColumns3.at("colString").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data14[0], "FastestDBinTheWorld");
    ASSERT_EQ(data14[1], "Speed is my second name");

    ASSERT_EQ(block14->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block14->GetMax(), "Speed is my second name");
    ASSERT_EQ(block14->GetSum(), "");
    ASSERT_FLOAT_EQ(block14->GetAvg(), 0.0f);

	//second table tests:
    auto& secondTableColumns = loadedTables3.at("TestTable2").GetColumns();

    // second table block counts:
    ASSERT_EQ((secondTableColumns.at("colInteger").get())->GetBlockCount(), 2);
    ASSERT_EQ((secondTableColumns.at("colDouble").get())->GetBlockCount(), 2);
    ASSERT_EQ((secondTableColumns.at("colString").get())->GetBlockCount(), 2);

    // second table nullability of columns:
    ASSERT_TRUE((secondTableColumns.at("colInteger").get())->GetIsNullable());
    ASSERT_TRUE((secondTableColumns.at("colDouble").get())->GetIsNullable());
    ASSERT_TRUE((secondTableColumns.at("colString").get())->GetIsNullable());

    // second table colInteger, first block:
    auto data15 = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                     ->GetBlocksList()
                     .at(0)
                     ->GetData();
    auto block15 = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                      ->GetBlocksList()
                      .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data15[0], 13);
    ASSERT_EQ(data15[1], -2);
    ASSERT_EQ(data15[2], 1399);
    ASSERT_EQ(data15[3], 13);

    ASSERT_EQ(block15->GetMin(), -2);
    ASSERT_EQ(block15->GetMax(), 1399);
    ASSERT_EQ(block15->GetSum(), 1423);
    ASSERT_FLOAT_EQ(block15->GetAvg(), 355.75);

    // second table colInteger, second block:
    auto data16 = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                      ->GetBlocksList()
                      .at(1)
                      ->GetData();
    auto block16 = dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                       ->GetBlocksList()
                       .at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<int32_t>*>(secondTableColumns.at("colInteger").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data16[0], -2);
    ASSERT_EQ(data16[1], 1399);

    ASSERT_EQ(block16->GetMin(), -2);
    ASSERT_EQ(block16->GetMax(), 1399);
    ASSERT_EQ(block16->GetSum(), 1397);
    ASSERT_FLOAT_EQ(block16->GetAvg(), 698.5);

    // second table colDouble, first block:
    auto data17 = dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())
                      ->GetBlocksList()
                      .at(0)
                      ->GetData();
    auto block17 =
        dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())->GetBlocksList().at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data17[0], 45.98924);
    ASSERT_DOUBLE_EQ(data17[1], 999.6665);
    ASSERT_DOUBLE_EQ(data17[2], 1.787985);
    ASSERT_DOUBLE_EQ(data17[3], 45.98924);

    ASSERT_DOUBLE_EQ(block17->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block17->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block17->GetSum(), 1093.432965f);
    ASSERT_FLOAT_EQ(block17->GetAvg(), 273.35824125f);

    // second table colDouble, second block:
    auto data18 = dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())
                      ->GetBlocksList()
                      .at(1)
                      ->GetData();
    auto block18 =
        dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())->GetBlocksList().at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<double>*>(secondTableColumns.at("colDouble").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_DOUBLE_EQ(data18[0], 999.6665);
    ASSERT_DOUBLE_EQ(data18[1], 1.787985);

    ASSERT_DOUBLE_EQ(block18->GetMin(), 1.787985);
    ASSERT_DOUBLE_EQ(block18->GetMax(), 999.6665);
    ASSERT_FLOAT_EQ(block18->GetSum(), 1001.454485f);
    ASSERT_FLOAT_EQ(block18->GetAvg(), 500.7272425f);

    // second table colString, first block:
    auto data19 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                      ->GetBlocksList()
                      .at(0)
                      ->GetData();
    auto block19 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                       ->GetBlocksList()
                       .at(0);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                    ->GetBlocksList()
                    .at(0)
                    ->GetIsNullable());
    ASSERT_EQ(data19[0], "QikkDB");
    ASSERT_EQ(data19[1], "FastestDBinTheWorld");
    ASSERT_EQ(data19[2], "Speed is my second name");
    ASSERT_EQ(data19[3], "QikkDB");

    ASSERT_EQ(block19->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block19->GetMax(), "Speed is my second name");
    ASSERT_EQ(block19->GetSum(), "");
    ASSERT_FLOAT_EQ(block19->GetAvg(), 0.0f);

    // second table colString, second block:
    auto data20 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                      ->GetBlocksList()
                      .at(1)
                      ->GetData();
    auto block20 = dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                       ->GetBlocksList()
                       .at(1);
    ASSERT_TRUE(dynamic_cast<ColumnBase<std::string>*>(secondTableColumns.at("colString").get())
                    ->GetBlocksList()
                    .at(1)
                    ->GetIsNullable());
    ASSERT_EQ(data20[0], "FastestDBinTheWorld");
    ASSERT_EQ(data20[1], "Speed is my second name");

    ASSERT_EQ(block20->GetMin(), "FastestDBinTheWorld");
    ASSERT_EQ(block20->GetMax(), "Speed is my second name");
    ASSERT_EQ(block20->GetSum(), "");
    ASSERT_FLOAT_EQ(block20->GetAvg(), 0.0f);
}