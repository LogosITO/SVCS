/**
 * @file HelpService.hxx
 * @brief Service for providing help information about commands.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

// Forward declarations
class ISubject;
struct Event;

/**
 * @brief Service that provides help information for commands.
 *
 * This service implements the application logic for displaying help. It uses a set of
 * **callback functions** (`std::function`) to retrieve command data (names, descriptions,
 * usage) and display specific help. This design choice prevents a direct dependency 
 * on the CommandFactory, thereby breaking a potential **circular dependency**.
 */
class HelpService {
private:
    /** @brief Shared pointer to the event bus used for logging or notifying the application. */
    std::shared_ptr<ISubject> eventBus_;
    
    // Callback functions instead of direct CommandFactory dependency
    
    /** @brief Callback function to retrieve a list of all available command names. */
    std::function<std::vector<std::string>()> getCommandsCallback_;
    
    /** @brief Callback function to retrieve the brief description of a specified command. */
    std::function<std::string(const std::string&)> getDescriptionCallback_;
    
    /** @brief Callback function to execute the full help output logic for a specified command. */
    std::function<void(const std::string&)> showHelpCallback_;
    
    /** @brief Callback function to retrieve the usage syntax for a specified command. */
    std::function<std::string(const std::string&)> getUsageCallback_;

public:
    /**
     * @brief Constructs the HelpService by injecting necessary dependencies as callbacks.
     *
     * @param bus Shared pointer to the event bus for notifications.
     * @param getCommands Function (callback) that returns a list of available command names.
     * @param getDescription Function (callback) that returns the brief description of a command.
     * @param showHelp Function (callback) that triggers the full help display for a command.
     * @param getUsage Function (callback) that returns the usage syntax of a command (defaults to nullptr if not strictly needed).
     */
    HelpService(std::shared_ptr<ISubject> bus,
                std::function<std::vector<std::string>()> getCommands,
                std::function<std::string(const std::string&)> getDescription,
                std::function<void(const std::string&)> showHelp,
                std::function<std::string(const std::string&)> getUsage = nullptr);
    
    /**
     * @brief Executes the `getCommandsCallback_` to retrieve the list of all available command names.
     *
     * @return Vector of strings containing the names of all registered commands.
     */
    [[nodiscard]] std::vector<std::string> getAvailableCommands() const;
    
    /**
     * @brief Executes the `getDescriptionCallback_` to retrieve the brief description of a specific command.
     *
     * @param commandName The name of the command.
     * @return The command description, or "Unknown command" if the command name is not valid or the callback fails.
     */
    [[nodiscard]] std::string getCommandDescription(const std::string& commandName) const;
    
    /**
     * @brief Executes the `showHelpCallback_` to trigger the display of detailed help for a specific command.
     *
     * @param commandName The name of the command to show help for.
     */
    void showCommandHelp(const std::string& commandName) const;
    
    /**
     * @brief Checks for the existence of a command by comparing against the list returned by `getAvailableCommands`.
     *
     * @param commandName The name of the command to check.
     * @return \c true if the command is registered, \c false otherwise.
     */
    [[nodiscard]] bool commandExists(const std::string& commandName) const;
    
    /**
     * @brief Executes the `getUsageCallback_` to retrieve the usage syntax of a specific command.
     *
     * @param commandName The name of the command.
     * @return The command usage string, or an empty string if the command is not found or the callback is not set.
     */
    [[nodiscard]] std::string getCommandUsage(const std::string& commandName) const;
};