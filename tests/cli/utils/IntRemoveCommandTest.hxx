/**
 * @file IntRemoveCommandTest.hxx
 * @brief Test fixture header for integration tests of the RemoveCommand class.
 *
 * @details Declares the RemoveCommandTest fixture, which sets up a controlled 
 * environment for testing the 'svcs remove' (or 'svcs rm') command. This fixture 
 * manages a temporary SVCS repository and provides helper functions for file 
 * manipulation, staging, and output verification.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../../cli/include/RemoveCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include <string>

/**
 * @class RemoveCommandTest
 * @brief Google Test fixture for running integration tests against the RemoveCommand.
 *
 * @ingroup IntegrationTests
 *
 * @details This fixture provides the necessary infrastructure for testing the 
 * file removal command, ensuring it correctly stages files for removal from the 
 * repository, handles errors, and respects command line options.
 */
class RemoveCommandTest : public ::testing::Test {
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
     * @brief Stages a list of files (using the add command logic for convenience) 
     * in the RepositoryManager's staging area.
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
    
    /// Shared pointer to the mock event bus (observer).
    std::shared_ptr<MockSubject> mockEventBus;
    
    /// Shared pointer to the repository core logic component.
    std::shared_ptr<RepositoryManager> repoManager;
    
    /// Unique pointer to the command being tested.
    std::unique_ptr<RemoveCommand> command;
    
    /// Path to the temporary test repository directory.
    std::filesystem::path testDir;
};