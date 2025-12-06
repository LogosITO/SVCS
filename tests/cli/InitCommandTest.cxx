/**
 * @file InitCommandIntegrationTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Integration tests for the 'init' command (InitCommand) and the RepositoryManager.
 *
 * @russian
 * @brief Интеграционные тесты для команды 'init' (InitCommand) и RepositoryManager.
 */

#include "utils/InitCommandIntegrationTest.hxx"
#include "mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <memory>

namespace svcs::test::cli {

using namespace svcs::test::cli::utils;

TEST_F(InitCommandIntegrationTest, Success_InitInEmptyDirectory) {
    bool result = command->execute({});

    EXPECT_TRUE(result);

    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs" / "objects"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs" / "refs"));
    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs" / "HEAD"));

    bool foundSuccess = false;
    for (const auto& notification : mockEventBus->notifications) {
        if (notification.type == Event::REPOSITORY_INIT_SUCCESS) {
            foundSuccess = true;
            break;
        }
    }
    EXPECT_TRUE(foundSuccess);
}

TEST_F(InitCommandIntegrationTest, Success_WithForceFlag) {
    command->execute({});

    mockEventBus->notifications.clear();
    bool result = command->execute({"-f"});

    EXPECT_TRUE(result);

    EXPECT_TRUE(std::filesystem::exists(testDir / ".svcs"));
}

TEST_F(InitCommandIntegrationTest, Success_WithCustomPath) {
    std::filesystem::create_directories(testDir / "subdir");

    bool result = command->execute({"subdir"});

    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(testDir / "subdir" / ".svcs"));
}

TEST_F(InitCommandIntegrationTest, HelpFlagShowsHelp) {
    bool result = command->execute({"--help"});

    EXPECT_TRUE(result);

    bool foundHelp = false;
    for (const auto& notification : mockEventBus->notifications) {
        if (notification.details.find("Usage:") != std::string::npos) {
            foundHelp = true;
            break;
        }
    }
    EXPECT_TRUE(foundHelp);
}

}