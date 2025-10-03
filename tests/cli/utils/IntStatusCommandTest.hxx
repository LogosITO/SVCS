/**
 * @file IntStatusCommandTest.hxx
 * @brief Test fixture header for integration tests of the StatusCommand class.
 *
 * @details Declares the StatusCommandTest fixture, which sets up a controlled
 * environment for testing the 'svcs status' command. This includes initializing 
 * mock and core components (MockSubject, RepositoryManager) and providing helper 
 * functions for file manipulation and checking command output.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "../../../cli/include/StatusCommand.hxx"
#include "../mocks/MockSubject.hxx"


/**
 * @class StatusCommandTest
 * @brief Google Test fixture for running integration tests against the StatusCommand.
 *
 * @ingroup IntegrationTests
 *
 * @details This fixture provides the necessary infrastructure for integration tests:
 * - **SetUp/TearDown:** Manage a temporary SVCS repository directory.
 * - **Core Components:** Instances of MockSubject (for event bus) and RepositoryManager (for core logic).
 * - **Helper Methods:** Simplify file creation, directory setup, staging, and verification of notifications.
 */
class StatusCommandTest : public ::testing::Test {
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
     * @brief Checks if a specific message (substring) exists in any event notification.
     * @param notifications The list of captured events.
     * @param message The substring to search for.
     * @return true if the message is found, false otherwise.
     */
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    
    /**
     * @brief Counts the occurrences of a specific message (substring) across all event notifications.
     * @param notifications The list of captured events.
     * @param message The substring to count.
     * @return The number of events containing the substring.
     */
    int countMessages(const std::vector<Event>& notifications, const std::string& message);
    
    /// Shared pointer to the mock event bus (observer).
    std::shared_ptr<MockSubject> mockEventBus;
    
    /// Shared pointer to the repository core logic component.
    std::shared_ptr<RepositoryManager> repoManager;
    
    /// Unique pointer to the command being tested.
    std::unique_ptr<StatusCommand> command;
    
    /// Path to the temporary test repository directory.
    std::filesystem::path testDir;
};