/**
 * @file HelpCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Unit tests for the HelpCommand class.
 * @details This file implements test cases for the HelpCommandTest fixture,
 * focusing on ensuring the command correctly interacts with the mock help service
 * to provide general help, specific command help, and handles various edge cases
 * like multiple arguments, missing help service dependency, and non-existent commands.
 *
 * @russian
 * @brief Модульные тесты для класса HelpCommand.
 * @details Этот файл реализует тестовые случаи для фикстура HelpCommandTest,
 * фокусируясь на обеспечении корректного взаимодействия команды с mock сервисом справки
 * для предоставления общей справки, справки по конкретным командам и обработки различных
 * граничных случаев, таких как множественные аргументы, отсутствие зависимости от сервиса
 * справки и несуществующие команды.
 */

#include "utils/IntHelpCommandTest.hxx"
#include "mocks/MockHelpServiceAdapter.hxx"

/**
 * @namespace svcs::test::cli
 * @brief Unit tests for command-line interface components.
 */
namespace svcs::test::cli {

using namespace svcs::test::cli::mocks;
using namespace svcs::test::cli::utils;

using ::svcs::cli::HelpCommand;
using ::svcs::services::Event;

TEST_F(HelpCommandTest, ShowGeneralHelp) {
    // Setup mock help service to return available commands
    mockHelpService->setAvailableCommands({"init", "add", "save", "status", "history"});
    mockHelpService->setCommandDescription("init", "Initialize a new repository");
    mockHelpService->setCommandDescription("add", "Add files to staging area");
    mockHelpService->setCommandDescription("save", "Save staged changes");
    mockHelpService->setCommandDescription("status", "Show repository status");
    mockHelpService->setCommandDescription("history", "Show commit history");

    std::vector<std::string> args = {};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();

    // Should show general help information
    EXPECT_TRUE(containsMessage(notifications, "SVCS - Simple Version Control System"));
    EXPECT_TRUE(containsMessage(notifications, "Usage: svcs <command>"));
    EXPECT_TRUE(containsMessage(notifications, "Available commands:"));
    EXPECT_TRUE(containsMessage(notifications, "init - Initialize a new repository"));
    EXPECT_TRUE(containsMessage(notifications, "add - Add files to staging area"));
    EXPECT_TRUE(containsMessage(notifications, "Global options:"));
    EXPECT_TRUE(containsMessage(notifications, "--version, -v"));
    EXPECT_TRUE(containsMessage(notifications, "--help, -h"));
}

// Test help for specific command
TEST_F(HelpCommandTest, ShowCommandHelp) {
    std::vector<std::string> args = {"init"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    // Should call help service for specific command
    EXPECT_TRUE(mockHelpService->wasCommandHelpCalled("init"));
    EXPECT_FALSE(mockHelpService->wasCommandHelpCalled("add"));
}

// Test help for multiple specific commands (only first is used)
TEST_F(HelpCommandTest, ShowCommandHelpMultipleArgs) {
    std::vector<std::string> args = {"add", "extra", "arguments"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    // Should only use first argument as command name
    EXPECT_TRUE(mockHelpService->wasCommandHelpCalled("add"));
}

// Test help command's own help
TEST_F(HelpCommandTest, ShowHelpCommandHelp) {
    command->showHelp();

    auto notifications = mockEventBus->getNotifications();

    EXPECT_TRUE(containsMessage(notifications, "Usage: svcs help [command]"));
    EXPECT_TRUE(containsMessage(notifications, "Show help information for commands"));
    EXPECT_TRUE(containsMessage(notifications, "If no command is specified, shows general help"));
    EXPECT_TRUE(containsMessage(notifications, "svcs help init"));
}

// Test error when help service is not available for general help
TEST_F(HelpCommandTest, ErrorWhenHelpServiceNotAvailableGeneral) {
    // Create command without help service
    auto commandWithoutService = std::make_unique<HelpCommand>(mockEventBus, nullptr);

    std::vector<std::string> args = {};
    bool result = commandWithoutService->execute(args);

    EXPECT_TRUE(result); // Should still return true

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Help service not available"));
}

// Test error when help service is not available for command help
TEST_F(HelpCommandTest, ErrorWhenHelpServiceNotAvailableCommand) {
    // Create command without help service
    auto commandWithoutService = std::make_unique<HelpCommand>(mockEventBus, nullptr);

    std::vector<std::string> args = {"init"};
    bool result = commandWithoutService->execute(args);

    EXPECT_TRUE(result); // Should still return true

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Help service not available"));
}

// Test command description and usage
TEST_F(HelpCommandTest, CommandDescriptionAndUsage) {
    EXPECT_EQ(command->getDescription(), "Show help information for commands");
    EXPECT_EQ(command->getUsage(), "svcs help [command]");
}

// Test that source is correctly set to "help"
TEST_F(HelpCommandTest, CorrectSource) {
    std::vector<std::string> args = {};
    command->execute(args);

    auto notifications = mockEventBus->getNotifications();

    // All notifications should have source "help"
    for (const auto& event : notifications) {
        EXPECT_EQ(event.source_name, "help");
    }
}

// Test empty command list from help service
TEST_F(HelpCommandTest, EmptyCommandList) {
    // Setup empty command list
    mockHelpService->setAvailableCommands({});

    std::vector<std::string> args = {};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();

    // Should still show general help structure
    EXPECT_TRUE(containsMessage(notifications, "Available commands:"));
    EXPECT_TRUE(containsMessage(notifications, "SVCS - Simple Version Control System"));
}

// Test help for non-existent command
TEST_F(HelpCommandTest, HelpForNonExistentCommand) {
    std::vector<std::string> args = {"nonexistent"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    // Should still call help service (it will handle the error)
    EXPECT_TRUE(mockHelpService->wasCommandHelpCalled("nonexistent"));
}

// Test multiple help commands in sequence
TEST_F(HelpCommandTest, MultipleHelpCommands) {
    // First: general help
    std::vector<std::string> args1 = {};
    bool result1 = command->execute(args1);
    EXPECT_TRUE(result1);

    mockEventBus->clear();
    mockHelpService->clear();

    // Second: specific command help
    std::vector<std::string> args2 = {"save"};
    bool result2 = command->execute(args2);
    EXPECT_TRUE(result2);

    // Third: another specific command
    std::vector<std::string> args3 = {"status"};
    bool result3 = command->execute(args3);
    EXPECT_TRUE(result3);

    EXPECT_TRUE(mockHelpService->wasCommandHelpCalled("save"));
    EXPECT_TRUE(mockHelpService->wasCommandHelpCalled("status"));
}

} // namespace svcs::test::cli