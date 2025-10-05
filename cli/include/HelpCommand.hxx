/**
 * @file HelpCommand.hxx
 * @brief Declaration of the HelpCommand class for displaying command help information.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../include/HelpService.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Command for displaying help information about other commands.
 *
 * The **HelpCommand** is the application's built-in help utility. It can show a general
 * list of all available commands or display detailed usage and descriptions for a specific
 * command provided as an argument. It delegates the retrieval of command information
 * to the **HelpService** to maintain a clean separation of concerns.
 */
class HelpCommand : public ICommand {
private:
    /** @brief Shared pointer to the event bus used for notifications (i.e., printing help messages to the user). */
    std::shared_ptr<ISubject> eventBus_;
    
    /** @brief Shared pointer to the service responsible for retrieving and formatting command help content. */
    std::shared_ptr<HelpService> helpService_;

public:
    /**
     * @brief Constructs the HelpCommand.
     *
     * @param subject Shared pointer to the event bus for logging and output notifications.
     * @param helpService Shared pointer to the help service dependency.
     */
    HelpCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<HelpService> helpService);
    
    /**
     * @brief Executes the help command logic.
     *
     * @param args Command arguments. If the vector is empty, general help is shown.
     * If it contains one command name, detailed help for that command is shown.
     * @return \c true always, as the help command is not expected to encounter a runtime failure.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return The string "help".
     */
    [[nodiscard]] std::string getName() const override { return "help"; }
    
    /**
     * @brief Gets the brief description of the command.
     * @return The string "Show help information for commands".
     */
    [[nodiscard]] std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return The string "svcs help [command]".
     */
    [[nodiscard]] std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this specific command via the event bus.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Shows general help, listing all available commands and their descriptions.
     * * Utilizes the HelpService to retrieve the list of all registered commands.
     */
    void showGeneralHelp() const;
    
    /**
     * @brief Shows detailed usage, description, and explanation for a specific command.
     * * Delegates the task of fetching and displaying the detailed help to the HelpService.
     * @param commandName The name of the command to show help for.
     */
    void showCommandHelp(const std::string& commandName) const;
};