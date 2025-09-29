/**
 * @file InitCommand.hxx
 * @brief Declaration of the InitCommand class, implementing repository initialization logic.
 *
 * InitCommand inherits from BaseCommand and executes the 'svcs init' command,
 * creating a new repository structure on the disk.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "BaseCommand.hxx"

/**
 * @class InitCommand
 * @brief Class implementing the logic for the 'init' command.
 *
 * This command is responsible for creating a new SVCS repository structure (.svcs)
 * in the specified directory or the current one.
 */
class InitCommand : public BaseCommand {
public:
    /**
     * @brief Retrieves the command's name.
     * @return The string "init".
     */
    std::string getName() const override { return "init"; }

    /**
     * @brief Retrieves the command's description.
     * @return The description "Initialize new repository".
     */
    std::string getDescription() const override { return "Initialize new repository"; }

    /**
     * @brief Retrieves the command's usage syntax.
     * @return The syntax "svcs init [path]".
     */
    std::string getUsage() const override { return "svcs init [path]"; }

    /**
     * @brief Constructor.
     *
     * Passes the EventBus (ISubject) pointer to the BaseCommand base class to
     * enable event logging capabilities.
     *
     * @param subject The ISubject (Event Bus) instance that will be used 
     * for generating events and logs.
     */
    InitCommand(std::shared_ptr<ISubject> subject) : BaseCommand(std::move(subject)) {}

    /**
     * @brief Destructor.
     *
     * Generates a debug message via the Event Bus when the command is destroyed.
     */
    ~InitCommand() {
        logDebug(getName() + " command destroyed");
    }

    /**
     * @brief Executes the repository initialization logic.
     *
     * Determines the initialization path (from arguments or the current directory),
     * checks for an existing repository, and creates a new one.
     *
     * @param args A vector of string arguments passed to the command. It's expected to have at most one 
     * argument—the target path.
     * @return true if the repository was successfully created; false in case of an error (e.g., already exists).
     */
    bool execute(const std::vector<std::string>& args) override;
};