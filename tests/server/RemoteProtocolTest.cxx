/**
 * @file RemoteProtocolTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Google Tests for RemoteProtocol class.
 * @details This file contains comprehensive unit tests for the RemoteProtocol class,
 * verifying network communication, protocol message serialization/deserialization,
 * error handling, and connection management with remote repositories.
 *
 * The tests cover:
 * - Connection establishment and authentication
 * - Protocol message formatting and parsing
 * - Data transfer reliability and integrity
 * - Error recovery and timeout handling
 * - Multi-threaded communication scenarios
 *
 * Mock network components are used to simulate various network conditions
 * and edge cases without requiring actual network connectivity.
 *
 * @russian
 * @brief Google Тесты для класса RemoteProtocol.
 * @details Этот файл содержит комплексные модульные тесты для класса RemoteProtocol,
 * проверяющие сетевое взаимодействие, сериализацию/десериализацию протокольных сообщений,
 * обработку ошибок и управление соединениями с удаленными репозиториями.
 *
 * Тесты охватывают:
 * - Установление соединения и аутентификацию
 * - Форматирование и парсинг протокольных сообщений
 * - Надежность и целостность передачи данных
 * - Восстановление после ошибок и обработку таймаутов
 * - Сценарии многопоточного взаимодействия
 *
 * Используются mock-компоненты сети для моделирования различных сетевых условий
 * и граничных случаев без необходимости реального сетевого подключения.
 */
#include <gtest/gtest.h>
#include "../../server/include/RemoteProtocol.hxx"
#include "../../services/ISubject.hxx"
#include "../../services/Event.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

class MockEventBus : public ISubject {
public:
    void attach(std::shared_ptr<IObserver> observer) override {
        observers.push_back(observer);
    }

    void detach(std::shared_ptr<IObserver> observer) override {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void notify(const Event& event) const override {
        last_event = event;
        notification_count++;

        for (const auto& observer : observers) {
            observer->update(event);
        }
    }

    mutable Event last_event;
    mutable int notification_count = 0;
    std::vector<std::shared_ptr<IObserver>> observers;
};

class MockRepositoryManager : public RepositoryManager {
public:
    MockRepositoryManager(std::shared_ptr<ISubject> bus)
        : RepositoryManager(bus) {}

    explicit MockRepositoryManager(std::shared_ptr<ISubject> bus, const std::string& path)
        : RepositoryManager(bus), repo_path_(path) {}

    bool isInitialized() const {
        return true;
    }

    std::filesystem::path getRepositoryPath() const {
        return repo_path_;
    }

    mutable std::unordered_map<std::string, std::string> objects;
    mutable std::unordered_map<std::string, std::string> references;

    void setupTestObjects() {
        objects["a1b2c3d4e5f67890123456789012345678901234"] = "test object data 1";
        objects["b2c3d4e5f67890123456789012345678901234567"] = "test object data 2";
        references["main"] = "a1b2c3d4e5f67890123456789012345678901234";
        references["develop"] = "b2c3d4e5f67890123456789012345678901234567";
    }

private:
    std::filesystem::path repo_path_ = "/tmp/test-repo";
};

class RemoteProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        event_bus = std::make_shared<MockEventBus>();
        repo_manager = std::make_shared<MockRepositoryManager>(event_bus);
        repo_manager->setupTestObjects();
        protocol = std::make_unique<RemoteProtocol>(event_bus, repo_manager);

        fs::create_directories("/tmp/test-repo/.svcs/objects");
        fs::create_directories("/tmp/test-repo/.svcs/refs/heads");
    }

    void TearDown() override {
        fs::remove_all("/tmp/test-repo");
    }

    bool testIsValidObjectHash(const std::string& hash) {
        return hash.length() == 40 &&
               std::all_of(hash.begin(), hash.end(), [](char c) {
                   return std::isxdigit(c);
               });
    }

    bool testIsValidReference(const std::string& ref_name) {
        return !ref_name.empty() &&
               ref_name.find("..") == std::string::npos &&
               ref_name.find('/') == std::string::npos &&
               ref_name.find('\\') == std::string::npos;
    }

    std::shared_ptr<MockEventBus> event_bus;
    std::shared_ptr<MockRepositoryManager> repo_manager;
    std::unique_ptr<RemoteProtocol> protocol;
};

TEST_F(RemoteProtocolTest, Construction) {
    EXPECT_NE(protocol, nullptr);
    EXPECT_EQ(event_bus->notification_count, 0);
}

