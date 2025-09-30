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
 * This service uses callback functions to avoid direct dependency on CommandFactory
 * and prevent circular dependencies.
 */
class HelpService {
private:
    std::shared_ptr<ISubject> eventBus_;
    
    // Callback functions instead of direct CommandFactory dependency
    std::function<std::vector<std::string>()> getCommandsCallback_;
    std::function<std::string(const std::string&)> getDescriptionCallback_;
    std::function<void(const std::string&)> showHelpCallback_;
    std::function<std::string(const std::string&)> getUsageCallback_;

public:
    /**
     * @brief Constructs the HelpService with callbacks.
     *
     * @param bus Shared pointer to the event bus for notifications.
     * @param getCommands Function to retrieve available command names.
     * @param getDescription Function to retrieve command descriptions.
     * @param showHelp Function to display command help.
     * @param getUsage Function to retrieve command usage.
     */
    HelpService(std::shared_ptr<ISubject> bus,
                std::function<std::vector<std::string>()> getCommands,
                std::function<std::string(const std::string&)> getDescription,
                std::function<void(const std::string&)> showHelp,
                std::function<std::string(const std::string&)> getUsage = nullptr);
    
    /**
     * @brief Gets the list of all available command names.
     *
     * @return Vector of strings containing the names of all registered commands.
     */
    std::vector<std::string> getAvailableCommands() const;
    
    /**
     * @brief Gets the description of a specific command.
     *
     * @param commandName The name of the command.
     * @return The command description, or "Unknown command" if not found.
     */
    std::string getCommandDescription(const std::string& commandName) const;
    
    /**
     * @brief Shows detailed help information for a specific command.
     *
     * @param commandName The name of the command to show help for.
     */
    void showCommandHelp(const std::string& commandName) const;
    
    /**
     * @brief Checks if a command exists.
     *
     * @param commandName The name of the command to check.
     * @return true if the command is registered, false otherwise.
     */
    bool commandExists(const std::string& commandName) const;
    
    /**
     * @brief Gets the usage syntax of a specific command.
     *
     * @param commandName The name of the command.
     * @return The command usage string, or empty string if not found.
     */
    std::string getCommandUsage(const std::string& commandName) const;
};