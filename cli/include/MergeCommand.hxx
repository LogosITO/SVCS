/**
 * @file MergeCommand.hxx
 * @brief Declaration of the MergeCommand for merging branches.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "ICommand.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <string>
#include <vector>
#include <memory>

/**
 * @class MergeCommand
 * @brief Command for merging branches in the version control system.
 */
class MergeCommand : public ICommand {
public:
    /**
     * @brief Constructs a MergeCommand with event bus and repository manager.
     * @param event_bus The event bus for notifications
     * @param repo_manager The repository manager for merge operations
     */
    MergeCommand(std::shared_ptr<ISubject> event_bus, 
                 std::shared_ptr<RepositoryManager> repo_manager);
    
    ~MergeCommand() override = default;
    
    // ICommand interface implementation
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] std::string getDescription() const override;
    [[nodiscard]] std::string getUsage() const override;
    bool execute(const std::vector<std::string>& args) override;
    void showHelp() const override;
    
private:
    void debugBranchHistory(const std::string& branch_name, const std::string& head_commit) const;

    /**
     * @brief Merges a branch into the current branch.
     * @param branch_name Name of the branch to merge
     * @return true if successful, false otherwise
     */
    bool mergeBranch(const std::string& branch_name) const;
    
    /**
     * @brief Aborts an ongoing merge operation.
     * @return true if successful, false otherwise
     */
    bool abortMerge() const;
    
    /**
     * @brief Checks if a merge is in progress.
     * @return true if merge is in progress, false otherwise
     */
    [[nodiscard]] bool isMergeInProgress() const;
    
    /**
     * @brief Finds the common ancestor of two commits.
     * @param commit1 First commit hash
     * @param commit2 Second commit hash
     * @return Common ancestor commit hash
     */
    std::string findCommonAncestor(const std::string& commit1, const std::string& commit2) const;
    
    /**
     * @brief Performs a three-way merge.
     * @param ancestor Common ancestor commit
     * @param current Current branch commit
     * @param other Other branch commit to merge
     * @return true if merge successful, false if conflicts
     */
    bool performThreeWayMerge(const std::string& ancestor, 
                             const std::string& current, 
                             const std::string& other) const;
    
    /**
     * @brief Detects merge conflicts between file versions.
     * @param ancestor_content Ancestor file content
     * @param current_content Current branch file content
     * @param other_content Other branch file content
     * @param merged_content Output merged content
     * @return true if no conflicts, false if conflicts detected
     */
    static bool mergeFileContent(const std::string& ancestor_content,
                         const std::string& current_content,
                         const std::string& other_content,
                         std::string& merged_content) ;
    
    std::shared_ptr<ISubject> event_bus_;
    std::shared_ptr<RepositoryManager> repo_manager_;
};