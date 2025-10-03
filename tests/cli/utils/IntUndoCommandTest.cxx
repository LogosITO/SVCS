/**
 * @file IntUndoCommandTest.cxx
 * @brief Implementation of the UndoCommandTest fixture.
 *
 * @details This file defines the setup and teardown routines for the UndoCommand 
 * integration test fixture, along with helper methods to simplify test case creation 
 * by automating file and directory creation, staging, committing, and output verification.
 * * @note The `simulateUserInput` method currently contains a placeholder comment, 
 * as mocking `std::cin` requires more complex setup, which is omitted here. 
 * For full interactive testing, the command/event bus must support injectable input streams.
 * * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "IntUndoCommandTest.hxx"

#include <fstream>
#include <algorithm>

void UndoCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<UndoCommand>(this->mockEventBus, this->repoManager);
    
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_undo";
    
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

void UndoCommandTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->clear();
}

void UndoCommandTest::createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void UndoCommandTest::createTestDirectory(const std::string& dirname) {
    std::filesystem::create_directories(testDir / dirname);
}

void UndoCommandTest::stageFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        repoManager->addFileToStaging(file);
    }
}

bool UndoCommandTest::createTestCommit(const std::string& message) {
    stageFiles({"file1.txt", "file2.txt"});
    return repoManager->saveStagedChanges(message);
}

bool UndoCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

int UndoCommandTest::countMessages(const std::vector<Event>& notifications, const std::string& message) {
    return std::count_if(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

void UndoCommandTest::simulateUserInput(const std::string& input) {}    