/**
 * @file IntHelpCommandTest.hxx
 * @brief Test fixture header for integration tests of the HelpCommand class.
 *
 * @details Declares the HelpCommandTest fixture, which sets up the mock event 
 * bus and the mock help service necessary to verify the command's delegation 
 * logic and output for both general and command-specific help requests.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

// Assuming the relative paths are correct for your project structure
#include "../../../cli/include/HelpCommand.hxx"
#include "../mocks/MockSubject.hxx"
#include "../mocks/MockHelpService.hxx"

// Forward declaration for the Event structure used in helpers
struct Event; 

/**
 * @class HelpCommandTest
 * @brief Google Test fixture for running unit/integration tests against the HelpCommand.
 *
 * @ingroup UnitTests
 *
 * @details This fixture is crucial for isolating the HelpCommand's logic, ensuring 
 * it correctly delegates help requests to the **MockHelpService** and reports 
 * the resulting output via the **MockSubject** (event bus).
 */
class HelpCommandTest : public ::testing::Test {
protected:
    /**
     * @brief Sets up the testing environment before each test.
     * @details Initializes the MockSubject, MockHelpService, and the HelpCommand instance.
     */
    void SetUp() override;

    /**
     * @brief Tears down the testing environment after each test.
     * @details Clears the event bus and the mock help service's internal state.
     */
    void TearDown() override;
    
    /**
     * @brief Checks if a specific message (substring) exists in any event notification.
     * @param notifications The list of captured events.
     * @param message The substring to search for.
     * @return true if the message is found, false otherwise.
     */
    static bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    
    /**
     * @brief Counts the occurrences of a specific message (substring) across all event notifications.
     * @param notifications The list of captured events.
     * @param message The substring to count.
     * @return The number of events containing the substring.
     */
    int countMessages(const std::vector<Event>& notifications, const std::string& message);
    
    /// Shared pointer to the mock event bus (observer) used to capture command output.
    std::shared_ptr<MockSubject> mockEventBus;
    
    /// Shared pointer to the mock help service, used to control help data and verify calls.
    std::shared_ptr<MockHelpService> mockHelpService;
    
    /// Unique pointer to the command being tested.
    std::unique_ptr<HelpCommand> command;
};