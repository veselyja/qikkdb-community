#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <stdio.h>

#include "ColumnBase.h"
#include "Configuration.h"
#include "Database.h"
#include "Table.h"
#include "Types/ComplexPolygon.pb.h"
#include "QueryEngine/Context.h"

std::mutex Database::dbMutex_;
std::mutex Database::dbAccessMutex_;

/// <summary>
/// Initializes a new instance of the <see cref="T:ColmnarDB.Database"/> class.
/// </summary>
/// <param name="databaseName">Database name.</param>
/// <param name="blockSize">Block size of all blocks in this database.</param>
Database::Database(const char* databaseName, int32_t blockSize)
{
    name_ = databaseName;
    blockSize_ = blockSize;
}

Database::~Database()
{
    Context& context = Context::getInstance();
    int32_t oldDeviceID = context.getBoundDeviceID();
    // Clear cache for all devices
    for (int32_t deviceID = 0; deviceID < Context::getInstance().getDeviceCount(); deviceID++)
    {
        context.bindDeviceToContext(deviceID);
        GPUMemoryCache& cacheForDevice = Context::getInstance().getCacheForDevice(deviceID);
        for (auto const& table : tables_)
        {
            for (auto const& column : table.second.GetColumns())
            {
                int32_t blockCount = column.second.get()->GetBlockCount();
                for (int32_t i = 0; i < blockCount; i++)
                {
                    cacheForDevice.clearCachedBlock(
                        name_, table.second.GetName() + "." + column.second.get()->GetName(), i);
                    if (column.second.get()->GetIsNullable())
                    {
                        cacheForDevice.clearCachedBlock(name_,
                                                        table.second.GetName() + "." +
                                                            column.second.get()->GetName() + "_nullMask",
                                                        i);
                    }
                }
            }
        }
    }
    context.bindDeviceToContext(oldDeviceID);
}

std::vector<std::string> Database::GetDatabaseNames()
{
    std::vector<std::string> ret;
    for (auto& entry : Context::getInstance().GetLoadedDatabases())
    {
        ret.push_back(entry.first);
    }
    return ret;
}

