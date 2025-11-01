/**
 * @file ServerIntegrationTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Integration tests for the Server class with real RepositoryManager.
 * @details Tests server functionality with actual dependencies including
 * initialization, startup/shutdown, port configuration, and event bus communication.
 * Uses real RepositoryManager instance with temporary test directories.
 *
 * @russian
 * @brief Интеграционные тесты для класса Server с реальным RepositoryManager.
 * @details Тестирует функциональность сервера с реальными зависимостями, включая
 * инициализацию, запуск/остановку, конфигурацию портов и коммуникацию с шиной событий.
 * Использует реальный экземпляр RepositoryManager с временными тестовыми директориями.
 */

#include "../../server/include/Server.hxx"
#include "../../services/ISubject.hxx"
#include "../../services/Event.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <boost/asio.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include <optional>
#include <filesystem>

using namespace std::chrono_literals;

class TestEventBus : public ISubject {
public:
    void attach(std::shared_ptr<IObserver> observer) override {}
    void detach(std::shared_ptr<IObserver> observer) override {}
    void notify(const Event& event) const override {
        last_event = event;
    }

    mutable std::optional<Event> last_event;
};

class ServerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir = std::filesystem::temp_directory_path() / ("svcs_test_" + std::to_string(time(nullptr)));
        std::filesystem::create_directories(test_dir);

        io_context = std::make_shared<boost::asio::io_context>();
        event_bus = std::make_shared<TestEventBus>();
        repo_manager = std::make_shared<RepositoryManager>(event_bus);
        repo_manager->initializeRepository(test_dir.string());

        port = findFreePort();
    }

    void TearDown() override {
        if (server && server->isRunning()) {
            server->stop();
        }
        server.reset();
        io_context->stop();
        std::filesystem::remove_all(test_dir);
    }

    unsigned short findFreePort() {
        boost::asio::io_context temp_io;
        boost::asio::ip::tcp::acceptor acceptor(temp_io);
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 0);
        acceptor.open(endpoint.protocol());
        acceptor.bind(endpoint);
        unsigned short port = acceptor.local_endpoint().port();
        acceptor.close();
        return port;
    }

    std::filesystem::path test_dir;
    std::shared_ptr<boost::asio::io_context> io_context;
    std::shared_ptr<TestEventBus> event_bus;
    std::shared_ptr<RepositoryManager> repo_manager;
    std::shared_ptr<Server> server;
    unsigned short port;
};

TEST_F(ServerIntegrationTest, ConstructorInitializesCorrectly) {
    EXPECT_NO_THROW({
        server = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);
    });

    EXPECT_NE(server, nullptr);
    EXPECT_FALSE(server->isRunning());
    EXPECT_EQ(server->getPort(), port);
    EXPECT_EQ(server->getActiveConnections(), 0);
}

TEST_F(ServerIntegrationTest, StartAndStopServer) {
    server = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);

    bool start_result = server->start();
    EXPECT_TRUE(start_result);
    EXPECT_TRUE(server->isRunning());

    server->stop();
    EXPECT_FALSE(server->isRunning());
    EXPECT_EQ(server->getActiveConnections(), 0);
}

TEST_F(ServerIntegrationTest, ServerPortConfiguration) {
    unsigned short test_port = 9420;
    server = std::make_shared<Server>(*io_context, test_port, repo_manager, event_bus);

    EXPECT_EQ(server->getPort(), test_port);
}

TEST_F(ServerIntegrationTest, ServerStateManagement) {
    server = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);

    EXPECT_FALSE(server->isRunning());
    EXPECT_EQ(server->getActiveConnections(), 0);

    server->start();
    EXPECT_TRUE(server->isRunning());

    server->stop();
    EXPECT_FALSE(server->isRunning());
}

TEST_F(ServerIntegrationTest, EventBusCommunication) {
    server = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);

    server->start();
    std::this_thread::sleep_for(50ms);
    EXPECT_TRUE(event_bus->last_event.has_value());
    server->stop();
}

TEST_F(ServerIntegrationTest, StartAlreadyRunningServer) {
    server = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);

    bool first_start = server->start();
    EXPECT_TRUE(first_start);
    EXPECT_TRUE(server->isRunning());

    bool second_start = server->start();
    EXPECT_FALSE(second_start);

    server->stop();
}

TEST_F(ServerIntegrationTest, StopNotRunningServer) {
    server = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);

    EXPECT_NO_THROW({
        server->stop();
    });

    EXPECT_FALSE(server->isRunning());
}

TEST_F(ServerIntegrationTest, ServerWithDifferentPorts) {
    auto server1 = std::make_shared<Server>(*io_context, port, repo_manager, event_bus);
    auto server2 = std::make_shared<Server>(*io_context, port + 1, repo_manager, event_bus);

    bool start1 = server1->start();
    bool start2 = server2->start();

    EXPECT_TRUE(start1);
    EXPECT_TRUE(start2);

    if (start1) server1->stop();
    if (start2) server2->stop();
}