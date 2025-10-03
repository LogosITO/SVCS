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
 * @brief Test fixture for StatusCommand tests.
 */
class StatusCommandTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    
    void createTestFile(const std::string& filename, const std::string& content);
    void createTestDirectory(const std::string& dirname);
    void stageFiles(const std::vector<std::string>& files);
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    int countMessages(const std::vector<Event>& notifications, const std::string& message);
    
    std::shared_ptr<MockSubject> mockEventBus;
    std::shared_ptr<RepositoryManager> repoManager;
    std::unique_ptr<StatusCommand> command;
    std::filesystem::path testDir;
};