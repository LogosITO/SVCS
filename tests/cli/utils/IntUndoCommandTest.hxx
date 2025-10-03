/**
 * @file IntUndoCommandTest.hxx
 * @brief Test fixture header for integration tests of the UndoCommand class.
 *
 * @details Declares the UndoCommandTest fixture, which sets up a controlled 
 * environment for testing the 'svcs undo' command. This includes initializing 
 * mock and core components (MockSubject, RepositoryManager) and providing helper 
 * functions for file manipulation, staging, committing, and simulating user 
 * input for confirmation prompts.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

// Assuming the relative path is correct for your project structure
#include "../../../cli/include/UndoCommand.hxx"
#include "../mocks/MockSubject.hxx"
// Forward declarations for core components used in fixture
class RepositoryManager; 
struct Event; 

/**
 * @class UndoCommandTest
 * @brief Google Test fixture for running integration tests against the UndoCommand.
 *
 * @ingroup IntegrationTests
 *
 * @details This fixture provides the necessary infrastructure for testing the undo command:
 * - **SetUp/TearDown:** Manage a temporary SVCS repository directory.
 * - **Core Components:** Instances of MockSubject (for event bus/output/input simulation) and RepositoryManager (for core logic).
 * - **Helper Methods:** Simplify file manipulation, staging, committing, and verification of notifications.
 */
class UndoCommandTest : public ::testing::Test {
protected:
    /**
     * @brief Sets up the testing environment before each test.
     * @details Initializes components, creates the temporary directory, and initializes the repository.
     */
    void SetUp() override;

    /**
     * @brief Tears down the testing environment after each test.
     * @details Cleans up the temporary directory and restores the working path.
     */
    void TearDown() override;
    
    /**
     * @brief Creates a test file with specified content in the temporary directory.
     * @param filename The relative path/name of the file.
     * @param content The content to write to the file.
     */
    void createTestFile(const std::string& filename, const std::string& content);
    
    /**
     * @brief Creates a test directory in the temporary location.
     * @param dirname The relative path/name of the directory.
     */
    void createTestDirectory(const std::string& dirname);
    
    /**
     * @brief Stages a list of files using the RepositoryManager's staging logic.
     * @param files A vector of file names to stage.
     */
    void stageFiles(const std::vector<std::string>& files);
    
    /**
     * @brief Creates a new commit (save) with a specified message.
     * @details Internally stages files (if needed) and calls the repository manager's commit logic.
     * @param message The commit message.
     * @return true if the commit was successful, false otherwise.
     */
    bool createTestCommit(const std::string& message);
    
    /**
     * @brief Checks if a specific message (substring) exists in any event notification.
     * @param notifications The list of captured events.
     * @param message The substring to search for.
     * @return true if the message is found in any notification, false otherwise.
     */
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    
    /**
     * @brief Counts the occurrences of a specific message (substring) across all event notifications.
     * @param notifications The list of captured events.
     * @param message The substring to count.
     * @return The number of events containing the substring.
     */
    int countMessages(const std::vector<Event>& notifications, const std::string& message);

    /**
     * @brief Simulates user input by pushing the string into the mock event bus's input queue.
     * @details This is crucial for testing the confirmation prompt in the UndoCommand.
     * @param input The string to simulate as user input (e.g., "y\n" or "n\n").
     */
    void simulateUserInput(const std::string& input);
    
    /// Shared pointer to the mock event bus (observer/input simulator).
    std::shared_ptr<MockSubject> mockEventBus;
    
    /// Shared pointer to the repository core logic component.
    std::shared_ptr<RepositoryManager> repoManager;
    
    /// Unique pointer to the command being tested.
    std::unique_ptr<UndoCommand> command;
    
    /// Path to the temporary test repository directory.
    std::filesystem::path testDir;
};