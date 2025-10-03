/**
 * @file SaveCommandTest.hxx
 * @brief Unit tests for SaveCommand class.
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "../../../cli/include/SaveCommand.hxx"
#include "../mocks/MockSubject.hxx"

/**
 * @class SaveCommandTest
 * @brief Test fixture for SaveCommand tests.
 */
class SaveCommandTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    
    void createTestFile(const std::string& filename, const std::string& content);
    void createTestDirectory(const std::string& dirname);
    void stageFiles(const std::vector<std::string>& files);
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    
    std::shared_ptr<MockSubject> mockEventBus;
    std::shared_ptr<RepositoryManager> repoManager;
    std::unique_ptr<SaveCommand> command;
    std::filesystem::path testDir;
};