/**
 * @file ClearCommandTest.hxx
 * @brief Declaration of the test fixture for ClearCommand integration tests.
 * * This fixture sets up an isolated environment with a temporary SVCS repository 
 * and handles the redirection of standard input (`std::cin`) to simulate user 
 * confirmation during the repository clearing process.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once 

#include "../../../core/include/RepositoryManager.hxx"
#include "../../../cli/include/ClearCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <sstream> // Добавлено для std::stringstream

/**
 * @brief Test fixture for ClearCommand integration tests.
 * * Manages the setup and teardown of a temporary, initialized SVCS repository. 
 * Provides utility methods to create test files/directories and to simulate 
 * user input required for the 'clear' confirmation prompt.
 * @ingroup IntegrationTests
 */
class ClearCommandTest : public ::testing::Test {
protected:
    /** @brief Mock event bus for capturing system notifications. */
    std::shared_ptr<MockSubject> mockEventBus;
    
    /** @brief Real RepositoryManager instance under test. */
    std::shared_ptr<RepositoryManager> repoManager;
    
    /** @brief ClearCommand instance under test. */
    std::unique_ptr<ClearCommand> command;
    
    /** @brief Path to the temporary directory used as the repository root. */
    std::filesystem::path testDir;
    
    /** @brief String stream used to feed simulated input to std::cin. */
    std::stringstream inputStream;
    
    /** @brief Pointer to the original buffer of std::cin, saved for restoration. */
    std::streambuf* originalCin;

    /**
     * @brief Sets up the test environment.
     * * Creates and initializes a new temporary SVCS repository and saves the original std::cin buffer.
     */
    void SetUp() override;

    /**
     * @brief Tears down the test environment.
     * * Restores the original std::cin buffer and recursively removes the temporary directory.
     */
    void TearDown() override;
    
    /**
     * @brief Creates a test file inside the test directory.
     * @param filename The name of the file to create.
     * @param content The content of the file (defaults to "test content").
     */
    void createTestFile(const std::string& filename, const std::string& content = "test content");
    
    /**
     * @brief Creates a test directory inside the test directory.
     * @param dirname The name of the directory to create.
     */
    void createTestDirectory(const std::string& dirname);
    
    /**
     * @brief Simulates user input by redirecting std::cin to a string stream.
     * @param input The string that will be read as user input.
     */
    void simulateUserInput(const std::string& input);
    
    /**
     * @brief Checks if the .svcs repository directory exists.
     * @return true if the repository exists, false otherwise.
     */
    bool repositoryExists() const;
};