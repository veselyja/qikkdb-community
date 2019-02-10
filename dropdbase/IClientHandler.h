#pragma once
#include <memory>
#include <google/protobuf/message.h>
#include "messages/InfoMessage.pb.h"
#include "messages/QueryMessage.pb.h"
#include "messages/CSVImportMessage.pb.h"
#include "messages/SetDatabaseMessage.pb.h"

class ITCPWorker;
/// <summary>
/// Interface for handling client requests
/// </summary>
class IClientHandler
{
public:
	/// <summary>
	/// Handle InfoMessage
	/// </summary>
	/// <param name="worker">Worker that requested handling</param>
	/// <param name="infoMessage">Message to handle</param>
	virtual std::unique_ptr<google::protobuf::Message> HandleInfoMessage(ITCPWorker& worker, const ColmnarDB::NetworkClient::Message::InfoMessage& infoMessage) = 0;
	/// <summary>
	/// Start execution of a query
	/// </summary>
	/// <param name="worker">Worker that requested handling</param>
	/// <param name="queryMessage">Message to handle</param>
	/// <returns>InfoMessage telling client to wait for execution to finish</returns>
	virtual std::unique_ptr<google::protobuf::Message> HandleQuery(ITCPWorker& worker, const ColmnarDB::NetworkClient::Message::QueryMessage& queryMessage) = 0;
	/// <summary>
	/// Import sent CSV
	/// </summary>
	/// <param name="worker">Worker that requested handling</param>
	/// <param name="csvImportMessage">Message to handle</param>
	/// <returns>InfoMessage representing success state of the operation</returns>
	virtual std::unique_ptr<google::protobuf::Message> HandleCSVImport(ITCPWorker& worker, const ColmnarDB::NetworkClient::Message::CSVImportMessage& csvImportMessage) = 0;
	/// <summary>
	/// Set working database
	/// </summary>
	/// <param name="worker">Worker that requested handling</param>
	/// <param name="setDatabaseMessage">Message to handle</param>
	/// <returns>InfoMessage representing success state of the operation</returns>
	virtual std::unique_ptr<google::protobuf::Message> HandleSetDatabase(ITCPWorker& worker, const ColmnarDB::NetworkClient::Message::SetDatabaseMessage& SetDatabaseMessage) = 0;
	virtual ~IClientHandler() {};
};
