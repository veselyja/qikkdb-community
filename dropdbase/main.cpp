﻿/// \mainpage Project summary
/// SQL-like database application with query executing on GPU.
/// <br />
/// <b>Used programming language:</b>
///   - C++
///
/// <b>Used technologies:</b>
///   - CUDA
///   - Antlr
///   - Google Protocol Buffers

#include "CSVDataImporter.h"
#include <cstdio>
#include <iostream>
#include <chrono>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include "QueryEngine/Context.h"
#include "GpuSqlParser/GpuSqlCustomParser.h"
#include "DatabaseGenerator.h"
#include "Configuration.h"
#include "TCPServer.h"
#include "ClientPoolWorker.h"
#include "TCPClientHandler.h"
#include "ConsoleHandler.h"
#include "QueryEngine/GPUMemoryCache.h"
#include "Version.h"

/// Startup function, called automatically.
/// <param name="argc">program argument count</param>
/// <param name="argv">program arguments (for CSV importing): [csv-path [new-db-name]],
/// 		for taxi rides import use switch -t</param>
/// <returns>Exit code (0 - OK)</returns>
int main(int argc, char** argv)
{
    // Logger setup
    boost::log::add_common_attributes();
    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>(
        "Severity");
    std::ifstream logConfigFile("../configuration/log_config");
    if (logConfigFile.fail())
    {
        logConfigFile = std::ifstream("../configuration/log_config.default");
    }
    if (logConfigFile.fail())
    {
        BOOST_LOG_TRIVIAL(error)
            << "ERROR: Failed to load log configuration in \"../configuration/log_config.default\"";
    }
    else
    {
        boost::log::init_from_stream(logConfigFile);
    }

    // std::string dbName = "TestDatabase";
    // std::vector<std::string> tableNames = { "TestTable1" };
    // std::vector<DataType> columnTypes = { {COLUMN_INT},
    //	 {COLUMN_INT},
    //	 {COLUMN_LONG},
    //	 {COLUMN_LONG},
    //	 {COLUMN_LONG},
    //	 {COLUMN_FLOAT},
    //	 {COLUMN_FLOAT},
    //	 {COLUMN_DOUBLE},
    //	 {COLUMN_DOUBLE},
    //	 {COLUMN_POLYGON},
    //	 {COLUMN_POINT} };
    // std::shared_ptr<Database> compressionDatabase =
    // DatabaseGenerator::GenerateDatabase(dbName.c_str(), 2, 1<<18, false, tableNames,
    // columnTypes); Database::AddToInMemoryDatabaseList(compressionDatabase);
    // Database::SaveAllToDisk();
    // return 0;

    BOOST_LOG_TRIVIAL(info) << "TellStoryDB " << CMAKE_BUILD_TYPE << " " << GIT_VERSION << GIT_BRANCH;
    Context::getInstance(); // Initialize CUDA context

    if (argc > 1) // Importing CSV
    {
        if (strcmp(argv[1], "--t1") == 0)
        {
            BOOST_LOG_TRIVIAL(info) << "Importing Taxi Rides database...";

            CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
            const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG, COLUMN_INT,
                                              COLUMN_DOUBLE, COLUMN_DOUBLE, COLUMN_INT};
            const std::string tableName = "trips";
            csvDataImporter3.SetTypes(types);
            csvDataImporter3.SetTableName(tableName);
            std::shared_ptr<Database> database3 = std::make_shared<Database>("TaxiRides", 1201031007);
            Database::AddToInMemoryDatabaseList(database3);
            BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part1.csv ...";
            csvDataImporter3.ImportTables(database3);

            CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
            csvDataImporter4.SetTypes(types);
            csvDataImporter4.SetTableName(tableName);
            BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
            csvDataImporter4.ImportTables(database3);
        }
        else
        {
            if (strcmp(argv[1], "--a1") == 0)
            {
                BOOST_LOG_TRIVIAL(info)
                    << "Importing all databases has started (4 databases will be loaded) ...";

                CSVDataImporter csvDataImporter1(R"(../../data/GeoPoint.csv)");
                std::shared_ptr<Database> database1 = std::make_shared<Database>("GeoTest", 1000000);
                Database::AddToInMemoryDatabaseList(database1);
                BOOST_LOG_TRIVIAL(info) << "Loading GeoPoint.csv ...";
                csvDataImporter1.ImportTables(database1);

                CSVDataImporter csvDataImporter2(R"(../../data/TargetLoc1B.csv)");
                std::shared_ptr<Database> database2 = std::make_shared<Database>("TargetLocator", 1000000000);
                Database::AddToInMemoryDatabaseList(database2);
                BOOST_LOG_TRIVIAL(info) << "Loading TargetLoc1B.csv ...";
                csvDataImporter2.ImportTables(database2);

                CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG,
                                                  COLUMN_INT,    COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                  COLUMN_INT};
                const std::string tableName2 = "trips";
                csvDataImporter3.SetTypes(types);
                csvDataImporter3.SetTableName(tableName2);
                std::shared_ptr<Database> database3 = std::make_shared<Database>("TaxiRides", 1201031007);
                Database::AddToInMemoryDatabaseList(database3);
                BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part1.csv ...";
                csvDataImporter3.ImportTables(database3);

                CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                csvDataImporter4.SetTypes(types);
                csvDataImporter4.SetTableName(tableName2);
                BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
                csvDataImporter4.ImportTables(database3);

                CSVDataImporter csvDataImporter5(R"(../../data/Target.csv)");
                std::shared_ptr<Database> database4 = std::make_shared<Database>("stcs", 53305175);
                Database::AddToInMemoryDatabaseList(database4);
                BOOST_LOG_TRIVIAL(info) << "Loading Target.csv ...";
                csvDataImporter5.ImportTables(database4);

                CSVDataImporter csvDataImporter6(R"(../../data/TargetTraffic.csv)");
                BOOST_LOG_TRIVIAL(info) << "Loading TargetTraffic.csv ...";
                csvDataImporter6.ImportTables(database4);

                CSVDataImporter csvDataImporter7(R"(../../data/D_Cell.csv)");
                BOOST_LOG_TRIVIAL(info) << "Loading D_Cell.csv ...";
                csvDataImporter7.ImportTables(database4);
            }
            else
            {
                if (strcmp(argv[1], "--s1") == 0)
                {
                    BOOST_LOG_TRIVIAL(info) << "Importing small subset (338M rows, 7 columns, 1 "
                                               "table) from Taxi Rides database...";

                    CSVDataImporter csvDataImporter(R"(../../data/preprocessed_trips_part2.csv)");
                    const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG,
                                                      COLUMN_INT,    COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                      COLUMN_INT};
                    const std::string tableName = "trips";
                    csvDataImporter.SetTypes(types);
                    csvDataImporter.SetTableName(tableName);
                    std::shared_ptr<Database> database = std::make_shared<Database>("TaxiRides", 338294143);
                    Database::AddToInMemoryDatabaseList(database);
                    BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
                    csvDataImporter.ImportTables(database);
                }
                else
                {
                    if (strcmp(argv[1], "--t2") == 0)
                    {
                        BOOST_LOG_TRIVIAL(info) << "Importing Taxi Rides database...";

                        CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                        const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,
                                                          COLUMN_LONG,   COLUMN_INT,
                                                          COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                          COLUMN_INT};
                        const std::string tableName = "trips";
                        csvDataImporter3.SetTypes(types);
                        csvDataImporter3.SetTableName(tableName);
                        std::shared_ptr<Database> database3 =
                            std::make_shared<Database>("TaxiRides", 600515504);
                        Database::AddToInMemoryDatabaseList(database3);
                        BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part1.csv ...";
                        csvDataImporter3.ImportTables(database3);

                        CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                        csvDataImporter4.SetTypes(types);
                        csvDataImporter4.SetTableName(tableName);
                        BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
                        csvDataImporter4.ImportTables(database3);
                    }
                    else
                    {
                        if (strcmp(argv[1], "--a2") == 0)
                        {
                            BOOST_LOG_TRIVIAL(info) << "Importing all databases has started (4 "
                                                       "databases will be loaded) ...";

                            CSVDataImporter csvDataImporter1(R"(../../data/GeoPoint.csv)");
                            std::shared_ptr<Database> database1 =
                                std::make_shared<Database>("GeoTest", 500000);
                            Database::AddToInMemoryDatabaseList(database1);
                            BOOST_LOG_TRIVIAL(info) << "Loading GeoPoint.csv ...";
                            csvDataImporter1.ImportTables(database1);

                            CSVDataImporter csvDataImporter2(R"(../../data/TargetLoc1B.csv)");
                            std::shared_ptr<Database> database2 =
                                std::make_shared<Database>("TargetLocator", 500000000);
                            Database::AddToInMemoryDatabaseList(database2);
                            BOOST_LOG_TRIVIAL(info) << "Loading TargetLoc1B.csv ...";
                            csvDataImporter2.ImportTables(database2);

                            CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                            const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,
                                                              COLUMN_LONG,   COLUMN_INT,
                                                              COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                              COLUMN_INT};
                            const std::string tableName2 = "trips";
                            csvDataImporter3.SetTypes(types);
                            csvDataImporter3.SetTableName(tableName2);
                            std::shared_ptr<Database> database3 =
                                std::make_shared<Database>("TaxiRides", 600515504);
                            Database::AddToInMemoryDatabaseList(database3);
                            BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part1.csv ...";
                            csvDataImporter3.ImportTables(database3);

                            CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                            csvDataImporter4.SetTypes(types);
                            csvDataImporter4.SetTableName(tableName2);
                            BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
                            csvDataImporter4.ImportTables(database3);

                            CSVDataImporter csvDataImporter5(R"(../../data/Target.csv)");
                            std::shared_ptr<Database> database4 = std::make_shared<Database>("stcs", 26652588);
                            Database::AddToInMemoryDatabaseList(database4);
                            BOOST_LOG_TRIVIAL(info) << "Loading Target.csv ...";
                            csvDataImporter5.ImportTables(database4);

                            CSVDataImporter csvDataImporter6(R"(../../data/TargetTraffic.csv)");
                            BOOST_LOG_TRIVIAL(info) << "Loading TargetTraffic.csv ...";
                            csvDataImporter6.ImportTables(database4);

                            CSVDataImporter csvDataImporter7(R"(../../data/D_Cell.csv)");
                            BOOST_LOG_TRIVIAL(info) << "Loading D_Cell.csv ...";
                            csvDataImporter7.ImportTables(database4);
                        }
                        else
                        {
                            if (strcmp(argv[1], "--s2") == 0)
                            {
                                BOOST_LOG_TRIVIAL(info)
                                    << "Importing small subset (338M rows, 7 columns, 1 "
                                       "table) from Taxi Rides database...";

                                CSVDataImporter csvDataImporter(R"(../../data/preprocessed_trips_part2.csv)");
                                const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,
                                                                  COLUMN_LONG,   COLUMN_INT,
                                                                  COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                                  COLUMN_INT};
                                const std::string tableName = "trips";
                                csvDataImporter.SetTypes(types);
                                csvDataImporter.SetTableName(tableName);
                                std::shared_ptr<Database> database =
                                    std::make_shared<Database>("TaxiRides", 169147072);
                                Database::AddToInMemoryDatabaseList(database);
                                BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
                                csvDataImporter.ImportTables(database);
                            }
                            else
                            {
                                if (strcmp(argv[1], "--t4") == 0)
                                {
                                    BOOST_LOG_TRIVIAL(info) << "Importing Taxi Rides database...";

                                    CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                                    const std::vector<DataType> types{COLUMN_STRING, COLUMN_LONG,
                                                                      COLUMN_LONG,   COLUMN_INT,
                                                                      COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                                      COLUMN_INT};
                                    const std::string tableName = "trips";
                                    csvDataImporter3.SetTypes(types);
                                    csvDataImporter3.SetTableName(tableName);
                                    std::shared_ptr<Database> database3 =
                                        std::make_shared<Database>("TaxiRides", 300257752);
                                    Database::AddToInMemoryDatabaseList(database3);
                                    BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part1.csv ...";
                                    csvDataImporter3.ImportTables(database3);

                                    CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                                    csvDataImporter4.SetTypes(types);
                                    csvDataImporter4.SetTableName(tableName);
                                    BOOST_LOG_TRIVIAL(info) << "Loading preprocessed_trips_part2.csv ...";
                                    csvDataImporter4.ImportTables(database3);
                                }
                                else
                                {
                                    if (strcmp(argv[1], "--a4") == 0)
                                    {
                                        BOOST_LOG_TRIVIAL(info)
                                            << "Importing all databases has started (4 "
                                               "databases will be loaded) ...";

                                        CSVDataImporter csvDataImporter1(R"(../../data/GeoPoint.csv)");
                                        std::shared_ptr<Database> database1 =
                                            std::make_shared<Database>("GeoTest", 125000);
                                        Database::AddToInMemoryDatabaseList(database1);
                                        BOOST_LOG_TRIVIAL(info) << "Loading GeoPoint.csv ...";
                                        csvDataImporter1.ImportTables(database1);

                                        CSVDataImporter csvDataImporter2(R"(../../data/TargetLoc1B.csv)");
                                        std::shared_ptr<Database> database2 =
                                            std::make_shared<Database>("TargetLocator", 125000000);
                                        Database::AddToInMemoryDatabaseList(database2);
                                        BOOST_LOG_TRIVIAL(info) << "Loading TargetLoc1B.csv ...";
                                        csvDataImporter2.ImportTables(database2);

                                        CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                                        const std::vector<DataType> types{
                                            COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG, COLUMN_INT,
                                            COLUMN_DOUBLE, COLUMN_DOUBLE, COLUMN_INT};
                                        const std::string tableName2 = "trips";
                                        csvDataImporter3.SetTypes(types);
                                        csvDataImporter3.SetTableName(tableName2);
                                        std::shared_ptr<Database> database3 =
                                            std::make_shared<Database>("TaxiRides", 300257752);
                                        Database::AddToInMemoryDatabaseList(database3);
                                        BOOST_LOG_TRIVIAL(info)
                                            << "Loading preprocessed_trips_part1.csv ...";
                                        csvDataImporter3.ImportTables(database3);

                                        CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                                        csvDataImporter4.SetTypes(types);
                                        csvDataImporter4.SetTableName(tableName2);
                                        BOOST_LOG_TRIVIAL(info)
                                            << "Loading preprocessed_trips_part2.csv ...";
                                        csvDataImporter4.ImportTables(database3);

                                        CSVDataImporter csvDataImporter5(R"(../../data/Target.csv)");
                                        std::shared_ptr<Database> database4 =
                                            std::make_shared<Database>("stcs", 13326294);
                                        Database::AddToInMemoryDatabaseList(database4);
                                        BOOST_LOG_TRIVIAL(info) << "Loading Target.csv ...";
                                        csvDataImporter5.ImportTables(database4);

                                        CSVDataImporter csvDataImporter6(R"(../../data/TargetTraffic.csv)");
                                        BOOST_LOG_TRIVIAL(info) << "Loading TargetTraffic.csv ...";
                                        csvDataImporter6.ImportTables(database4);

                                        CSVDataImporter csvDataImporter7(R"(../../data/D_Cell.csv)");
                                        BOOST_LOG_TRIVIAL(info) << "Loading D_Cell.csv ...";
                                        csvDataImporter7.ImportTables(database4);
                                    }
                                    else
                                    {
                                        if (strcmp(argv[1], "--s4") == 0)
                                        {
                                            BOOST_LOG_TRIVIAL(info)
                                                << "Importing small subset (338M rows, 7 columns, "
                                                   "1 "
                                                   "table) from Taxi Rides database...";

                                            CSVDataImporter csvDataImporter(R"(../../data/preprocessed_trips_part2.csv)");
                                            const std::vector<DataType> types{
                                                COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG,
                                                COLUMN_INT,    COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                COLUMN_INT};
                                            const std::string tableName = "trips";
                                            csvDataImporter.SetTypes(types);
                                            csvDataImporter.SetTableName(tableName);
                                            std::shared_ptr<Database> database =
                                                std::make_shared<Database>("TaxiRides", 84573536);
                                            Database::AddToInMemoryDatabaseList(database);
                                            BOOST_LOG_TRIVIAL(info)
                                                << "Loading preprocessed_trips_part2.csv ...";
                                            csvDataImporter.ImportTables(database);
                                        }
                                        else
                                        {
                                            if (strcmp(argv[1], "--t8") == 0)
                                            {
                                                BOOST_LOG_TRIVIAL(info)
                                                    << "Importing Taxi Rides database...";

                                                CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                                                const std::vector<DataType> types{
                                                    COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG,
                                                    COLUMN_INT,    COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                    COLUMN_INT};
                                                const std::string tableName = "trips";
                                                csvDataImporter3.SetTypes(types);
                                                csvDataImporter3.SetTableName(tableName);
                                                std::shared_ptr<Database> database3 =
                                                    std::make_shared<Database>("TaxiRides", 150128876);
                                                Database::AddToInMemoryDatabaseList(database3);
                                                BOOST_LOG_TRIVIAL(info)
                                                    << "Loading preprocessed_trips_part1.csv ...";
                                                csvDataImporter3.ImportTables(database3);

                                                CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                                                csvDataImporter4.SetTypes(types);
                                                csvDataImporter4.SetTableName(tableName);
                                                BOOST_LOG_TRIVIAL(info)
                                                    << "Loading preprocessed_trips_part2.csv ...";
                                                csvDataImporter4.ImportTables(database3);
                                            }
                                            else
                                            {
                                                if (strcmp(argv[1], "--a8") == 0)
                                                {
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Importing all databases has started (4 "
                                                           "databases will be loaded) ...";

                                                    CSVDataImporter csvDataImporter1(R"(../../data/GeoPoint.csv)");
                                                    std::shared_ptr<Database> database1 =
                                                        std::make_shared<Database>("GeoTest", 62500);
                                                    Database::AddToInMemoryDatabaseList(database1);
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading GeoPoint.csv ...";
                                                    csvDataImporter1.ImportTables(database1);

                                                    CSVDataImporter csvDataImporter2(R"(../../data/TargetLoc1B.csv)");
                                                    std::shared_ptr<Database> database2 =
                                                        std::make_shared<Database>("TargetLocator", 62500000);
                                                    Database::AddToInMemoryDatabaseList(database2);
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading TargetLoc1B.csv ...";
                                                    csvDataImporter2.ImportTables(database2);

                                                    CSVDataImporter csvDataImporter3(R"(../../data/preprocessed_trips_part1.csv)");
                                                    const std::vector<DataType> types{
                                                        COLUMN_STRING, COLUMN_LONG,   COLUMN_LONG,
                                                        COLUMN_INT,    COLUMN_DOUBLE, COLUMN_DOUBLE,
                                                        COLUMN_INT};
                                                    const std::string tableName2 = "trips";
                                                    csvDataImporter3.SetTypes(types);
                                                    csvDataImporter3.SetTableName(tableName2);
                                                    std::shared_ptr<Database> database3 =
                                                        std::make_shared<Database>("TaxiRides", 150128876);
                                                    Database::AddToInMemoryDatabaseList(database3);
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading preprocessed_trips_part1.csv ...";
                                                    csvDataImporter3.ImportTables(database3);

                                                    CSVDataImporter csvDataImporter4(R"(../../data/preprocessed_trips_part2.csv)");
                                                    csvDataImporter4.SetTypes(types);
                                                    csvDataImporter4.SetTableName(tableName2);
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading preprocessed_trips_part2.csv ...";
                                                    csvDataImporter4.ImportTables(database3);

                                                    CSVDataImporter csvDataImporter5(R"(../../data/Target.csv)");
                                                    std::shared_ptr<Database> database4 =
                                                        std::make_shared<Database>("stcs", 6663147);
                                                    Database::AddToInMemoryDatabaseList(database4);
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading Target.csv ...";
                                                    csvDataImporter5.ImportTables(database4);

                                                    CSVDataImporter csvDataImporter6(R"(../../data/TargetTraffic.csv)");
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading TargetTraffic.csv ...";
                                                    csvDataImporter6.ImportTables(database4);

                                                    CSVDataImporter csvDataImporter7(R"(../../data/D_Cell.csv)");
                                                    BOOST_LOG_TRIVIAL(info)
                                                        << "Loading D_Cell.csv ...";
                                                    csvDataImporter7.ImportTables(database4);
                                                }
                                                else
                                                {
                                                    if (strcmp(argv[1], "--s8") == 0)
                                                    {
                                                        BOOST_LOG_TRIVIAL(info)
                                                            << "Importing small subset (338M rows, "
                                                               "7 columns, "
                                                               "1 "
                                                               "table) from Taxi Rides database...";

                                                        CSVDataImporter csvDataImporter(
                                                            R"(../../data/preprocessed_trips_part2.csv)");
                                                        const std::vector<DataType> types{COLUMN_STRING,
                                                                                          COLUMN_LONG,
                                                                                          COLUMN_LONG,
                                                                                          COLUMN_INT,
                                                                                          COLUMN_DOUBLE,
                                                                                          COLUMN_DOUBLE,
                                                                                          COLUMN_INT};
                                                        const std::string tableName = "trips";
                                                        csvDataImporter.SetTypes(types);
                                                        csvDataImporter.SetTableName(tableName);
                                                        std::shared_ptr<Database> database =
                                                            std::make_shared<Database>("TaxiRides", 42286768);
                                                        Database::AddToInMemoryDatabaseList(database);
                                                        BOOST_LOG_TRIVIAL(info)
                                                            << "Loading preprocessed_trips_part2.csv ...";
                                                        csvDataImporter.ImportTables(database);
                                                    }
                                                    else
                                                    {
                                                        if (strcmp(argv[1], "--stcs2") == 0)
                                                        {
                                                            CSVDataImporter csvDataImporter5(R"(../../data/Target.csv)");
                                                            std::shared_ptr<Database> database4 =
                                                                std::make_shared<Database>("stcs", 26652588);
                                                            Database::AddToInMemoryDatabaseList(database4);
                                                            BOOST_LOG_TRIVIAL(info)
                                                                << "Loading Target.csv ...";
                                                            csvDataImporter5.ImportTables(database4);

                                                            CSVDataImporter csvDataImporter6(
                                                                R"(../../data/TargetTraffic.csv)");
                                                            BOOST_LOG_TRIVIAL(info)
                                                                << "Loading TargetTraffic.csv ...";
                                                            csvDataImporter6.ImportTables(database4);

                                                            CSVDataImporter csvDataImporter7(R"(../../data/D_Cell.csv)");
                                                            BOOST_LOG_TRIVIAL(info)
                                                                << "Loading D_Cell.csv ...";
                                                            csvDataImporter7.ImportTables(database4);
                                                        }
                                                        else
                                                        {
                                                            if (strcmp(argv[1], "--test") == 0)
                                                            {
                                                                CSVDataImporter csvDataImporter1(R"(../../data/AllTableA.csv)");
                                                                std::shared_ptr<Database> database =
                                                                    std::make_shared<Database>(
                                                                        "AllDataTypes", 2);
                                                                Database::AddToInMemoryDatabaseList(database);
                                                                BOOST_LOG_TRIVIAL(info)
                                                                    << "Loading AllTableA.csv ...";
                                                                csvDataImporter1.ImportTables(database);

                                                                CSVDataImporter csvDataImporter2(R"(../../data/AllTableB.csv)");
                                                                BOOST_LOG_TRIVIAL(info)
                                                                    << "Loading AllTableB.csv ...";
                                                                csvDataImporter2.ImportTables(database);

																CSVDataImporter csvDataImporter3(R"(../../data/AllTableC.csv)");
                                                                BOOST_LOG_TRIVIAL(info)
                                                                    << "Loading AllTableC.csv ...";
                                                                csvDataImporter3.ImportTables(database);

																CSVDataImporter csvDataImporter4(R"(../../data/AllTableD.csv)");
                                                                BOOST_LOG_TRIVIAL(info)
                                                                    << "Loading AllTableD.csv ...";
                                                                csvDataImporter4.ImportTables(database);

																CSVDataImporter csvDataImporter5(R"(../../data/AllTableE.csv)");
                                                                BOOST_LOG_TRIVIAL(info)
                                                                    << "Loading AllTableE.csv ...";
                                                                csvDataImporter5.ImportTables(database);
                                                            }
                                                            else
                                                            {
                                                                // Import CSV file if entered as program argument
                                                                CSVDataImporter csvDataImporter(argv[1]);
                                                                std::shared_ptr<Database> database =
                                                                    std::make_shared<Database>(
                                                                        argc > 2 ? argv[2] : "TestDb",
                                                                        argc > 3 ? std::stoll(argv[3]) : 1048576);
                                                                Database::AddToInMemoryDatabaseList(database);
                                                                BOOST_LOG_TRIVIAL(info)
                                                                    << "Loading CSV from \""
                                                                    << argv[1] << "\"";
                                                                csvDataImporter.ImportTables(database);
                                                            }
														} 
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else // TCP server
    {
        BOOST_LOG_TRIVIAL(info) << "Loading databases...";
        Database::LoadDatabasesFromDisk();
        BOOST_LOG_TRIVIAL(info) << "All databases loaded.";

        TCPServer<TCPClientHandler, ClientPoolWorker> tcpServer(
            Configuration::GetInstance().GetListenIP().c_str(), Configuration::GetInstance().GetListenPort());
        RegisterCtrlCHandler(&tcpServer);
        tcpServer.Run();
    }

    Database::SaveModifiedToDisk();
    BOOST_LOG_TRIVIAL(info) << "TellStoryDB exiting cleanly...";

    for (auto& db : Database::GetDatabaseNames())
    {
        Database::RemoveFromInMemoryDatabaseList(db.c_str());
    }

    BOOST_LOG_TRIVIAL(info) << "TellStoryDB exited.";
    boost::log::core::get()->remove_all_sinks();
    return 0;
}
