/**
 * @file InitCommand.hxx
 * @brief Declaration of the InitCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Implements the "init" command for the Version Control System (VCS).
 *
 * This command is responsible for initializing a new SVCS repository in the
 * current directory or a specified location, setting up the necessary
 * internal structure (e.g., the hidden .svcs directory).
 *
 * The class inherits from ICommand, ensuring it adheres to the Command pattern
 * interface. It requires an event bus for communication and a repository manager
 * to perform the actual initialization logic.
 */
class InitCommand : public ICommand {
private:
    /** @brief Shared pointer to the event bus used for application-wide notifications. */
    std::shared_ptr<ISubject> eventBus_;
    
    /** @brief Shared pointer to the RepositoryManager responsible for repository operations. */
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Constructs an InitCommand.
     * @param subject A shared pointer to the event bus (ISubject).
     * @param repoManager A shared pointer to the RepositoryManager.
     */
    InitCommand(std::shared_ptr<ISubject> subject, 
                std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the repository initialization logic.
     * * This method attempts to create the repository structure.
     * @param args A vector of strings containing command-line arguments (usually empty for 'init').
     * @return \c true if the repository was successfully initialized or if it already exists, 
     * \c false if the initialization failed.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Returns the command name.
     * @return The string "init".
     */
    std::string getName() const override { return "init"; }
    
    /**
     * @brief Returns a brief description of the command's purpose.
     * @return A string containing the description.
     */
    std::string getDescription() const override;
    
    /**
     * @brief Returns the usage syntax for the command.
     * @return A string containing the usage: "svcs init".
     */
    std::string getUsage() const override;
    
    /**
     * @brief Displays detailed help information about the command via the event bus.
     */
    void showHelp() const override;
};