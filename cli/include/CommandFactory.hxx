/**
 * @file CommandFactory.hxx
 * @brief Declaration of the CommandFactory class, updated to work with the EventBus.
 *
 * CommandFactory implements the Singleton pattern for centralized command
 * creation management. It is integrated with the EventBus (ISubject),
 * ensuring that every command created (ICommand) automatically
 * receives access to the event bus for logging purposes.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */

#pragma once

#include "ICommand.hxx"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex> 

/**
 * @class ISubject
 * @brief (Forward Declaration) The interface for the Event Bus.
 */
class ISubject; 

/**
 * @class CommandFactory
 * @brief Factory for creating CLI commands (ICommand), implemented as a Singleton.
 *
 * The factory is responsible for registering creator functions for each command and
 * providing all created commands with access to the EventBus.
 */
class CommandFactory {
private:
    /**
     * @brief Pointer to the Event Bus, necessary for creating commands.
     * * Stored as a shared_ptr to ensure its availability to commands.
     */
    std::shared_ptr<ISubject> eventBus_;
    
    /**
     * @brief Private constructor for the Singleton.
     * * Accepts and stores the EventBus.
     * @param eventBus A pointer to the EventBus.
     */
    CommandFactory(std::shared_ptr<ISubject> eventBus);

    /**
     * @brief Registers the system's default commands (e.g., init).
     */
    void registerDefaultCommands();

    /**
     * @brief Map of command creator functions.
     * * Key: The command's name (string).
     * * Value: A function that accepts std::shared_ptr<ISubject> and returns std::unique_ptr<ICommand>.
     */
    std::unordered_map<std::string,
                      std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>)>> creators_;
    
    /**
     * @brief Flag to ensure thread-safe initialization of the Singleton (std::call_once).
     */
    static std::once_flag initInstanceFlag_;

public:
    /**
     * @brief Deleted copy constructor.
     */
    CommandFactory(const CommandFactory&) = delete;

    /**
     * @brief Deleted assignment operator.
     */
    CommandFactory& operator=(const CommandFactory&) = delete;

    /**
     * @brief Returns the single instance of CommandFactory (Singleton access point).
     * @details Must be called *once* with the EventBus on the first access for initialization. 
     * Subsequent calls may omit the argument. Implements a thread-safe singleton.
     * @param eventBus Pointer to the EventBus. Used only during the first initialization.
     * @return Reference to the single CommandFactory instance.
     */
    static CommandFactory& getInstance(std::shared_ptr<ISubject> eventBus = nullptr);

    /**
     * @brief Registers a new command with the factory.
     * @param name The name of the command used for invoking it.
     * @param creator The function that will be called to create the command;
     * must accept std::shared_ptr<ISubject>.
     */
    void registerCommand(const std::string& name,
                        std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>)> creator);

    /**
     * @brief Creates a new command instance by its name.
     * * The stored EventBus is automatically passed when the creator function is called.
     * @param name The name of the command to create.
     * @return A unique pointer to the created command instance, or nullptr if the command is not found.
     */
    std::unique_ptr<ICommand> createCommand(const std::string& name);
};