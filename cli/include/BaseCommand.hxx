/**
 * @file BaseCommand.hxx
 * @brief Declaration of the BaseCommand class for all commands in the system.
 *
 * BaseCommand implements the logic for interacting with the Event Bus system,
 * allowing all derived commands to easily generate informational, debug,
 * and other messages that will be automatically processed by registered
 * observers (e.g., Logger).
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "ICommand.hxx"
#include "../../core/include/Repository.hxx"
// Note: Assumes ISubject, IObserver, and Event are available via other includes.

#include <memory>
#include <string>
#include <iostream>

// Forward declaration of ISubject is necessary if not already included
class ISubject; 

/**
 * @class BaseCommand
 * @brief Base class for all CLI commands.
 *
 * Provides a common infrastructure for interacting with the Event Bus system (ISubject)
 * and utility methods for working with the repository. It is responsible for logging
 * via the Event Bus.
 */
class BaseCommand : public ICommand {
protected:
    /**
     * @brief The Event Subject (Event Bus) instance.
     *
     * Used to generate events (logs) that will be received by observers, such as the Logger.
     */
    std::shared_ptr<ISubject> eventSubject_; 

    /**
     * @brief Finds an existing repository in the current or parent directories.
     * @return A smart pointer to the found Repository or nullptr if no repository is found.
     */
    std::shared_ptr<Repository> findRepository() const {
        return Repository::findRepository();
    }
    
    /**
     * @brief Creates a new repository instance.
     * @param path The path where the repository should be created (defaults to '.').
     * @return A smart pointer to the new Repository.
     */
    std::shared_ptr<Repository> createRepository(const std::string& path = ".") const {
        return std::make_shared<Repository>(path);
    }

public:
    /**
     * @brief Constructor.
     * @param subject The ISubject (Event Bus) instance that will be used for generating events and logs.
     */
    BaseCommand(std::shared_ptr<ISubject> subject) : eventSubject_(std::move(subject)) {}
    
    /**
     * @brief Virtual destructor.
     * * Ensures correct cleanup of derived classes.
     */
    virtual ~BaseCommand() = default;

    /**
     * @brief Gets the command name (e.g., "init", "commit").
     * @return The command name as a string.
     */
    virtual std::string getName() const override = 0;
    
    /**
     * @brief Gets a brief description of the command.
     * @return The command's description.
     */
    virtual std::string getDescription() const override = 0;
    
    /**
     * @brief Gets the command's usage syntax.
     * @return A string with the syntax (e.g., "vcs init <path>").
     */
    virtual std::string getUsage() const override = 0;
    
    /**
     * @brief Displays the command's help (Usage and Description).
     * * This method outputs information directly to the standard output stream (stdout).
     */
    void showHelp() const override {
        std::cout << "Usage: " << getUsage() << std::endl;
        std::cout << "Description: " << getDescription() << std::endl;
    }
    
    /**
     * @brief Generates a debug information event (DEBUG_MESSAGE).
     * @param message The text of the debug message.
     */
    void logDebug(const std::string& message) const {
        if (eventSubject_) {
            // Note: Assuming Event takes (Type, Message, Source/Command Name)
            eventSubject_->notify({Event::DEBUG_MESSAGE, message, getName()});
        }
    }
    
    /**
     * @brief Generates a general informational event (GENERAL_INFO).
     * @param message The text of the informational message.
     */
    void logInfo(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::GENERAL_INFO, message, getName()});
        }
    }
    
    /**
     * @brief Generates a runtime error event (RUNTIME_ERROR).
     * @param message The text of the error message.
     */
    void logError(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::RUNTIME_ERROR, message, getName()});
        }
    }
    
    /**
     * @brief Generates a successful execution event (GENERAL_INFO with SUCCESS prefix).
     * @param message The text of the successful execution message.
     */
    void logSuccess(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::GENERAL_INFO, "SUCCESS: " + message, getName()});
        }
    }
    
    /**
     * @brief Generates a warning event (RUNTIME_ERROR with WARNING prefix).
     * @param message The text of the warning message.
     */
    void logWarning(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::RUNTIME_ERROR, "WARNING: " + message, getName()});
        }
    }
};