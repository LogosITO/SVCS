/**
 * @file IntSaveCommandTest.cxx
 * @brief Implementation of integration tests for the SaveCommand class.
 *
 * @details This file contains the concrete implementation for the SaveCommandTest fixture. 
 * It sets up a temporary SVCS repository and provides utility methods (like file creation 
 * and staging) to simulate real-world usage scenarios for testing the 'svcs save' 
 * (commit) command's logic, message handling, and repository state changes.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "IntSaveCommandTest.hxx"

#include <fstream>
#include <algorithm>

void SaveCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<SaveCommand>(this->mockEventBus, this->repoManager);
    
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_save";
    
    std::filesystem::remove_all(this->testDir);
    std::filesystem::create_directories(this->testDir);
    std::filesystem::current_path(this->testDir);
    
    // Initialize repository
    this->repoManager->initializeRepository(".", true);
    this->mockEventBus->clear();
    
    // Create test files
    createTestFile("file1.txt", "content1");
    createTestFile("file2.txt", "content2");
    createTestDirectory("subdir");
    createTestFile("subdir/file3.txt", "content3");
}

void SaveCommandTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->clear();
}

void SaveCommandTest::createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void SaveCommandTest::createTestDirectory(const std::string& dirname) {
    std::filesystem::create_directories(testDir / dirname);
}

void SaveCommandTest::stageFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        repoManager->addFileToStaging(file);
    }
}

bool SaveCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}