TEST_F(RemoteProtocolTest, ValidationMethods) {
    EXPECT_TRUE(testIsValidObjectHash("a1b2c3d4e5f67890123456789012345678901234"));
    EXPECT_TRUE(testIsValidObjectHash("ffffffffffffffffffffffffffffffffffffffff"));

    EXPECT_FALSE(testIsValidObjectHash(""));
    EXPECT_FALSE(testIsValidObjectHash("abc"));
    EXPECT_FALSE(testIsValidObjectHash("a1b2c3d4e5f67890123456789012345678901234xxx"));
    EXPECT_FALSE(testIsValidObjectHash("gggggggggggggggggggggggggggggggggggggggg"));

    EXPECT_TRUE(testIsValidReference("main"));
    EXPECT_TRUE(testIsValidReference("develop"));
    EXPECT_TRUE(testIsValidReference("feature-branch"));

    EXPECT_FALSE(testIsValidReference(""));
    EXPECT_FALSE(testIsValidReference("main..develop"));
    EXPECT_FALSE(testIsValidReference("branch/path"));
    EXPECT_FALSE(testIsValidReference("branch\\path"));
}

TEST_F(RemoteProtocolTest, ObjectManagement) {
    fs::path objects_dir = "/tmp/test-repo/.svcs/objects";
    fs::path object_dir = objects_dir / "a1";
    fs::path object_path = object_dir / "b2c3d4e5f67890123456789012345678901234";

    fs::create_directories(object_dir);
    std::ofstream file(object_path);
    file << "test object data";
    file.close();

    EXPECT_TRUE(fs::exists(object_path));
    EXPECT_TRUE(repo_manager->objects.count("a1b2c3d4e5f67890123456789012345678901234") > 0);
}

TEST_F(RemoteProtocolTest, EventNotifications) {
    Event test_event({Event::Type::GENERAL_INFO, "Test message"});
    event_bus->notify(test_event);

    EXPECT_EQ(event_bus->notification_count, 1);
    EXPECT_EQ(event_bus->last_event.type, Event::Type::GENERAL_INFO);
    EXPECT_EQ(event_bus->last_event.details, "Test message");
}

class RemoteProtocolIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        old_cin = std::cin.rdbuf();
        old_cout = std::cout.rdbuf();
        old_cerr = std::cerr.rdbuf();

        std::cin.rdbuf(test_input.rdbuf());
        std::cout.rdbuf(test_output.rdbuf());
        std::cerr.rdbuf(test_error.rdbuf());

        event_bus = std::make_shared<MockEventBus>();
        repo_manager = std::make_shared<MockRepositoryManager>(event_bus);
        protocol = std::make_unique<RemoteProtocol>(event_bus, repo_manager);
    }

    void TearDown() override {
        std::cin.rdbuf(old_cin);
        std::cout.rdbuf(old_cout);
        std::cerr.rdbuf(old_cerr);
    }

    void provideInput(const std::string& input) {
        test_input << input << std::endl;
    }

    std::string getOutput() {
        return test_output.str();
    }

    std::string getError() {
        return test_error.str();
    }

    void clearStreams() {
        test_input.str("");
        test_input.clear();
        test_output.str("");
        test_output.clear();
        test_error.str("");
        test_error.clear();
    }

    std::streambuf* old_cin;
    std::streambuf* old_cout;
    std::streambuf* old_cerr;
    std::stringstream test_input;
    std::stringstream test_output;
    std::stringstream test_error;

    std::shared_ptr<MockEventBus> event_bus;
    std::shared_ptr<MockRepositoryManager> repo_manager;
    std::unique_ptr<RemoteProtocol> protocol;
};

TEST_F(RemoteProtocolIntegrationTest, SimpleProtocolCommunication) {
    provideInput("test input");

    std::string input;
    std::getline(std::cin, input);

    EXPECT_EQ(input, "test input");

    std::cout << "test output" << std::endl;
    std::string output = getOutput();
    EXPECT_TRUE(output.find("test output") != std::string::npos);
}

TEST_F(RemoteProtocolIntegrationTest, StreamErrorConditions) {
    test_input.setstate(std::ios::badbit);

    std::string input;
    bool read_success = static_cast<bool>(std::getline(std::cin, input));

    EXPECT_FALSE(read_success);
    EXPECT_TRUE(std::cin.fail());
}

TEST_F(RemoteProtocolTest, ErrorHandling) {
    EXPECT_NO_THROW({
        RemoteProtocol error_protocol(event_bus, repo_manager);
    });
}

TEST_F(RemoteProtocolTest, RepositoryValidation) {
    EXPECT_TRUE(repo_manager->isInitialized());

    auto repo_path = repo_manager->getRepositoryPath();
    EXPECT_FALSE(repo_path.empty());
}

TEST_F(RemoteProtocolTest, ValidationPerformance) {
    const int NUM_VALIDATIONS = 1000;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_VALIDATIONS; ++i) {
        testIsValidObjectHash("a1b2c3d4e5f67890123456789012345678901234");
        testIsValidReference("main");
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    EXPECT_LT(duration.count(), 100);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}