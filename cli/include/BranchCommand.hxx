/**
 * @file include/commands/BranchCommand.hxx
 * @brief Declaration of the BranchCommand class for managing branches.
 *
 * @details The BranchCommand is the interface for the 'svcs branch' command. 
 * It supports various branch operations, including listing existing branches, 
 * creating, deleting, renaming, and switching branches. It relies on the 
 * BranchManager for core logic and the ISubject event bus for user communication.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
#pragma once

#include "ICommand.hxx"
#include "../../core/include/BranchManager.hxx"
#include "../../services/ISubject.hxx"

#include <string>
#include <vector>
#include <memory>

/**
 * @class BranchCommand
 * @brief Command for managing branches in the version control system.
 *
 * @ingroup CLI
 *
 * @details Implements the 'svcs branch' functionality, providing an interface 
 * for the user to interact with the repository's branch structure.
 */
class BranchCommand : public ICommand {
public:
    /**
     * @brief Constructs a BranchCommand.
     * @param event_bus Shared pointer to the event bus for communication (output, error reporting).
     * @param branch_manager Shared pointer to the core BranchManager for branch operations.
     */
    BranchCommand(std::shared_ptr<ISubject> event_bus, 
                  std::shared_ptr<BranchManager> branch_manager);
    
    /**
     * @brief Default destructor.
     */
    ~BranchCommand() override = default;
    
    // --- ICommand interface implementation ---
    
    /**
     * @brief Gets the command name.
     * @return The string "branch".
     */
    std::string getName() const override;

    /**
     * @brief Gets a short description of the command's function.
     * @return A string describing branch management capabilities.
     */
    std::string getDescription() const override;

    /**
     * @brief Gets the usage syntax of the command.
     * @return A string detailing how the command is used (e.g., `svcs branch <name>`).
     */
    std::string getUsage() const override;

    /**
     * @brief Executes the branch command based on the provided arguments.
     * @details Parses arguments to determine the specific branch operation (list, create, delete, switch, rename).
     * @param args The command-line arguments.
     * @return \c true on successful execution, \c false on error.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @brief Displays detailed help information for the command.
     */
    void showHelp() const override;
    
private:
    // --- Private Operation Handlers ---

    /**
     * @brief Lists all existing branches, marking the current branch.
     * @return \c true on success, \c false otherwise.
     */
    bool listBranches();

    /**
     * @brief Creates a new branch.
     * @param branch_name The name of the new branch to create.
     * @return \c true if the branch was created successfully, \c false otherwise.
     */
    bool createBranch(const std::string& branch_name);

    /**
     * @brief Deletes an existing branch.
     * @param branch_name The name of the branch to delete.
     * @param force If \c true, forces the deletion (e.g., skips unmerged check).
     * @return \c true if the branch was deleted successfully, \c false otherwise.
     */
    bool deleteBranch(const std::string& branch_name, bool force = false);

    /**
     * @brief Renames an existing branch.
     * @param old_name The current name of the branch.
     * @param new_name The new name for the branch.
     * @return \c true if the branch was renamed successfully, \c false otherwise.
     */
    bool renameBranch(const std::string& old_name, const std::string& new_name);

    /**
     * @brief Displays the name of the currently active branch.
     * @return \c true on success, \c false otherwise.
     */
    bool showCurrentBranch();

    /**
     * @brief Switches the current working branch to the specified name.
     * @param branch_name The name of the branch to switch to.
     * @return \c true if the switch was successful, \c false otherwise.
     */
    bool switchBranch(const std::string& branch_name);
    
    // --- Private Utility Wrappers ---
    
    /**
     * @brief Checks if a name is valid for a branch (wrapper for BranchManager::isValidBranchName).
     * @param name The name to validate.
     * @return \c true if the name is valid, \c false otherwise.
     */
    bool isValidBranchName(const std::string& name) const;
    
    /**
     * @brief Checks if a branch with the given name exists (wrapper for BranchManager::branchExists).
     * @param name The name of the branch to check.
     * @return \c true if the branch exists, \c false otherwise.
     */
    bool branchExists(const std::string& name) const;
    
    /// @brief Shared pointer to the event bus for logging and user output.
    std::shared_ptr<ISubject> event_bus_;

    /// @brief Shared pointer to the BranchManager for core branch logic.
    std::shared_ptr<BranchManager> branch_manager_;
};