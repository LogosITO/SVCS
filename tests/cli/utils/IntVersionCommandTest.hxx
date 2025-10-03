/**
 * @file IntVersionCommandTest.hxx
 * @brief Test fixture header for integration tests of the VersionCommand class.
 *
 * @details Declares the VersionCommandTest fixture, which sets up the mock event 
 * bus and the command instance necessary to verify the output and behavior of 
 * the 'svcs version' command. Since the command is stateless, the setup is minimal.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "../../../cli/include/VersionCommand.hxx"
#include "../mocks/MockSubject.hxx"

/**
 * @class VersionCommandTest
 * @brief Google Test fixture for running unit/integration tests against the VersionCommand.
 *
 * @ingroup IntegrationTests
 *
 * @details This fixture provides the necessary infrastructure for testing the VersionCommand:
 * - **SetUp/TearDown:** Initialize and clean up the mock event bus and the command object.
 * - **Core Components:** Instances of MockSubject (for capturing output) and the VersionCommand itself.
 * - **Helper Methods:** Simplify checking the captured event notifications for specific content.
 */
class VersionCommandTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    bool containsExactMessage(const std::vector<Event>& notifications, const std::string& message);
    
    std::shared_ptr<MockSubject> mockEventBus;
    std::unique_ptr<VersionCommand> command;
};