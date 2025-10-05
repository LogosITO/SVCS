/**
 * @file ICommand.hxx
 * @brief Declaration of the ICommand interface for all executable CLI commands.
 *
 * ICommand defines the basic contract that any command in the system must conform to,
 * ensuring a uniform way to retrieve metadata (name, description, usage)
 * and execute the command's logic.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include <string>
#include <vector>

/**
 * @class ICommand
 * @brief Interface (abstract base class) for all commands executed via the CLI.
 *
 * Every concrete command (e.g., InitCommand, CommitCommand) must inherit
 * this interface and implement all its pure virtual methods.
 */
class ICommand {
public:
    /**
     * @brief Virtual destructor.
     * * Ensures correct cleanup of derived classes via a base class pointer.
     */
    virtual ~ICommand() = default;
    
    /**
     * @brief Retrieves the unique name of the command (e.g., "init").
     * @return The command name as a string.
     */
    [[nodiscard]] virtual std::string getName() const = 0;
    
    /**
     * @brief Retrieves a brief description of what the command does.
     * @return The command's description.
     */
    [[nodiscard]] virtual std::string getDescription() const = 0;
    
    /**
     * @brief Retrieves the command's usage syntax.
     * @return A string with the syntax (e.g., "vcs commit <message>").
     */
    [[nodiscard]] virtual std::string getUsage() const = 0;
    
    /**
     * @brief Executes the main logic of the command.
     * @param args A vector of string arguments passed to the command (excluding the command name itself).
     * @return true if the command execution was successful; false on error.
     */
    virtual bool execute(const std::vector<std::string>& args) = 0;
    
    /**
     * @brief Displays detailed help information about the command (Usage and Description).
     */
    virtual void showHelp() const = 0;
};