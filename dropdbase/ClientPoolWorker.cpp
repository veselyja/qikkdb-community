#include "ClientPoolWorker.h"
#include "NetworkMessage.h"
#include "messages/InfoMessage.pb.h"
#include "messages/QueryMessage.pb.h"
#include "messages/CSVImportMessage.pb.h"
#include "messages/SetDatabaseMessage.pb.h"
#include "messages/BulkImportMessage.pb.h"
#include "IClientHandler.h"
#include <boost/log/trivial.hpp>

const int32_t MAXIMUM_BULK_FRAGMENT_SIZE = 8192 * 1024;

/// <summary>
/// Create new instance of ClientPoolWorker object
/// </summary>
/// <param name="activeWorkers">Instance of object responsible for handling messages</param>
/// <param name="handler">Instance of object responsible for handling messages</param>
/// <param name="socket">Client that will be handled by this instance</param>
/// <param name="requestTimeout">Timeout for TCP read and write in ms</param>
ClientPoolWorker::ClientPoolWorker(std::unique_ptr<IClientHandler>&& clientHandler,
                                   boost::asio::ip::tcp::socket socket,
                                   int requestTimeout)
: ITCPWorker(std::move(clientHandler), std::move(socket), requestTimeout)
{
    quit_ = false;
}

/// <summary>
/// Main client request handler
/// </summary>
void ClientPoolWorker::HandleClient()
{
    BOOST_LOG_TRIVIAL(debug) << "Waiting for hello from " << socket_.remote_endpoint().address().to_string();
    auto recvMsg = NetworkMessage::ReadFromNetwork(socket_);
    ColmnarDB::NetworkClient::Message::InfoMessage outInfo;
    if (!recvMsg.UnpackTo(&outInfo))
    {
        BOOST_LOG_TRIVIAL(error) << "Invalid message received from client "
                                 << socket_.remote_endpoint().address().to_string();
        Abort();
        return;
    }
    BOOST_LOG_TRIVIAL(debug) << "Hello from " << socket_.remote_endpoint().address().to_string();
    outInfo.set_message("");
    outInfo.set_code(ColmnarDB::NetworkClient::Message::InfoMessage::OK);
    NetworkMessage::WriteToNetwork(outInfo, socket_);
    BOOST_LOG_TRIVIAL(debug) << "Hello to " << socket_.remote_endpoint().address().to_string();
    try
    {
        while (!quit_ && !globalQuit_)
        {
            BOOST_LOG_TRIVIAL(debug)
                << "Waiting for message from " << socket_.remote_endpoint().address().to_string();
            recvMsg = NetworkMessage::ReadFromNetwork(socket_);
            BOOST_LOG_TRIVIAL(debug)
                << "Got message from " << socket_.remote_endpoint().address().to_string();
            if (recvMsg.Is<ColmnarDB::NetworkClient::Message::InfoMessage>())
            {
                ColmnarDB::NetworkClient::Message::InfoMessage infoMessage;
                recvMsg.UnpackTo(&infoMessage);
                BOOST_LOG_TRIVIAL(debug)
                    << "Info message from " << socket_.remote_endpoint().address().to_string();
                std::unique_ptr<google::protobuf::Message> resultMessage =
                    clientHandler_->HandleInfoMessage(*this, infoMessage);

                if (resultMessage != nullptr)
                {
                    NetworkMessage::WriteToNetwork(*resultMessage, socket_);
                }
            }
            else if (recvMsg.Is<ColmnarDB::NetworkClient::Message::QueryMessage>())
            {
                ColmnarDB::NetworkClient::Message::QueryMessage queryMessage;
                recvMsg.UnpackTo(&queryMessage);
                BOOST_LOG_TRIVIAL(debug)
                    << "Query message from " << socket_.remote_endpoint().address().to_string();
                std::unique_ptr<google::protobuf::Message> waitMessage =
                    clientHandler_->HandleQuery(*this, queryMessage);
                if (waitMessage != nullptr)
                {
                    NetworkMessage::WriteToNetwork(*waitMessage, socket_);
                }
            }
            else if (recvMsg.Is<ColmnarDB::NetworkClient::Message::CSVImportMessage>())
            {
                ColmnarDB::NetworkClient::Message::CSVImportMessage csvImportMessage;
                recvMsg.UnpackTo(&csvImportMessage);
                BOOST_LOG_TRIVIAL(debug)
                    << "CSV message from " << socket_.remote_endpoint().address().to_string();
                std::unique_ptr<google::protobuf::Message> importResultMessage =
                    clientHandler_->HandleCSVImport(*this, csvImportMessage);
                if (importResultMessage != nullptr)
                {
                    NetworkMessage::WriteToNetwork(*importResultMessage, socket_);
                }
            }
            else if (recvMsg.Is<ColmnarDB::NetworkClient::Message::SetDatabaseMessage>())
            {
                ColmnarDB::NetworkClient::Message::SetDatabaseMessage setDatabaseMessage;
                recvMsg.UnpackTo(&setDatabaseMessage);
                BOOST_LOG_TRIVIAL(debug) << "Set database message from "
                                         << socket_.remote_endpoint().address().to_string();
                std::unique_ptr<google::protobuf::Message> setDatabaseResult =
                    clientHandler_->HandleSetDatabase(*this, setDatabaseMessage);
                if (setDatabaseResult != nullptr)
                {
                    NetworkMessage::WriteToNetwork(*setDatabaseResult, socket_);
                }
            }
            else if (recvMsg.Is<ColmnarDB::NetworkClient::Message::BulkImportMessage>())
            {
                ColmnarDB::NetworkClient::Message::BulkImportMessage bulkImportMessage;
                recvMsg.UnpackTo(&bulkImportMessage);
                BOOST_LOG_TRIVIAL(debug)
                    << "BulkImport message from " << socket_.remote_endpoint().address().to_string();
                std::unique_ptr<char[]> dataBuffer(new char[MAXIMUM_BULK_FRAGMENT_SIZE]);
                constexpr size_t nullBufferSize =
                    (MAXIMUM_BULK_FRAGMENT_SIZE + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                std::unique_ptr<char[]> nullBuffer(new char[nullBufferSize]);
                std::memset(nullBuffer.get(), 0, nullBufferSize);
                DataType columnType = static_cast<DataType>(bulkImportMessage.columntype());
                int32_t elementCount = bulkImportMessage.elemcount();
                bool isNullable = bulkImportMessage.isnullable();
                if (elementCount * GetDataTypeSize(columnType) > MAXIMUM_BULK_FRAGMENT_SIZE)
                {
                    outInfo.set_message("Data fragment larger than allowed");
                    outInfo.set_code(ColmnarDB::NetworkClient::Message::InfoMessage::QUERY_ERROR);
                    NetworkMessage::WriteToNetwork(outInfo, socket_);
                    continue;
                }
                NetworkMessage::ReadRaw(socket_, dataBuffer.get(), elementCount, columnType);
                if (isNullable)
                {
                    size_t nullBufferSize = (elementCount + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
                    NetworkMessage::ReadRaw(socket_, nullBuffer.get(), nullBufferSize, DataType::COLUMN_INT8_T);
                }
                std::unique_ptr<google::protobuf::Message> importResultMessage =
                    clientHandler_->HandleBulkImport(*this, bulkImportMessage, dataBuffer.get(),
                                                     nullBuffer.get());
                if (importResultMessage != nullptr)
                {
                    NetworkMessage::WriteToNetwork(*importResultMessage, socket_);
                }
            }
            else
            {
                BOOST_LOG_TRIVIAL(error)
                    << "Invalid message from " << socket_.remote_endpoint().address().to_string();
            }
        }
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << e.what();
    }
    if (!quit_)
    {
        Abort();
    }
}

/// <summary>
/// Stops the worker after reading next message
/// </summary>
void ClientPoolWorker::Abort()
{
    quit_ = true;
    socket_.close();
}
