/**
 * @file UndoCommand.hxx
 * @brief Declaration of the UndoCommand class for reverting changes.
 *
 * @details The UndoCommand is responsible for reverting the repository's working 
 * directory and history to the state of a previous commit. It supports reverting 
 * the last commit or a specific commit identified by its hash. This operation 
 * typically involves confirming the action before execution, unless a force flag 
 * is used.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <string>
#include <vector>

/**
 * @class UndoCommand
 * @brief Command to revert the repository state to a previous commit.
 * * @ingroup CLI
 *
 * @details Implements the 'svcs undo' functionality. It uses the RepositoryManager 
 * to perform the actual history and working directory manipulation. It utilizes 
 * the ISubject (event bus) for user output and confirmation prompts.
 */
class UndoCommand : public ICommand {
public:
    /**
     * @brief Constructor for UndoCommand.
     * @param subject Shared pointer to the event bus for communication (output, prompts).
     * @param repoManager Shared pointer to the RepositoryManager for core repository operations.
     */
    UndoCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the undo command based on the provided arguments.
     * @details Parses arguments for a specific commit hash or the `--force` flag. 
     * If no hash is provided, it attempts to undo the last commit.
     * @param args The command-line arguments (e.g., commit hash, --force).
     * @return true on successful execution (or if help is shown), false on error.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @brief Gets the command's primary name.
     * @return The string "undo".
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @brief Gets the short description of the command.
     * @return A string describing the command's function.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @brief Gets the usage syntax of the command.
     * @return A string detailing how the command is used.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @brief Displays detailed help information for the command.
     */
    void showHelp() const override;

private:
    /**
     * @brief Reverts the changes introduced by the last commit in the current branch.
     * @param force Flag to skip confirmation prompt (\c true for forced undo).
     * @return \c true if the undo operation was successful, \c false otherwise.
     */
    bool undoLastCommit(bool force) const;

    /**
     * @brief Reverts the repository state to a specific commit hash.
     * @param commitHash The hash of the commit to revert to.
     * @param force Flag to skip confirmation prompt (\c true for forced undo).
     * @return \c true if the undo operation was successful, \c false otherwise.
     */
    bool undoSpecificCommit(const std::string& commitHash, bool force) const;

    /**
     * @brief Prompts the user for confirmation before performing the undo operation.
     * @details This prompt is skipped if the \c force flag is \c true.
     * @param commitMessage The message of the commit being undone.
     * @param commitHash The hash of the commit being undone.
     * @param force The flag indicating if the confirmation should be skipped.
     * @return \c true if the user confirms or if the operation is forced, \c false otherwise.
     */
    static bool confirmUndo(const std::string& commitMessage, const std::string& commitHash, bool force);

    /**
     * @brief Resets the current repository state to the previous valid commit, discarding all staged and working directory changes.
     * @return \c true if the forced reset was successful, \c false otherwise.
     */
    bool forceResetRepository() const;
    
    /// @brief Shared pointer to the event bus for communication.
    std::shared_ptr<ISubject> event_bus;
    
    /// @brief Shared pointer to the RepositoryManager for core VCS logic.
    std::shared_ptr<RepositoryManager> repo_manager;
};