/**
 * @file IntHelpCommandTest.cxx
 * @brief Implementation of integration tests for the HelpCommand class.
 *
 * @details This file provides the concrete implementation for the HelpCommandTest fixture. 
 * It sets up the necessary dependencies: the MockSubject (event bus) for capturing output 
 * and the MockHelpService for controlling the command's information and verifying 
 * delegation calls.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
 
#include "IntHelpCommandTest.hxx"

#include <algorithm>

void HelpCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->mockHelpService = std::make_shared<MockHelpService>(mockEventBus);
    this->command = std::make_unique<HelpCommand>(this->mockEventBus, this->mockHelpService);
    this->mockEventBus->clear();
}

void HelpCommandTest::TearDown() {
    mockEventBus->clear();
    mockHelpService->clear();
}

bool HelpCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

int HelpCommandTest::countMessages(const std::vector<Event>& notifications, const std::string& message) {
    return std::count_if(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}