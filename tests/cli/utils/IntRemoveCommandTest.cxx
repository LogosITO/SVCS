/**
 * @file IntRemoveCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of integration tests for the RemoveCommand class.
 * @details This file provides the concrete implementation for the RemoveCommandTest fixture,
 * setting up a temporary SVCS repository with staged and unstaged files to thoroughly
 * test the 'svcs rm' command's logic, including handling files, directories,
 * and various options.
 *
 * @russian
 * @brief Реализация интеграционных тестов для класса RemoveCommand.
 * @details Этот файл предоставляет конкретную реализацию для фикстура RemoveCommandTest,
 * настраивая временный репозиторий SVCS с проиндексированными и непроиндексированными файлами
 * для тщательного тестирования логики команды 'svcs rm', включая обработку файлов, директорий
 * и различных параметров.
 */

#include "IntRemoveCommandTest.hxx"

#include <fstream>
#include <algorithm>

namespace svcs::test::cli::utils {

void RemoveCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<RemoveCommand>(this->mockEventBus, this->repoManager);
    
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_remove";
    
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

void RemoveCommandTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->clear();
}

void RemoveCommandTest::createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void RemoveCommandTest::createTestDirectory(const std::string& dirname) {
    std::filesystem::create_directories(testDir / dirname);
}

void RemoveCommandTest::stageFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        repoManager->addFileToStaging(file);
    }
}

bool RemoveCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

}