#pragma once
#include "IColumn.h"
#include "DataType.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#ifndef __CUDACC__
#include <any>
#endif

class Database;

/// <summary>
/// The main class representing table containing columns with data.
/// </summary>
class Table
{
private:
	const std::shared_ptr<Database>& database;
	std::string name;
	int32_t blockSize;
	std::unordered_map<std::string, std::unique_ptr<IColumn>> columns;
	std::unique_ptr<std::mutex> columnsMutex_;

public:
	const std::shared_ptr<Database> &GetDatabase() const;
	const std::string &GetName() const;
	int32_t GetBlockSize() const;
	int32_t GetBlockCount() const;
	const std::unordered_map<std::string, std::unique_ptr<IColumn>> &GetColumns() const;

	/// <summary>
	/// Initializes a new instance of the <see cref="T:ColmnarDB.Table"/> class. Also gets from database
	/// the block size and initializes with this value the private variable blockSize. Finally, it initializes columnsMutex_.
	/// </summary>
	/// <param name="database">Pointer to the database which will contains the new table.</param>
	/// <param name="name">Name of the newly created table.</param>
	Table(const std::shared_ptr<Database> &database, const char* name);

	/// <summary>
	/// Removes column from columns.
	/// </summary>
	/// <param name="columnName">Name of column to be removed.</param>
	void RemoveColumn(const char* columnName);

	/// <summary>
	/// Insert new column with proper data type into the table.
	/// </summary>
	/// <param name="columnName">Name of column.</param>
	/// <param name="dataType">Data type of colum.n</param>
	void CreateColumn(const char* columnName, DataType columnType);

#ifndef __CUDACC__
	/// <summary>
	/// Insert data into proper column of table considering empty space of last block and maximum size of blocks.
	/// </summary>
	/// <param name="data">Name of column with inserting data.</param>
	void InsertData(const std::unordered_map<std::string, std::any> &data);
	int32_t AssignGroupId(std::vector<std::any>& rowData, std::vector<std::unique_ptr<IColumn>>& columns);
#endif

	/// <summary>
	/// Search for column according to its name.
	/// </summary>
	/// <param name="column">Name of column.</param>
	/// <returns>Return true, if table contains particular column. Returns false, if table does not contains particular column.</returns>
	bool ContainsColumn(const char* column);
	std::vector<int32_t> GetTableGroupIds(std::unordered_map<std::string, std::unique_ptr<IColumn>>& columns);
	std::vector<int32_t> GetTableGroupIds(std::vector<std::unique_ptr<IColumn>>& columns);
};
