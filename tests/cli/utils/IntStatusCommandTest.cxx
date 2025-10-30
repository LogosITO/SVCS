/**
* @file IntStatusCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the integration tests for the StatusCommand class.
 * @details This file provides the concrete implementation for the StatusCommandTest fixture.
 * It sets up a temporary SVCS repository with initial untracked files and includes
 * helper functions for creating files, directories, and staging changes. The fixture
 * is designed to test the `svcs status` command's ability to report the working
 * tree's state accurately.
 *
 * @russian
 * @brief Реализация интеграционных тестов для класса StatusCommand.
 * @details Этот файл предоставляет конкретную реализацию для фикстура StatusCommandTest.
 * Он настраивает временный репозиторий SVCS с начальными неотслеживаемыми файлами и включает
 * вспомогательные функции для создания файлов, директорий и индексации изменений. Фикстур
 * предназначен для тестирования способности команды `svcs status` точно отчитываться о состоянии
 * рабочего дерева.
 */

#include "IntStatusCommandTest.hxx"

#include <fstream>
#include <algorithm>

void StatusCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<StatusCommand>(this->mockEventBus, this->repoManager);
    
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_status";
    
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
    createTestDirectory("subdir");
    createTestFile("subdir/file4.txt", "content4");
}

void StatusCommandTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->clear();
}

void StatusCommandTest::createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void StatusCommandTest::createTestDirectory(const std::string& dirname) {
    std::filesystem::create_directories(testDir / dirname);
}

void StatusCommandTest::stageFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        repoManager->addFileToStaging(file);
    }
}

bool StatusCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

int StatusCommandTest::countMessages(const std::vector<Event>& notifications, const std::string& message) {
    return std::count_if(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}