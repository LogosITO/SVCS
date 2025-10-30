/**
* @file IntHistoryCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the integration tests for the HistoryCommand class.
 * @details This file contains the test cases for verifying the correct behavior of
 * the HistoryCommand class when interacting with a real RepositoryManager and
 * the file system. It includes setup/teardown logic and helper functions for
 * creating files and commits to simulate a repository's history.
 *
 * @russian
 * @brief Реализация интеграционных тестов для класса HistoryCommand.
 * @details Этот файл содержит тестовые случаи для проверки корректного поведения
 * класса HistoryCommand при взаимодействии с реальным RepositoryManager и
 * файловой системой. Он включает логику настройки/очистки и вспомогательные функции
 * для создания файлов и коммитов для имитации истории репозитория.
 */

#include "IntHistoryCommandTest.hxx"

#include <fstream>
#include <algorithm>

void HistoryCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<HistoryCommand>(this->mockEventBus, this->repoManager);
    
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_history";
    
    std::filesystem::remove_all(this->testDir);
    std::filesystem::create_directories(this->testDir);
    std::filesystem::current_path(this->testDir);
    
    // Initialize repository
    this->repoManager->initializeRepository(".", true);
    this->mockEventBus->clear();
    
    // Create test files
    createTestFile("file1.txt", "content1");
    createTestFile("file2.txt", "content2");
    createTestFile("file3.txt", "content3");
}

void HistoryCommandTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->clear();
}

void HistoryCommandTest::createTestFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void HistoryCommandTest::createTestDirectory(const std::string& dirname) const {
    std::filesystem::create_directories(testDir / dirname);
}

void HistoryCommandTest::stageFiles(const std::vector<std::string>& files) const {
    for (const auto& file : files) {
        repoManager->addFileToStaging(file);
    }
}

bool HistoryCommandTest::createTestCommit(const std::string& message) {
    stageFiles({"file1.txt", "file2.txt"});
    return repoManager->saveStagedChanges(message);
}

bool HistoryCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            // Проверяем все типы сообщений, а не только GENERAL_INFO
            return event.details.find(message) != std::string::npos;
        });
}

int HistoryCommandTest::countMessages(const std::vector<Event>& notifications, const std::string& message) {
    return std::count_if(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}
