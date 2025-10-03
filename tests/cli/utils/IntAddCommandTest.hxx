/**
 * @file AddCommandTest.hxx
 * @brief Declaration of the test fixture for AddCommand integration tests.
 * This fixture sets up an isolated environment with an initialized SVCS repository
 * and provides helper functions for creating test files and directories.
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../../core/include/RepositoryManager.hxx"
#include "../../../cli/include/AddCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>

/**
 * @brief Test fixture for AddCommand integration tests.
 * Manages the setup and teardown of a temporary, initialized SVCS repository. 
 * Provides utility methods to create various file system structures for testing the 
 * staging logic of the AddCommand.
 * @ingroup IntegrationTests
 */
class AddCommandTest : public ::testing::Test {
protected:
    /** @brief Mock event bus for capturing system notifications. */
    std::shared_ptr<MockSubject> mockEventBus;
    
    /** @brief Real RepositoryManager instance under test. */
    std::shared_ptr<RepositoryManager> repoManager;
    
    /** @brief AddCommand instance under test. */
    std::unique_ptr<AddCommand> command;
    
    /** @brief Path to the temporary directory used as the repository root. */
    std::filesystem::path testDir;

    /**
     * @brief Sets up the test environment.
     * * Creates a unique temporary directory, changes the current path to it, 
     * initializes a repository, and clears initial events.
     */
    void SetUp() override;

    /**
     * @brief Tears down the test environment.
     * * Restores the original current path and recursively removes the temporary directory.
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
};