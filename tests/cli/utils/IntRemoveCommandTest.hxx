/**
 * @file IntRemoveCommandTest.hxx
 * @brief Integration test fixture for RemoveCommand.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../../cli/include/RemoveCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include <string>

class RemoveCommandTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    
    void createTestFile(const std::string& filename, const std::string& content);
    void createTestDirectory(const std::string& dirname);
    void stageFiles(const std::vector<std::string>& files);
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);

    std::shared_ptr<MockSubject> mockEventBus;
    std::shared_ptr<RepositoryManager> repoManager;
    std::unique_ptr<RemoveCommand> command;
    std::filesystem::path testDir;
};