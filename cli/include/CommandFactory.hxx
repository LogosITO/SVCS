/**
 * @file CommandFactory.hxx
 * @brief Declaration of the CommandFactory class, which implements the Factory pattern for commands.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../include/ICommand.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

/**
 * @brief Factory for creating ICommand objects by their string name.
 *
 * The CommandFactory manages the lifecycle of commands, creating them dynamically
 * using creator functions registered under unique names. Commands are constructed
 * with shared ownership of an event bus (@ref ISubject) and a repository manager
 * (@ref RepositoryManager).
 */
class CommandFactory {
private:
    /** @brief Shared pointer to the event bus used for notifications (e.g., errors or info). */
    std::shared_ptr<ISubject> event_bus;
    
    /** @brief Shared pointer to the repository manager used for executing SVCS operations. */
    std::shared_ptr<RepositoryManager> repo_manager;
    
    /** * @brief Map of command creator functions.
     * * Key: The command name (e.g., "add", "commit").
     * Value: A std::function that accepts an ISubject and a RepositoryManager 
     * and returns a unique pointer to a concrete ICommand implementation.
     */
    std::unordered_map<std::string, 
        std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>, 
                                              std::shared_ptr<RepositoryManager>)>> creators;

    /**
     * @brief Registers the set of commands that should be available by default.
     * * This function is typically called in the constructor to pre-configure the Factory.
     */
    void registerDefaultCommands();

public:
    /**
     * @brief Constructs the CommandFactory.
     *
     * @param bus A shared pointer to the event bus.
     * @param repoManager A shared pointer to the repository manager.
     */
    explicit CommandFactory(std::shared_ptr<ISubject> bus, 
                          std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Deletes the copy constructor. The Factory should not be copied.
     */
    CommandFactory(const CommandFactory&) = delete;
    
    /**
     * @brief Deletes the assignment operator. The Factory should not be assigned.
     */
    CommandFactory& operator=(const CommandFactory&) = delete;
    
    /**
     * @brief Registers a creator function for a new command.
     *
     * @param name The string name under which the command will be callable.
     * @param creator The function that creates and returns a new ICommand object.
     */
    void registerCommand(const std::string& name,
                       std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                                             std::shared_ptr<RepositoryManager>)> creator);

    // --- New Method Documentation ---

    /**
     * @brief Retrieves the brief description for a registered command.
     * * To do this, it must create a temporary instance of the command.
     * @param name The name of the command.
     * @return The command's description string, or an error message if the command is not registered.
     */
    std::string getCommandDescription(const std::string& name) const;

    /**
     * @brief Displays the detailed help and usage for a registered command.
     * * This is accomplished by creating a temporary command instance and calling its showHelp method.
     * @param name The name of the command.
     */
    void showCommandHelp(const std::string& name) const;

    // --- Existing Method Documentation (Updated) ---

    /**
     * @brief Creates and returns a command object by its name.
     * * Transfers ownership (via unique_ptr) of the created object to the caller.
     * @param name The name of the command (the key in the creators map).
     * @return std::unique_ptr<ICommand> holding a new command object, or nullptr if the command is not registered.
     */
    std::unique_ptr<ICommand> createCommand(const std::string& name) const;
    
    /**
     * @brief Retrieves a list of names for all currently registered commands.
     * @return A vector of strings containing the names of all registered commands.
     */
    std::vector<std::string> getRegisteredCommands() const;
};