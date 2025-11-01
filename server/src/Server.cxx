/**
 * @file Server.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the Server class for SVCS remote operations.
 * @details Provides TCP server functionality for handling Git protocol connections
 * and delegating to RemoteProtocol for actual push/pull operations.
 *
 * @russian
 * @brief Реализация класса Server для удаленных операций SVCS.
 * @details Предоставляет функциональность TCP-сервера для обработки соединений
 * по Git-протоколу и делегирования RemoteProtocol для фактических операций push/pull.
 */
#include "Server.hxx"

#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;
using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context,
               unsigned short port,
               std::shared_ptr<RepositoryManager> repo_manager,
               std::shared_ptr<ISubject> event_bus)
    : io_context_(io_context)
    , acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    , repository_manager_(std::move(repo_manager))
    , event_bus_(std::move(event_bus))
    , is_running_(false)
{
    notifyServerEvent("Server instance created on port " + std::to_string(port));
}

Server::~Server()
{
    stop();
    notifyServerEvent("Server instance destroyed");
}

bool Server::start()
{
    if (is_running_) {
        notifyServerError("Server is already running");
        return false;
    }

    try {
        is_running_ = true;
        startAccept();

        std::stringstream ss;
        ss << "Server started successfully on port " << acceptor_.local_endpoint().port();
        notifyServerEvent(ss.str());

        return true;
    } catch (const std::exception& e) {
        is_running_ = false;
        notifyServerError(std::string("Failed to start server: ") + e.what());
        return false;
    }
}

void Server::stop()
{
    if (!is_running_) {
        return;
    }

    is_running_ = false;

    try {
        boost::system::error_code ec;
        acceptor_.close(ec);

        if (ec) {
            notifyServerError("Error closing acceptor: " + ec.message());
        }

        std::lock_guard<std::mutex> lock(connections_mutex_);
        for (auto& socket : active_connections_) {
            if (socket->is_open()) {
                socket->close(ec);
                if (ec) {
                    notifyServerError("Error closing client socket: " + ec.message());
                }
            }
        }
        active_connections_.clear();

        notifyServerEvent("Server stopped");
    } catch (const std::exception& e) {
        notifyServerError(std::string("Error during server shutdown: ") + e.what());
    }
}

bool Server::isRunning() const
{
    return is_running_;
}

unsigned short Server::getPort() const
{
    try {
        return acceptor_.local_endpoint().port();
    } catch (const std::exception& e) {
        notifyServerError(std::string("Error getting port: ") + e.what());
        return 0;
    }
}

size_t Server::getActiveConnections() const
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    return active_connections_.size();
}

void Server::startAccept()
{
    if (!is_running_) {
        return;
    }

    auto socket = std::make_shared<tcp::socket>(io_context_);

    acceptor_.async_accept(*socket,
        [this, socket](const boost::system::error_code& error) {
            handleAccept(socket, error);
        });
}

void Server::handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error)
{
    if (!error && is_running_) {
        std::string client_endpoint = "unknown";
        try {
            client_endpoint = socket->remote_endpoint().address().to_string() + ":" +
                            std::to_string(socket->remote_endpoint().port());
        } catch (const std::exception&) {
            // Use default "unknown" value
        }

        notifyClientConnection("Client connected from " + client_endpoint);

        {
            std::lock_guard<std::mutex> lock(connections_mutex_);
            active_connections_.insert(socket);
        }

        spawnClientHandler(socket);

        startAccept();
    } else if (error && is_running_) {
        notifyServerError("Accept error: " + error.message());
        if (error != boost::asio::error::operation_aborted) {
            startAccept();
        }
    }
}

void Server::spawnClientHandler(std::shared_ptr<tcp::socket> socket)
{
    std::thread([this, socket]() {
        handleClient(socket);
    }).detach();
}

void Server::handleClient(std::shared_ptr<tcp::socket> socket)
{
    try {
        std::string service_type = determineServiceType(socket);

        if (service_type.empty()) {
            notifyServerError("Failed to determine service type for client");
            return;
        }

        notifyClientConnection("Client requested service: " + service_type);

        bool success = executeProtocol(socket, service_type);

        if (success) {
            notifyClientConnection("Service " + service_type + " completed successfully");
        } else {
            notifyServerError("Service " + service_type + " failed");
        }

    } catch (const std::exception& e) {
        notifyServerError(std::string("Client handling error: ") + e.what());
    }

    removeClientConnection(socket);
}

std::string Server::determineServiceType(std::shared_ptr<tcp::socket> socket)
{
    try {
        boost::system::error_code ec;
        boost::asio::streambuf buffer;

        size_t bytes_read = boost::asio::read_until(*socket, buffer, '\0', ec);

        if (ec && ec != boost::asio::error::eof) {
            notifyServerError("Read error in determineServiceType: " + ec.message());
            return "";
        }

        if (bytes_read == 0) {
            return "";
        }

        std::istream stream(&buffer);
        std::string request_line;
        std::getline(stream, request_line, '\0');

        if (request_line.empty()) {
            return "";
        }

        if (request_line.find("git-upload-pack") != std::string::npos) {
            return "upload-pack";
        } else if (request_line.find("git-receive-pack") != std::string::npos) {
            return "receive-pack";
        } else {
            notifyServerError("Unknown service request: " + request_line);
            return "";
        }

    } catch (const std::exception& e) {
        notifyServerError(std::string("Error determining service type: ") + e.what());
        return "";
    }
}

bool Server::executeProtocol(std::shared_ptr<tcp::socket> socket, const std::string& service_type)
{
    try {
        RemoteProtocol protocol(event_bus_, repository_manager_);

        if (service_type == "upload-pack") {
            return protocol.handleUploadPack();
        } else if (service_type == "receive-pack") {
            return protocol.handleReceivePack();
        } else {
            notifyServerError("Unknown service type: " + service_type);
            return false;
        }
    } catch (const std::exception& e) {
        notifyServerError(std::string("Protocol execution error: ") + e.what());
        return false;
    }
}

void Server::removeClientConnection(std::shared_ptr<tcp::socket> socket)
{
    try {
        if (socket->is_open()) {
            boost::system::error_code ec;
            socket->shutdown(tcp::socket::shutdown_both, ec);
            socket->close(ec);
        }

        std::lock_guard<std::mutex> lock(connections_mutex_);
        active_connections_.erase(socket);

    } catch (const std::exception& e) {
        notifyServerError(std::string("Error removing client connection: ") + e.what());
    }
}

void Server::notifyServerEvent(const std::string& message) const
{
    if (event_bus_) {
        event_bus_->notify(Event{Event::GENERAL_INFO, message, "Server"});
    }
}

void Server::notifyClientConnection(const std::string& message) const
{
    if (event_bus_) {
        event_bus_->notify(Event{Event::GENERAL_INFO, message, "Server"});
    }
}

void Server::notifyServerError(const std::string& message) const
{
    if (event_bus_) {
        event_bus_->notify(Event{Event::ERROR_MESSAGE, message, "Server"});
    }
}