/// <summary>
/// Set saveNecessaty_ to false for block, column and table, because data in the database were NOT modified yet.
/// </summary>
void Database::SetSaveNecessaryToFalseForEverything()
{
    auto& tables = GetTables();

    for (auto& table : tables)
    {
        table.second.SetSaveNecessaryToFalse();

        auto& columns = table.second.GetColumns();

        for (const auto& column : columns)
        {
            auto type = column.second.get()->GetColumnType();

            switch (type)
            {
            case COLUMN_POLYGON:
            {
                auto castedColumn =
                    dynamic_cast<ColumnBase<ColmnarDB::Types::ComplexPolygon>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_POINT:
            {
                auto castedColumn = dynamic_cast<ColumnBase<ColmnarDB::Types::Point>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_STRING:
            {
                auto castedColumn = dynamic_cast<ColumnBase<std::string>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_INT8_T:
            {
                auto castedColumn = dynamic_cast<ColumnBase<int8_t>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_INT:
            {
                auto castedColumn = dynamic_cast<ColumnBase<int32_t>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_LONG:
            {
                auto castedColumn = dynamic_cast<ColumnBase<int64_t>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_FLOAT:
            {
                auto castedColumn = dynamic_cast<ColumnBase<float>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;

            case COLUMN_DOUBLE:
            {
                auto castedColumn = dynamic_cast<ColumnBase<double>*>(column.second.get());
                castedColumn->SetSaveNecessaryToFalse();

                auto& blocks = castedColumn->GetBlocksList();

                for (int32_t i = 0; i < blocks.size(); i++)
                {
                    blocks[i]->SetSaveNecessaryToFalse();
                }
            }
            break;
            }
        }
    }
}

/// <summary>
/// Save only .db file to disk.
/// </summary>
/// <param name="path">Path to database storage directory.</param>
void Database::PersistOnlyDbFile(const char* path)
{
    auto& tables = GetTables();
    auto& name = GetName();
    auto pathStr = std::string(path);

    boost::filesystem::create_directories(path);

    int32_t blockSize = GetBlockSize();
    int32_t tableSize = tables.size();

    // write file .db
    BOOST_LOG_TRIVIAL(debug) << "Saving .db file with name: " << pathStr << name << ".db";
    std::ofstream dbFile(pathStr + "/" + name + ".db", std::ios::binary);

    int32_t dbNameLength = name.length() + 1; // +1 because '\0'

    dbFile.write(reinterpret_cast<const char*>(&PERSISTENCE_FORMAT_VERSION),
                 sizeof(int32_t)); // write persistence format version
    dbFile.write(reinterpret_cast<char*>(&dbNameLength), sizeof(int32_t)); // write db name length
    dbFile.write(name.c_str(), dbNameLength); // write db name
    dbFile.write(reinterpret_cast<char*>(&blockSize), sizeof(int32_t)); // write block size
    dbFile.write(reinterpret_cast<char*>(&tableSize), sizeof(int32_t)); // write number of tables
    for (auto& table : tables)
    {
        auto& columns = table.second.GetColumns();
        const auto& sortingColumns = table.second.GetSortingColumns();
        int32_t tableNameLength = table.first.length() + 1; // +1 because '\0'
        int32_t columnNumber = columns.size();
        int32_t sortingColumnNumber = sortingColumns.size();

        dbFile.write(reinterpret_cast<char*>(&tableNameLength), sizeof(int32_t)); // write table name length
        dbFile.write(table.first.c_str(), tableNameLength); // write table name
        dbFile.write(reinterpret_cast<char*>(&columnNumber), sizeof(int32_t)); // write number of columns of the table
        dbFile.write(reinterpret_cast<char*>(&sortingColumnNumber),
                     sizeof(int32_t)); // write number of sorting columns of the table

        if (sortingColumnNumber > 0)
        {
            for (const std::string sortingColumn : sortingColumns)
            {
                int32_t sortingColumnLength = sortingColumn.length() + 1; // +1 because '\0'

                dbFile.write(reinterpret_cast<char*>(&sortingColumnLength),
                             sizeof(int32_t)); // write sorting column name length
                dbFile.write(sortingColumn.c_str(), sortingColumnLength); // write sorting column name
                BOOST_LOG_TRIVIAL(debug) << "Sorting column (table: " + std::string(table.first.c_str()) +
                                                ") saved: " + std::string(sortingColumn.c_str()) + ".";
            }
        }

        for (const auto& column : columns)
        {
            int32_t columnNameLength = column.first.length() + 1; // +1 because '\0'

            dbFile.write(reinterpret_cast<char*>(&columnNameLength), sizeof(int32_t)); // write column name length
            dbFile.write(column.first.c_str(), columnNameLength); // write column name
        }
    }
    dbFile.close();
}

/// <summary>
/// Save database from memory to disk.
/// </summary>
/// <param name="path">Path to database storage directory.</param>
void Database::Persist(const char* path)
{
    auto& tables = GetTables();
    auto& name = GetName();
    auto pathStr = std::string(path);

    BOOST_LOG_TRIVIAL(info) << "Saving database with name: " << name << " and " << tables.size() << " tables.";

    int32_t blockSize = GetBlockSize();
    int32_t tableSize = tables.size();

    PersistOnlyDbFile(path);

    // write files .col:
    for (auto& table : tables)
    {
        auto& columns = table.second.GetColumns();

        std::vector<std::thread> threads;

        for (const auto& column : columns)
        {
            threads.emplace_back(Database::WriteColumn, std::ref(column), pathStr, name, std::ref(table));
            column.second.get()->SetSaveNecessaryToFalse();
        }

        for (int j = 0; j < columns.size(); j++)
        {
            threads[j].join();
        }

        table.second.SetSaveNecessaryToFalse();
    }

    if (boost::filesystem::exists(boost::filesystem::path(path + name + ".db")))
    {
        BOOST_LOG_TRIVIAL(info) << "Database " << name << " was successfully saved into disk.";
    }
    else
    {
        BOOST_LOG_TRIVIAL(info)
            << "Database "
            << name << " was NOT saved into disk. Check if you have write access into the destination folder.";
    }
}

/// <summary>
/// Save modified blocks and columns of the database from memory to disk.
/// </summary>
/// <param name="path">Path to database storage directory.</param>
void Database::PersistOnlyModified(const char* path)
{
    auto& tables = GetTables();
    auto& name = GetName();
    auto pathStr = std::string(path);

    BOOST_LOG_TRIVIAL(info) << "Saving database with name: " << name << " and " << tables.size() << " table/s.";

    int32_t blockSize = GetBlockSize();
    int32_t tableSize = tables.size();

    PersistOnlyDbFile(path);

    // write files .col:
    for (auto& table : tables)
    {
        if (table.second.GetSaveNecessary())
        {
            const auto& columns = table.second.GetColumns();

            std::vector<std::thread> threads;

            for (auto& column : columns)
            {
                if (column.second.get()->GetSaveNecessary())
                {
                    threads.emplace_back(Database::WriteColumn, std::ref(column), pathStr, name,
                                         std::ref(table));
                    column.second.get()->SetSaveNecessaryToFalse();
                }
            }

            for (int j = 0; j < threads.size(); j++)
            {
                threads[j].join();
            }

            table.second.SetSaveNecessaryToFalse();
        }
    }

    if (boost::filesystem::exists(boost::filesystem::path(path + name + ".db")))
    {
        BOOST_LOG_TRIVIAL(info) << "Database " << name << " was successfully saved into disk.";
    }
    else
    {
        BOOST_LOG_TRIVIAL(info)
            << "Database "
            << name << " was NOT saved into disk. Check if you have write access into the destination folder.";
    }
}

/// <summary>
/// Save all databases currently in memory to disk. All databases will be saved in the same directory
/// </summary>
void Database::SaveAllToDisk()
{
    std::unique_lock<std::mutex>(dbMutex_);
    BOOST_LOG_TRIVIAL(info) << "Saving all loaded databases to disk has started...";
    auto path = Configuration::GetInstance().GetDatabaseDir().c_str();
    for (auto& database : Context::getInstance().GetLoadedDatabases())
    {
        database.second->Persist(path);
    }
    BOOST_LOG_TRIVIAL(info) << "Saving loaded databases to disk has finished.";
}

/// <summary>
/// Save only modified blocks and columns to disk. All databases will be saved in the same directory.
/// </summary>
void Database::SaveModifiedToDisk()
{
    std::unique_lock<std::mutex>(dbMutex_);
    BOOST_LOG_TRIVIAL(info)
        << "Saving only modified blocks and columns of the loaded databases to disk has started...";
    auto path = Configuration::GetInstance().GetDatabaseDir().c_str();
    for (auto& database : Context::getInstance().GetLoadedDatabases())
    {
        database.second->PersistOnlyModified(path);
    }
    BOOST_LOG_TRIVIAL(info)
        << "Saving only modified blocks and columns of the loaded databases to disk has finished.";
}

/// <summary>
/// Load databases from disk storage. Databases .db and .col files have to be in the same directory,
/// so all databases have to be in the same dorectory to be loaded using this procedure.
/// </summary>
void Database::LoadDatabasesFromDisk()
{
    std::unique_lock<std::mutex>(dbMutex_);
    auto& path = Configuration::GetInstance().GetDatabaseDir();

    if (boost::filesystem::exists(path))
    {
        for (auto& p : boost::filesystem::directory_iterator(path))
        {
            auto extension = p.path().extension();
            if (extension == ".db")
            {
                auto database =
                    Database::LoadDatabase(p.path().filename().stem().generic_string().c_str(), path.c_str());

                if (database != nullptr)
                {
                    database->SetSaveNecessaryToFalseForEverything();
                    Context::getInstance().GetLoadedDatabases().insert({database->name_, database});
                }
            }
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Directory " << path << " does not exists.";
    }
}

/// <summary>
/// Delete database from disk. Deletes .db and .col files which belong to the specified database.
/// Database is not deleted from memory.
/// </summary>
void Database::DeleteDatabaseFromDisk()
{
    std::unique_lock<std::mutex>(dbMutex_);
    auto& path = Configuration::GetInstance().GetDatabaseDir();

    // std::cout << "DeleteDatabaseFromDisk path: " << path << std::endl;
    if (boost::filesystem::exists(path))
    {
        // std::cout << "DeleteDatabaseFromDisk prefix: " << prefix << std::endl;
        // Delete main .db file
        if (boost::filesystem::remove(path + name_ + ".db"))
        {
            BOOST_LOG_TRIVIAL(info) << "Main (.db) file of db " << name_ << " was successfully removed from disk.";
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Main (.db) file of db " << name_
                                    << " was NOT removed from disk. No such file or write access.";
        }

        // Delete tables and columns
        std::string prefix(path + name_ + SEPARATOR);
        for (auto& p : boost::filesystem::directory_iterator(path))
        {
            // std::cout << "DeleteDatabaseFromDisk p.path().string(): " << p.path().string() <<
            // std::endl; delete files which starts with prefix of db name:
            if (!p.path().string().compare(0, prefix.size(), prefix))
            {
                if (boost::filesystem::remove(p.path().string().c_str()))
                {
                    BOOST_LOG_TRIVIAL(info) << "File " << p.path().string() << " was successfully removed from disk.";
                }
                else
                {
                    BOOST_LOG_TRIVIAL(info)
                        << "File " << p.path().string()
                        << " was NOT removed from disk. No such file or write access.";
                }
            }
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Directory " << path << " does not exists.";
    }
}

/// <summary>
/// <param name="tableName">Name of the table to be deleted.</param>
/// Delete table from disk. Deletes .col files which belong to the specified table of currently loaded database.
/// To alter .db file, this action also calls a function PersistOnlyDbFile().
/// Table needs to be deleted from memory before calling this method, so that .db file can be updated correctly.
/// </summary>
void Database::DeleteTableFromDisk(const char* tableName)
{
    auto& path = Configuration::GetInstance().GetDatabaseDir();

    if (boost::filesystem::exists(path))
    {
        std::string prefix(path + name_ + SEPARATOR + std::string(tableName) + SEPARATOR);

        for (auto& p : boost::filesystem::directory_iterator(path))
        {
            // delete files which starts with prefix of db name and table name:
            if (!p.path().string().compare(0, prefix.size(), prefix))
            {
                if (boost::filesystem::remove(p.path().string().c_str()))
                {
                    BOOST_LOG_TRIVIAL(info) << "File " << p.path().string() << " from database "
                                            << name_ << " was successfully removed from disk.";
                }
                else
                {
                    BOOST_LOG_TRIVIAL(info)
                        << "File " << p.path().string()
                        << " was NOT removed from disk. No such file or write access.";
                }
            }
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Directory " << path << " does not exists.";
    }

    // persist only db file, so that changes are saved, BUT PERSIST ONLY if there already is a .db file, so it is not only in memory
    if (boost::filesystem::exists(path + name_ + ".db"))
    {
        PersistOnlyDbFile(Configuration::GetInstance().GetDatabaseDir().c_str());
    }
}

/// <summary>
/// <param name="tableName">Name of the table which have the specified column that will be deleted.</param>
/// <param name="columnName">Name of the column file (*.col) without the ".col" suffix that will be deleted.</param>
/// Delete column of a table. Deletes single .col file which belongs to specified column and specified table.
/// To alter .db file, this action also calls a function Persist.
/// Column needs to be deleted from memory before calling this method, so that .db file can be updated correctly.
/// </summary>
void Database::DeleteColumnFromDisk(const char* tableName, const char* columnName)
{
    auto& path = Configuration::GetInstance().GetDatabaseDir();

    std::string filePath =
        path + name_ + SEPARATOR + std::string(tableName) + SEPARATOR + std::string(columnName) + ".col";

    if (boost::filesystem::exists(filePath))
    {
        if (boost::filesystem::remove(filePath.c_str()))
        {
            BOOST_LOG_TRIVIAL(info) << "Column " << columnName << " from table " << tableName << " from database "
                                    << name_ << " was successfully removed from disk.";
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "File " << filePath
                                    << " was NOT removed from disk. No such file or write access.";
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "File " << path << " does not exists.";
    }

    // persist only db file, so that changes are saved, BUT PERSIST ONLY if there already is a .db file, so it is not only in memory
    if (boost::filesystem::exists(path + name_ + ".db"))
    {
        PersistOnlyDbFile(Configuration::GetInstance().GetDatabaseDir().c_str());
    }
}

/// <summary>
/// Load database from disk into memory.
/// </summary>
/// <param name="fileDbName">Name of the database file (*.db) without the ".db" suffix.</param>
/// <param name="path">Path to directory in which database files are.</param>
/// <returns>Shared pointer of database.</returns>
std::shared_ptr<Database> Database::LoadDatabase(const char* fileDbName, const char* path)
{
    BOOST_LOG_TRIVIAL(info) << "Loading database from: " << path << fileDbName << ".db.";

    // read file .db
    std::ifstream dbFile(path + std::string(fileDbName) + ".db", std::ios::binary);

    int32_t persistenceFormatVersion;
    dbFile.read(reinterpret_cast<char*>(&persistenceFormatVersion),
                sizeof(int32_t)); // read persistence format version

    int32_t dbNameLength;
    dbFile.read(reinterpret_cast<char*>(&dbNameLength), sizeof(int32_t)); // read db name length

    std::unique_ptr<char[]> dbName(new char[dbNameLength]);
    dbFile.read(dbName.get(), dbNameLength); // read db name

    int32_t blockSize;
    dbFile.read(reinterpret_cast<char*>(&blockSize), sizeof(int32_t)); // read block size

    int32_t tablesCount;
    dbFile.read(reinterpret_cast<char*>(&tablesCount), sizeof(int32_t)); // read number of tables

    std::shared_ptr<Database> database = std::make_shared<Database>(dbName.get(), blockSize);

    for (int32_t i = 0; i < tablesCount; i++)
    {
        int32_t tableNameLength;
        dbFile.read(reinterpret_cast<char*>(&tableNameLength), sizeof(int32_t)); // read table name length

        std::unique_ptr<char[]> tableName(new char[tableNameLength]);
        dbFile.read(tableName.get(), tableNameLength); // read table name
        database->tables_.emplace(
            std::make_pair(std::string(tableName.get()), Table(database, tableName.get())));
        int32_t columnCount;
        int32_t sortingColumnCount;
        dbFile.read(reinterpret_cast<char*>(&columnCount), sizeof(int32_t)); // read number of columns
        dbFile.read(reinterpret_cast<char*>(&sortingColumnCount), sizeof(int32_t)); // read number of sorting columns

        std::vector<std::string> columnNames;
        std::vector<std::string> sortingColumnNames;

        for (int32_t j = 0; j < sortingColumnCount; j++)
        {
            int32_t sortingColumnLength;
            dbFile.read(reinterpret_cast<char*>(&sortingColumnLength), sizeof(int32_t)); // read sorting column name length

            std::unique_ptr<char[]> sortingColumnName(new char[sortingColumnLength]);
            dbFile.read(sortingColumnName.get(), sortingColumnLength); // read sorting column name
            BOOST_LOG_TRIVIAL(debug) << "Sorting column (table: " + std::string(tableName.get()) +
                                            ") loaded: " + std::string(sortingColumnName.get()) + ".";
            sortingColumnNames.push_back(sortingColumnName.get());
        }

        for (int32_t j = 0; j < columnCount; j++)
        {
            int32_t columnNameLength;
            dbFile.read(reinterpret_cast<char*>(&columnNameLength), sizeof(int32_t)); // read column name length

            std::unique_ptr<char[]> columnName(new char[columnNameLength]);
            dbFile.read(columnName.get(), columnNameLength); // read column name

            columnNames.push_back(columnName.get());
        }

        auto& table = database->tables_.at(tableName.get());
        table.SetSortingColumns(sortingColumnNames);

        std::vector<std::thread> threads;

        for (const std::string& columnName : columnNames)
        {
            threads.emplace_back(Database::LoadColumn, path, dbName.get(), persistenceFormatVersion,
                                 std::ref(table), std::ref(columnName));
        }

        for (int i = 0; i < columnNames.size(); i++)
        {
            threads[i].join();
        }
    }

    dbFile.close();

    return database;
}

/// <summary>
/// Load column of a table into memory from disk.
/// </summary>
/// <param name="path">Path directory, where column file (*.col) is.</param>
/// <param name="dbName">Name of the database.</param>
/// <param name="persistenceFormatVersion">Version of format used to persist .db and .col files into
/// disk.</param> <param name="table">Instance of table into which the column should be
/// added.</param> <param name="columnName">Names of particular column.</param>
void Database::LoadColumn(const char* path,
                          const char* dbName,
                          const int32_t persistenceFormatVersion,
                          Table& table,
                          const std::string& columnName)
{
    // read files .col:
    std::string pathStr = std::string(path);

    BOOST_LOG_TRIVIAL(info) << "Loading .col file with name: " << pathStr + dbName << SEPARATOR
                            << table.GetName() << SEPARATOR << columnName << ".col.";

    std::ifstream colFile(pathStr + dbName + SEPARATOR + table.GetName() + SEPARATOR + columnName + ".col",
                          std::ios::binary);

    int32_t emptyBlockIndex = 0;

    int32_t type;
    bool isNullable;
    bool isUnique;

    colFile.read(reinterpret_cast<char*>(&type), sizeof(int32_t)); // read type of column
    colFile.read(reinterpret_cast<char*>(&isNullable), sizeof(bool)); // read nullability of column
    colFile.read(reinterpret_cast<char*>(&isUnique), sizeof(bool)); // read unicity of column

    int32_t nullBitMaskAllocationSize =
        ((table.GetBlockSize() + sizeof(int8_t) * 8 - 1) / (8 * sizeof(int8_t)));

    switch (type)
    {
    case COLUMN_POLYGON:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_POLYGON, isNullable, isUnique);

        auto& columnPolygon =
            dynamic_cast<ColumnBase<ColmnarDB::Types::ComplexPolygon>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int64_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int64_t)); // read data length (data block length)

            if (index != emptyBlockIndex) // there is null block
            {
                columnPolygon.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty ComplexPolygon block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<ColmnarDB::Types::ComplexPolygon> dataPolygon;
                std::unique_ptr<char[]> data(new char[dataLength]);

                colFile.read(data.get(), dataLength); // read entry data

                int64_t byteIndex = 0;
                while (byteIndex < dataLength)
                {
                    int32_t entryByteLength = *reinterpret_cast<int32_t*>(&data[byteIndex]);

                    byteIndex += sizeof(int32_t);

                    std::unique_ptr<char[]> byteArray(new char[entryByteLength]);
                    memcpy(byteArray.get(), &data[byteIndex], entryByteLength);

                    ColmnarDB::Types::ComplexPolygon entryDataPolygon;

                    entryDataPolygon.ParseFromArray(byteArray.get(), entryByteLength);

                    dataPolygon.push_back(entryDataPolygon);

                    byteIndex += entryByteLength;
                }
                data.release();

                if (dataPolygon.size() > columnPolygon.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnPolygon.AddBlock(dataPolygon, groupId);
                block.SetNullBitmask(std::move(nullBitMask));
                BOOST_LOG_TRIVIAL(debug) << "Added ComplexPolygon block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_POINT:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_POINT, isNullable, isUnique);

        auto& columnPoint =
            dynamic_cast<ColumnBase<ColmnarDB::Types::Point>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;
            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int64_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int64_t)); // read byte data length (data block length)

            if (index != emptyBlockIndex) // there is null block
            {
                columnPoint.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty Point block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<ColmnarDB::Types::Point> dataPoint;
                std::unique_ptr<char[]> data(new char[dataLength]);

                colFile.read(data.get(), dataLength); // read entry data

                int64_t byteIndex = 0;
                while (byteIndex < dataLength)
                {
                    int32_t entryByteLength = *reinterpret_cast<int32_t*>(&data[byteIndex]);

                    byteIndex += sizeof(int32_t);

                    std::unique_ptr<char[]> byteArray(new char[entryByteLength]);
                    memcpy(byteArray.get(), &data[byteIndex], entryByteLength);

                    ColmnarDB::Types::Point entryDataPoint;

                    entryDataPoint.ParseFromArray(byteArray.get(), entryByteLength);

                    dataPoint.push_back(entryDataPoint);

                    byteIndex += entryByteLength;
                }
                data.release();

                if (dataPoint.size() > columnPoint.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnPoint.AddBlock(dataPoint, groupId);
                block.SetNullBitmask(std::move(nullBitMask));

                BOOST_LOG_TRIVIAL(debug) << "Added Point block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_STRING:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_STRING, isNullable, isUnique);

        auto& columnString = dynamic_cast<ColumnBase<std::string>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int64_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int64_t)); // read data length (data block length)

            if (index != emptyBlockIndex) // there is null block
            {
                columnString.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty String block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<std::string> dataString;
                std::unique_ptr<char[]> data(new char[dataLength]);

                colFile.read(data.get(), dataLength); // read block length

                int64_t byteIndex = 0;
                while (byteIndex < dataLength)
                {
                    int32_t entryByteLength = *reinterpret_cast<int32_t*>(&data[byteIndex]); // read entry byte length

                    byteIndex += sizeof(int32_t);

                    std::unique_ptr<char[]> byteArray(new char[entryByteLength]);
                    memcpy(byteArray.get(), &data[byteIndex], entryByteLength); // read entry data

                    std::string entryDataString(byteArray.get());
                    dataString.push_back(entryDataString);

                    byteIndex += entryByteLength;
                }
                data.release();

                if (dataString.size() > columnString.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnString.AddBlock(dataString, groupId);
                block.SetNullBitmask(std::move(nullBitMask));

                BOOST_LOG_TRIVIAL(debug) << "Added String block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_INT8_T:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_INT8_T, isNullable, isUnique);

        auto& columnInt = dynamic_cast<ColumnBase<int8_t>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int32_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // read data length (number of entries)
            int8_t isCompressed;
            colFile.read(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // read whether compressed
            int8_t min;
            colFile.read(reinterpret_cast<char*>(&min), sizeof(int8_t)); // read statistics min
            int8_t max;
            colFile.read(reinterpret_cast<char*>(&max), sizeof(int8_t)); // read statistics max
            float avg;
            colFile.read(reinterpret_cast<char*>(&avg), sizeof(float)); // read statistics avg
            int8_t sum;
            colFile.read(reinterpret_cast<char*>(&sum), sizeof(int8_t)); // read statistics sum

            if (index != emptyBlockIndex) // there is null block
            {
                columnInt.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty Int8 block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<int8_t> data(dataLength, 0);

                colFile.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(int8_t)); // read entry data

                if (data.size() > columnInt.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnInt.AddBlock(data, groupId, false, static_cast<bool>(isCompressed));
                block.SetNullBitmask(std::move(nullBitMask));
                block.setBlockStatistics(min, max, avg, sum);

                BOOST_LOG_TRIVIAL(debug) << "Added Int8 block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_INT:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_INT, isNullable, isUnique);

        auto& columnInt = dynamic_cast<ColumnBase<int32_t>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int32_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // read data length (number of entries)
            int8_t isCompressed;
            colFile.read(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // read whether compressed
            int32_t min;
            colFile.read(reinterpret_cast<char*>(&min), sizeof(int32_t)); // read statistics min
            int32_t max;
            colFile.read(reinterpret_cast<char*>(&max), sizeof(int32_t)); // read statistics max
            float avg;
            colFile.read(reinterpret_cast<char*>(&avg), sizeof(float)); // read statistics avg
            int32_t sum;
            colFile.read(reinterpret_cast<char*>(&sum), sizeof(int32_t)); // read statistics sum

            if (index != emptyBlockIndex) // there is null block
            {
                columnInt.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty Int32 block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<int32_t> data(dataLength, 0);

                colFile.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(int32_t)); // read entry data

                if (data.size() > columnInt.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnInt.AddBlock(data, groupId, false, static_cast<bool>(isCompressed));
                block.SetNullBitmask(std::move(nullBitMask));
                block.setBlockStatistics(min, max, avg, sum);

                BOOST_LOG_TRIVIAL(debug) << "Added Int32 block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_LONG:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_LONG, isNullable, isUnique);

        auto& columnLong = dynamic_cast<ColumnBase<int64_t>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int32_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // read data length (number of entries)
            int8_t isCompressed;
            colFile.read(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // read whether compressed
            int64_t min;
            colFile.read(reinterpret_cast<char*>(&min), sizeof(int64_t)); // read statistics min
            int64_t max;
            colFile.read(reinterpret_cast<char*>(&max), sizeof(int64_t)); // read statistics max
            float avg;
            colFile.read(reinterpret_cast<char*>(&avg), sizeof(float)); // read statistics avg
            int64_t sum;
            colFile.read(reinterpret_cast<char*>(&sum), sizeof(int64_t)); // read statistics sum

            if (index != emptyBlockIndex) // there is null block
            {
                columnLong.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty Int64 block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<int64_t> data(dataLength, 0);

                colFile.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(int64_t)); // read entry data

                if (data.size() > columnLong.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnLong.AddBlock(data, groupId, false, static_cast<bool>(isCompressed));
                block.SetNullBitmask(std::move(nullBitMask));
                block.setBlockStatistics(min, max, avg, sum);

                BOOST_LOG_TRIVIAL(debug) << "Added Int64 block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_FLOAT:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_FLOAT, isNullable, isUnique);

        auto& columnFloat = dynamic_cast<ColumnBase<float>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int32_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // read data length (number of entries)
            int8_t isCompressed;
            colFile.read(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // read whether compressed
            float min;
            colFile.read(reinterpret_cast<char*>(&min), sizeof(float)); // read statistics min
            float max;
            colFile.read(reinterpret_cast<char*>(&max), sizeof(float)); // read statistics max
            float avg;
            colFile.read(reinterpret_cast<char*>(&avg), sizeof(float)); // read statistics avg
            float sum;
            colFile.read(reinterpret_cast<char*>(&sum), sizeof(float)); // read statistics sum

            if (index != emptyBlockIndex) // there is null block
            {
                columnFloat.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty Float block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<float> data(dataLength, 0);

                colFile.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(float)); // read entry data

                if (data.size() > columnFloat.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnFloat.AddBlock(data, groupId, false, static_cast<bool>(isCompressed));
                block.SetNullBitmask(std::move(nullBitMask));
                block.setBlockStatistics(min, max, avg, sum);

                BOOST_LOG_TRIVIAL(debug) << "Added Float block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    case COLUMN_DOUBLE:
    {
        table.CreateColumn(columnName.c_str(), COLUMN_DOUBLE, isNullable, isUnique);

        auto& columnDouble = dynamic_cast<ColumnBase<double>&>(*table.GetColumns().at(columnName));

        while (!colFile.eof())
        {
            int32_t index;
            colFile.read(reinterpret_cast<char*>(&index), sizeof(int32_t)); // read block index

            int32_t groupId;
            colFile.read(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // read block groupId

            int32_t nullBitMaskLength;

            if (isNullable)
            {
                colFile.read(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // read nullBitMask length
            }

            std::unique_ptr<int8_t[]> nullBitMask = nullptr;

            if (isNullable)
            {
                nullBitMask = std::unique_ptr<int8_t[]>(new int8_t[nullBitMaskAllocationSize]);
                colFile.read(reinterpret_cast<char*>(nullBitMask.get()), nullBitMaskLength); // read nullBitMask
            }

            // this is needed because of how EOF is checked:
            if (colFile.eof())
            {
                BOOST_LOG_TRIVIAL(debug)
                    << "Loading of the file: " << pathStr + dbName << SEPARATOR << table.GetName()
                    << SEPARATOR << columnName << ".col has finished successfully.";
                break;
            }

            int32_t dataLength;
            colFile.read(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // read data length (number of entries)
            int8_t isCompressed;
            colFile.read(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // read whether compressed
            double min;
            colFile.read(reinterpret_cast<char*>(&min), sizeof(double)); // read statistics min
            double max;
            colFile.read(reinterpret_cast<char*>(&max), sizeof(double)); // read statistics max
            float avg;
            colFile.read(reinterpret_cast<char*>(&avg), sizeof(float)); // read statistics avg
            double sum;
            colFile.read(reinterpret_cast<char*>(&sum), sizeof(double)); // read statistics sum

            if (index != emptyBlockIndex) // there is null block
            {
                columnDouble.AddBlock(); // add empty block
                BOOST_LOG_TRIVIAL(debug) << "Added empty Double block at index: " << emptyBlockIndex;
            }
            else // read data from block
            {
                std::vector<double> data(dataLength, 0);

                colFile.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(double)); // read entry data

                if (data.size() > columnDouble.GetBlockSize())
                {
                    throw std::runtime_error(
                        "Loaded data from disk does not fit into existing block");
                    break;
                }

                auto& block = columnDouble.AddBlock(data, groupId, false, static_cast<bool>(isCompressed));
                block.SetNullBitmask(std::move(nullBitMask));
                block.setBlockStatistics(min, max, avg, sum);

                BOOST_LOG_TRIVIAL(debug) << "Added Double block with data at index: " << index;
            }

            emptyBlockIndex += 1;
        }
    }
    break;

    default:
        throw std::domain_error("Unsupported data type (when loading database): " + std::to_string(type));
    }

    colFile.close();
}

/// <summary>
/// Creates table with given name and columns and adds it to database. If the table already existed, create missing columns if there are any missing
/// </summary>
/// <param name="columns">Columns with types.</param>
/// <param name="tableName">Table name.</param>
/// <param name="areNullable">Nullablity of columns. Default values are set to be true.</param>
/// <returns>Newly created table.</returns>
Table& Database::CreateTable(const std::unordered_map<std::string, DataType>& columns,
                             const char* tableName,
                             const std::unordered_map<std::string, bool>& areNullable)
{
    auto search = tables_.find(tableName);

    if (search != tables_.end())
    {
        auto& table = search->second;

        for (const auto& entry : columns)
        {
            if (table.ContainsColumn(entry.first.c_str()))
            {
                auto& tableColumns = table.GetColumns();

                if (tableColumns.at(entry.first)->GetColumnType() != entry.second)
                {
                    throw std::domain_error(
                        "Column type in CreateTable does not match with existing column.");
                }
            }
            else
            {
                if (areNullable.empty())
                {
                    table.CreateColumn(entry.first.c_str(), entry.second, true);
                }
                else
                {
                    table.CreateColumn(entry.first.c_str(), entry.second, areNullable.at(entry.first));
                }
            }
        }

        return table;
    }
    else
    {
        tables_.emplace(std::make_pair(tableName, Table(Database::GetDatabaseByName(name_), tableName)));
        auto& table = tables_.at(tableName);

        for (auto& entry : columns)
        {
            if (areNullable.empty())
            {
                table.CreateColumn(entry.first.c_str(), entry.second, true);
            }
            else
            {
                table.CreateColumn(entry.first.c_str(), entry.second, areNullable.at(entry.first));
            }
        }

        return table;
    }
}

/// <summary>
/// Add database to in memory list.
/// </summary>
/// <param name="database">Database to be added.</param>
void Database::AddToInMemoryDatabaseList(std::shared_ptr<Database> database)
{
    std::lock_guard<std::mutex> lock(dbAccessMutex_);
    if (!Context::getInstance().GetLoadedDatabases().insert({database->name_, database}).second)
    {
        throw std::invalid_argument("Attempt to insert duplicate database name");
    }
}

/// <summary>
/// Remove database from in memory database list.
/// </summary>
/// <param name="databaseName">Name of database to be removed.</param>
void Database::RemoveFromInMemoryDatabaseList(const char* databaseName)
{
    // erase db from map
    std::lock_guard<std::mutex> lock(dbAccessMutex_);
    Context::getInstance().GetLoadedDatabases().erase(databaseName);
}

/// <summary>
/// Get number of blocks.
/// </summary>
/// <returns>Number of blocks.</param>
int Database::GetBlockCount()
{
    for (auto& table : tables_)
    {
        for (auto& column : table.second.GetColumns())
        {
            return column.second.get()->GetBlockCount();
        }
    }
    return 0;
}

/// <summary>
/// Write column into disk.
/// </summary>
/// <param name="column">Column to be written.</param>
/// <param name="pathStr">Path to database storage directory.</param>
/// <param name="name">Names of particular column.</param>
/// <param name="table">Names of particular table.</param>
void Database::WriteColumn(const std::pair<const std::string, std::unique_ptr<IColumn>>& column,
                           std::string pathStr,
                           std::string name,
                           const std::pair<const std::string, Table>& table)
{
    BOOST_LOG_TRIVIAL(debug) << "Saving .col file with name: " << pathStr << name << SEPARATOR
                             << table.first << SEPARATOR << column.second->GetName() << ".col";

    std::ofstream colFile(pathStr + "/" + name + SEPARATOR + table.first + SEPARATOR +
                              column.second->GetName() + ".col",
                          std::ios::binary);

    int32_t type = column.second->GetColumnType();
    bool isNullable = column.second->GetIsNullable();
    bool isUnique = column.second->GetIsUnique();

    colFile.write(reinterpret_cast<char*>(&type), sizeof(int32_t)); // write type of column
    colFile.write(reinterpret_cast<char*>(&isNullable), sizeof(bool)); // write nullability of column
    colFile.write(reinterpret_cast<char*>(&isUnique), sizeof(bool)); // write unicity of column

    switch (type)
    {
    case COLUMN_POLYGON:
    {
        int32_t index = 0;

        const ColumnBase<ColmnarDB::Types::ComplexPolygon>& colPolygon =
            dynamic_cast<const ColumnBase<ColmnarDB::Types::ComplexPolygon>&>(*(column.second));

        for (const auto& block : colPolygon.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of ComplexPolygon data with index = " << index;

            auto data = block->GetData();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            int64_t dataByteSize = 0;

            for (int32_t i = 0; i < dataLength; i++)
            {
                dataByteSize += data[i].ByteSize();
            }

            int64_t dataRawLength = dataByteSize + dataLength * sizeof(int32_t);

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataRawLength), sizeof(int64_t)); // write block length in bytes
                for (size_t i = 0; i < dataLength; i++)
                {
                    int32_t entryByteLength = data[i].ByteSize();
                    std::unique_ptr<char[]> byteArray(new char[entryByteLength]);

                    data[i].SerializeToArray(byteArray.get(), entryByteLength);

                    colFile.write(reinterpret_cast<char*>(&entryByteLength), sizeof(int32_t)); // write entry length
                    colFile.write(byteArray.get(), entryByteLength); // write entry data
                }
                index += 1;
            }
        }
    }
    break;

    case COLUMN_POINT:
    {
        int32_t index = 0;

        const ColumnBase<ColmnarDB::Types::Point>& colPoint =
            dynamic_cast<const ColumnBase<ColmnarDB::Types::Point>&>(*(column.second));

        for (const auto& block : colPoint.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of Point data with index = " << index;

            auto data = block->GetData();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            int64_t dataByteSize = 0;

            for (int32_t i = 0; i < dataLength; i++)
            {
                dataByteSize += data[i].ByteSize();
            }

            int64_t dataRawLength = dataByteSize + dataLength * sizeof(int32_t);

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataRawLength), sizeof(int64_t)); // write block length in bytes
                for (size_t i = 0; i < dataLength; i++)
                {
                    int32_t entryByteLength = data[i].ByteSize();
                    std::unique_ptr<char[]> byteArray(new char[entryByteLength]);

                    data[i].SerializeToArray(byteArray.get(), entryByteLength);

                    colFile.write(reinterpret_cast<char*>(&entryByteLength), sizeof(int32_t)); // write entry length
                    colFile.write(byteArray.get(), entryByteLength); // write entry data
                }
                index += 1;
            }
        }
    }
    break;

    case COLUMN_STRING:
    {
        int32_t index = 0;

        const ColumnBase<std::string>& colStr =
            dynamic_cast<const ColumnBase<std::string>&>(*(column.second));

        for (const auto& block : colStr.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of String data with index = " << index;

            auto data = block->GetData();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            int64_t dataByteSize = 0;

            for (int32_t i = 0; i < dataLength; i++)
            {
                dataByteSize += data[i].length() + 1;
            }

            int64_t dataRawLength = dataByteSize + dataLength * sizeof(int32_t);

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataRawLength), sizeof(int64_t)); // write block length in bytes
                for (size_t i = 0; i < dataLength; i++)
                {
                    int32_t entryByteLength = data[i].length() + 1; // +1 because '\0'

                    colFile.write(reinterpret_cast<char*>(&entryByteLength), sizeof(int32_t)); // write entry length
                    colFile.write(data[i].c_str(), entryByteLength); // write entry data
                }
                index += 1;
            }
        }
    }
    break;

    case COLUMN_INT8_T:
    {
        int32_t index = 0;

        const ColumnBase<int8_t>& colInt = dynamic_cast<const ColumnBase<int8_t>&>(*(column.second));

        for (const auto& block : colInt.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of Int8 data with index = " << index;

            auto data = block->GetData();
            int8_t isCompressed = (int8_t)block->IsCompressed();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            int8_t min = block->GetMin();
            int8_t max = block->GetMax();
            float avg = block->GetAvg();
            int8_t sum = block->GetSum();

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // write block length (number of entries)
                colFile.write(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // write whether compressed
                colFile.write(reinterpret_cast<char*>(&min), sizeof(int8_t)); // write statistics min
                colFile.write(reinterpret_cast<char*>(&max), sizeof(int8_t)); // write statistics max
                colFile.write(reinterpret_cast<char*>(&avg), sizeof(float)); // write statistics avg
                colFile.write(reinterpret_cast<char*>(&sum), sizeof(int8_t)); // write statistics sum
                colFile.write(reinterpret_cast<const char*>(data), dataLength * sizeof(int8_t)); // write block of data
                index += 1;
            }
        }
    }
    break;

    case COLUMN_INT:
    {
        int32_t index = 0;

        const ColumnBase<int32_t>& colInt = dynamic_cast<const ColumnBase<int32_t>&>(*(column.second));

        for (const auto& block : colInt.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of Int32 data with index = " << index;

            auto data = block->GetData();
            int8_t isCompressed = (int8_t)block->IsCompressed();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            int32_t min = block->GetMin();
            int32_t max = block->GetMax();
            float avg = block->GetAvg();
            int32_t sum = block->GetSum();

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // write block length (number of entries)
                colFile.write(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // write whether compressed
                colFile.write(reinterpret_cast<char*>(&min), sizeof(int32_t)); // write statistics min
                colFile.write(reinterpret_cast<char*>(&max), sizeof(int32_t)); // write statistics max
                colFile.write(reinterpret_cast<char*>(&avg), sizeof(float)); // write statistics avg
                colFile.write(reinterpret_cast<char*>(&sum), sizeof(int32_t)); // write statistics sum
                colFile.write(reinterpret_cast<const char*>(data), dataLength * sizeof(int32_t)); // write block of data
                index += 1;
            }
        }
    }
    break;

    case COLUMN_LONG:
    {
        int32_t index = 0;

        const ColumnBase<int64_t>& colLong = dynamic_cast<const ColumnBase<int64_t>&>(*(column.second));

        for (const auto& block : colLong.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of Int64 data with index = " << index;

            auto data = block->GetData();
            int8_t isCompressed = (int8_t)block->IsCompressed();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            int64_t min = block->GetMin();
            int64_t max = block->GetMax();
            float avg = block->GetAvg();
            int64_t sum = block->GetSum();

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // write block length (number of entries)
                colFile.write(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // write whether compressed
                colFile.write(reinterpret_cast<char*>(&min), sizeof(int64_t)); // write statistics min
                colFile.write(reinterpret_cast<char*>(&max), sizeof(int64_t)); // write statistics max
                colFile.write(reinterpret_cast<char*>(&avg), sizeof(float)); // write statistics avg
                colFile.write(reinterpret_cast<char*>(&sum), sizeof(int64_t)); // write statistics sum
                colFile.write(reinterpret_cast<const char*>(data), dataLength * sizeof(int64_t)); // write block of data
                index += 1;
            }
        }
    }
    break;

    case COLUMN_FLOAT:
    {
        int32_t index = 0;

        const ColumnBase<float>& colFloat = dynamic_cast<const ColumnBase<float>&>(*(column.second));

        for (const auto& block : colFloat.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of Float data with index = " << index;

            auto data = block->GetData();
            int8_t isCompressed = (int8_t)block->IsCompressed();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            float min = block->GetMin();
            float max = block->GetMax();
            float avg = block->GetAvg();
            float sum = block->GetSum();

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // write block length (number of entries)
                colFile.write(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // write whether compressed
                colFile.write(reinterpret_cast<char*>(&min), sizeof(float)); // write statistics min
                colFile.write(reinterpret_cast<char*>(&max), sizeof(float)); // write statistics max
                colFile.write(reinterpret_cast<char*>(&avg), sizeof(float)); // write statistics avg
                colFile.write(reinterpret_cast<char*>(&sum), sizeof(float)); // write statistics sum
                colFile.write(reinterpret_cast<const char*>(data), dataLength * sizeof(float)); // write block of data
                index += 1;
            }
        }
    }
    break;

    case COLUMN_DOUBLE:
    {
        int32_t index = 0;

        const ColumnBase<double>& colDouble = dynamic_cast<const ColumnBase<double>&>(*(column.second));

        for (const auto& block : colDouble.GetBlocksList())
        {
            BOOST_LOG_TRIVIAL(debug) << "Saving block of Double data with index = " << index;

            auto data = block->GetData();
            int8_t isCompressed = (int8_t)block->IsCompressed();
            int32_t groupId = block->GetGroupId();
            int32_t dataLength = block->GetSize();
            double min = block->GetMin();
            double max = block->GetMax();
            float avg = block->GetAvg();
            double sum = block->GetSum();

            if (dataLength > 0)
            {
                colFile.write(reinterpret_cast<char*>(&index), sizeof(int32_t)); // write index
                colFile.write(reinterpret_cast<char*>(&groupId), sizeof(int32_t)); // write groupId
                if (isNullable)
                {
                    int32_t nullBitMaskLength =
                        (block->GetSize() + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    colFile.write(reinterpret_cast<char*>(&nullBitMaskLength), sizeof(int32_t)); // write nullBitMask length
                    colFile.write(reinterpret_cast<char*>(block->GetNullBitmask()), nullBitMaskLength); // write nullBitMask
                }
                colFile.write(reinterpret_cast<char*>(&dataLength), sizeof(int32_t)); // write block length (number of entries)
                colFile.write(reinterpret_cast<char*>(&isCompressed), sizeof(int8_t)); // write whether compressed
                colFile.write(reinterpret_cast<char*>(&min), sizeof(double)); // write statistics min
                colFile.write(reinterpret_cast<char*>(&max), sizeof(double)); // write statistics max
                colFile.write(reinterpret_cast<char*>(&avg), sizeof(float)); // write statistics avg
                colFile.write(reinterpret_cast<char*>(&sum), sizeof(double)); // write statistics sum
                colFile.write(reinterpret_cast<const char*>(data), dataLength * sizeof(double)); // write block of data
                index += 1;
            }
        }
    }
    break;

    default:
        throw std::domain_error("Unsupported data type (when persisting database): " + std::to_string(type));
        break;
    }

    colFile.close();
}
