/**
 * @file InitCommandIntegrationTest.cxx
 * @brief Integration tests for the 'init' command (InitCommand) and the RepositoryManager.
 * * These tests verify the end-to-end functionality of repository initialization,
 * including creating the necessary directory structure on the filesystem and 
 * checking event notifications.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */

#include "../../services/Event.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../cli/include/InitCommand.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <memory>

/**
 * @brief Mock implementation of ISubject.
 * * This class captures all events published by the system core 
 * without requiring the full Observer system implementation.
 */
class MockSubject : public ISubject {
public:
    std::vector<Event> notifications;

    void attach(std::shared_ptr<IObserver> observer) override {}
    void detach(std::shared_ptr<IObserver> observer) override {}

    void notify(const Event& event) const override {
        const_cast<MockSubject*>(this)->notifications.push_back(event);
    }
};

class InitCommandIntegrationTest : public ::testing::Test {
protected:
    std::shared_ptr<MockSubject> mockEventBus;
    std::shared_ptr<RepositoryManager> repoManager;
    std::unique_ptr<InitCommand> command;
    std::filesystem::path testDir;

    void SetUp() override {
        mockEventBus = std::make_shared<MockSubject>();
        repoManager = std::make_shared<RepositoryManager>(mockEventBus);
        command = std::make_unique<InitCommand>(mockEventBus, repoManager);
        
        // Создаем временную директорию для тестов
        testDir = std::filesystem::temp_directory_path() / "svcs_test_init";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        std::filesystem::current_path(testDir);
    }

    void TearDown() override {
        std::filesystem::current_path(std::filesystem::temp_directory_path());
        std::filesystem::remove_all(testDir);
        mockEventBus->notifications.clear();
    }
};

/**
 * @brief Test fixture for InitCommand integration tests.
 * * Sets up a clean, temporary directory before each test and ensures it's 
 * cleaned up afterward. It uses the real RepositoryManager along with 
 * a MockSubject to verify side effects and notifications.
 */
TEST_F(InitCommandIntegrationTest, Success_InitInEmptyDirectory) {
    bool result = command->execute({});
    
    EXPECT_TRUE(result);
    
    // Проверяем что создались необходимые файлы
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs" / "objects"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs" / "refs"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs" / "HEAD"));
    
    // Проверяем уведомления
    bool foundSuccess = false;
    for (const auto& notification : mockEventBus->notifications) {
        if (notification.type == Event::REPOSITORY_INIT_SUCCESS) {
            foundSuccess = true;
            break;
        }
    }
    EXPECT_TRUE(foundSuccess);
}

TEST_F(InitCommandIntegrationTest, Success_WithForceFlag) {
    // Сначала создаем репозиторий
    command->execute({});
    
    // Затем переинициализируем с force
    mockEventBus->notifications.clear();
    bool result = command->execute({"-f"});
    
    EXPECT_TRUE(result);
    
    // Проверяем что репозиторий все еще существует
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs"));
}

TEST_F(InitCommandIntegrationTest, Success_WithCustomPath) {
    std::filesystem::create_directories(testDir / "subdir");
    
    bool result = command->execute({"subdir"});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(testDir / "subdir" / ".svcs"));
}

TEST_F(InitCommandIntegrationTest, HelpFlagShowsHelp) {
    bool result = command->execute({"--help"});
    
    EXPECT_TRUE(result);
    
    // Проверяем что help был показан (не было попытки инициализации)
    bool foundHelp = false;
    for (const auto& notification : mockEventBus->notifications) {
        if (notification.details.find("Usage:") != std::string::npos) {
            foundHelp = true;
            break;
        }
    }
    EXPECT_TRUE(foundHelp);
}