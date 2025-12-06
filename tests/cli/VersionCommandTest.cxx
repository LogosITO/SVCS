/**
 * @file VersionCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Unit tests for the VersionCommand class.
 *
 * @russian
 * @brief Модульные тесты для класса VersionCommand.
 */

#include "utils/IntVersionCommandTest.hxx"

namespace svcs::test::cli {

using svcs::test::cli::utils::VersionCommandTest;

TEST_F(VersionCommandTest, ShowVersion) {
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    // Should show version information
    EXPECT_TRUE(containsMessage(notifications, "SVCS (Simple Version Control System) version"));
    EXPECT_TRUE(containsMessage(notifications, "Build:"));
    EXPECT_TRUE(containsMessage(notifications, "Copyright (c) 2025 LogosITO"));
}

// Test version command with arguments (should be ignored)
TEST_F(VersionCommandTest, ShowVersionWithArguments) {
    std::vector<std::string> args = {"--some-flag", "value", "another"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    // Should still show version information despite arguments
    EXPECT_TRUE(containsMessage(notifications, "SVCS (Simple Version Control System) version"));
    EXPECT_TRUE(containsMessage(notifications, "Build:"));
    EXPECT_TRUE(containsMessage(notifications, "Copyright"));
}

// Test help command
TEST_F(VersionCommandTest, ShowHelp) {
    std::vector<std::string> args = {"--help"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    EXPECT_TRUE(containsMessage(notifications, "Usage: svcs version"));
    EXPECT_TRUE(containsMessage(notifications, "Show version information"));
    EXPECT_TRUE(containsMessage(notifications, "Shows the current version of SVCS"));
    EXPECT_TRUE(containsMessage(notifications, "does not accept any arguments"));
}

// Test exact version string format
TEST_F(VersionCommandTest, VersionStringFormat) {
    std::vector<std::string> args = {};
    command->execute(args);
    
    auto notifications = mockEventBus->getNotifications();
    
    // Check for exact version string
    bool foundVersion = false;
    for (const auto& event : notifications) {
        if (event.details.find("SVCS (Simple Version Control System) version 1.0.0") != std::string::npos) {
            foundVersion = true;
            break;
        }
    }
    EXPECT_TRUE(foundVersion);
}

// Test copyright information
TEST_F(VersionCommandTest, CopyrightInformation) {
    std::vector<std::string> args = {};
    command->execute(args);
    
    auto notifications = mockEventBus->getNotifications();
    
    bool foundCopyright = false;
    for (const auto& event : notifications) {
        if (event.details.find("Copyright (c) 2025 LogosITO. Licensed under MIT License.") != std::string::npos) {
            foundCopyright = true;
            break;
        }
    }
    EXPECT_TRUE(foundCopyright);
}

// Test build information contains expected elements
TEST_F(VersionCommandTest, BuildInformation) {
    std::vector<std::string> args = {};
    command->execute(args);
    
    auto notifications = mockEventBus->getNotifications();
    
    bool foundBuildInfo = false;
    for (const auto& event : notifications) {
        if (event.details.find("Build:") == 0) { // Starts with "Build:"
            foundBuildInfo = true;
            // Should contain compiler and standard info
            EXPECT_TRUE(event.details.find("Compiler:") != std::string::npos);
            EXPECT_TRUE(event.details.find("C++ Standard:") != std::string::npos);
            break;
        }
    }
    EXPECT_TRUE(foundBuildInfo);
}

// Test multiple executions
TEST_F(VersionCommandTest, MultipleExecutions) {
    // First execution
    std::vector<std::string> args1 = {};
    bool result1 = command->execute(args1);
    EXPECT_TRUE(result1);
    
    auto notifications1 = mockEventBus->getNotifications();
    int firstCount = notifications1.size();
    
    mockEventBus->clear();
    
    // Second execution
    std::vector<std::string> args2 = {"--help"};
    bool result2 = command->execute(args2);
    EXPECT_TRUE(result2);
    
    auto notifications2 = mockEventBus->getNotifications();
    int secondCount = notifications2.size();
    
    // Should work multiple times without issues
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_GT(firstCount, 0);
    EXPECT_GT(secondCount, 0);
}

// Test that source is correctly set to "version"
TEST_F(VersionCommandTest, CorrectSource) {
    std::vector<std::string> args = {};
    command->execute(args);
    
    auto notifications = mockEventBus->getNotifications();
    
    // All notifications should have source "version"
    for (const auto& event : notifications) {
        EXPECT_EQ(event.source_name, "version");
    }
}

// Test empty arguments
TEST_F(VersionCommandTest, EmptyArguments) {
    std::vector<std::string> args;
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_FALSE(notifications.empty());
}

// Test command always returns true
TEST_F(VersionCommandTest, AlwaysReturnsTrue) {
    // Test with no args
    std::vector<std::string> args1;
    bool result1 = command->execute(args1);
    EXPECT_TRUE(result1);
    
    mockEventBus->clear();
    
    // Test with help
    std::vector<std::string> args2 = {"--help"};
    bool result2 = command->execute(args2);
    EXPECT_TRUE(result2);
    
    mockEventBus->clear();
    
    // Test with random args
    std::vector<std::string> args3 = {"random", "arguments"};
    bool result3 = command->execute(args3);
    EXPECT_TRUE(result3);
}

}