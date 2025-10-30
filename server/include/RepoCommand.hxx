/**
 * @file RepoCommand.hxx
 * @brief Declaration of the RepoCommand class.
 *
 * @details RepoCommand implements the ICommand interface to manage repository-level
 * configurations, primarily focusing on managing **remotes** (удаленные репозитории).
 *
 * This command acts as a facade, delegating the actual management of remotes
 * to the @ref RemoteManager class after ensuring the command is executed within
 * a valid repository context (implicitly handled by @ref RepositoryManager).
 *
 * The command supports subcommands like `add`, `remove`, `list`, and `rename`.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../core/include/Repository.hxx"
#include "../../services/ISubject.hxx"
#include "RemoteManager.hxx"
#include <memory>
#include <vector>

/**
 * @class RepoCommand
 * @brief Command handler for managing repository remotes and configurations.
 *
 * @details Implements the "svcs repo" command, providing subcommands for
 * managing the list of remote repositories associated with the current project.
 */
class RepoCommand : public ICommand {
private:
    std::shared_ptr<ISubject> event_bus_;           ///< Event bus for sending notifications (info, error, success).
    std::shared_ptr<RepositoryManager> repo_manager_; ///< Manager to locate and access the current repository.

public:
    /**
     * @brief Constructs a RepoCommand.
     *
     * @param event_bus The application's event bus.
     * @param repo_manager The repository locator and manager.
     */
    RepoCommand(std::shared_ptr<ISubject> event_bus,
                std::shared_ptr<RepositoryManager> repo_manager);

    std::string getName() const override;
    std::string getDescription() const override;
    std::string getUsage() const override;

    /**
     * @brief Executes the RepoCommand logic.
     *
     * @param args The list of arguments, where args[0] is expected to be a subcommand (e.g., "add", "remove").
     * @return bool True if the command executes successfully, false otherwise.
     *
     * @details Finds the current repository and delegates the task based on the subcommand.
     */
    bool execute(const std::vector<std::string>& args) override;

    void showHelp() const override;

private:
    // --- Subcommand Handlers ---

    /**
     * @brief Handles the "repo add" subcommand.
     *
     * @param remote_manager The manager instance for remotes in the current repo.
     * @param args Arguments containing the remote name and URL. Expected format: `{"add", <name>, <url>}`.
     * @return bool True on successful addition, false otherwise.
     */
    bool handleAdd(RemoteManager& remote_manager, const std::vector<std::string>& args);

    /**
     * @brief Handles the "repo remove" subcommand.
     *
     * @param remote_manager The manager instance for remotes.
     * @param args Arguments containing the remote name. Expected format: `{"remove", <name>}`.
     * @return bool True on successful removal, false otherwise.
     */
    bool handleRemove(RemoteManager& remote_manager, const std::vector<std::string>& args);

    /**
     * @brief Handles the "repo list" subcommand.
     *
     * @param remote_manager The manager instance for remotes.
     * @return bool Always true, lists all configured remotes.
     */
    bool handleList(RemoteManager& remote_manager);

    /**
     * @brief Handles the "repo rename" subcommand (Placeholder).
     *
     * @param remote_manager The manager instance for remotes.
     * @param args Arguments containing the old and new remote name. Expected format: `{"rename", <old_name>, <new_name>}`.
     * @return bool True on successful rename, false otherwise.
     *
     * @note Renaming a remote requires removing the old one and adding a new one,
     * potentially updating refspecs if they were manually configured.
     */
    bool handleRename(RemoteManager& remote_manager, const std::vector<std::string>& args);
};