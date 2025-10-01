/**
 * @file InitCommandIntegrationTest.hxx
 * @brief Declaration of the test fixture for InitCommand integration tests.
 * * This fixture sets up an isolated environment with a temporary directory 
 * and uses the real RepositoryManager to verify end-to-end repository initialization.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once 

#include "../../../core/include/RepositoryManager.hxx"
#include "../../../cli/include/InitCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>

/**
 * @brief Test fixture for InitCommand integration tests.
 * * Manages the setup and teardown of a temporary, isolated directory on the filesystem 
 * for each test case. It uses the real RepositoryManager to test actual repository 
 * creation logic and a MockSubject to verify event publications.
 */
class InitCommandIntegrationTest : public ::testing::Test {
protected:
    /** @brief Mock event bus for capturing system notifications. */
    std::shared_ptr<MockSubject> mockEventBus;
    
    /** @brief Real RepositoryManager instance under test. */
    std::shared_ptr<RepositoryManager> repoManager;
    
    /** @brief InitCommand instance under test. */
    std::unique_ptr<InitCommand> command;
    
    /** @brief Path to the temporary directory created for the test. */
    std::filesystem::path testDir;

    /**
     * @brief Sets up the test environment.
     * * Creates a unique temporary directory, changes the current path to it, 
     * and initializes the RepositoryManager and InitCommand.
     */
    void SetUp() override;

    /**
     * @brief Tears down the test environment.
     * * Restores the original current path and recursively removes the temporary directory.
     */
    void TearDown() override;
};