/**
 * @file BranchCommand.hxx
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
    BranchCommand(std::shared_ptr<ISubject> event_bus, 
                  std::shared_ptr<BranchManager> branch_manager);
    
    ~BranchCommand() override = default;
    
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] std::string getDescription() const override;
    [[nodiscard]] std::string getUsage() const override;
    bool execute(const std::vector<std::string>& args) override;
    void showHelp() const override;
    
private:
    bool listBranches() const;
    bool createBranch(const std::string& branch_name) const;
    bool createBranchFromCommit(const std::string& branch_name, const std::string& commit_hash) const; // ← ОСТАВЛЯЕМ только этот
    bool deleteBranch(const std::string& branch_name, bool force = false) const;
    bool renameBranch(const std::string& old_name, const std::string& new_name) const;
    bool showCurrentBranch() const;
    bool switchBranch(const std::string& branch_name) const;
    
    [[nodiscard]] static bool isValidBranchName(const std::string& name) ;
    [[nodiscard]] bool branchExists(const std::string& name) const;
    [[nodiscard]] static bool isValidCommitHash(const std::string& hash) ; // ← ДОБАВЛЯЕМ
    
    std::shared_ptr<ISubject> event_bus_;
    std::shared_ptr<BranchManager> branch_manager_;
};