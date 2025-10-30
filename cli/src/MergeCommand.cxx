/**
 * @file MergeCommand.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the MergeCommand for merging branches.
 *
 * @russian
 * @brief Реализация MergeCommand для слияния веток.
 */

#include "../include/MergeCommand.hxx"

#include <fstream>
#include <set>

MergeCommand::MergeCommand(std::shared_ptr<ISubject> event_bus,
                         std::shared_ptr<RepositoryManager> repo_manager)
    : event_bus_(std::move(event_bus))
    , repo_manager_(std::move(repo_manager)) {}

std::string MergeCommand::getName() const {
    return "merge";
}

std::string MergeCommand::getDescription() const {
    return "Join two development histories together";
}

std::string MergeCommand::getUsage() const {
    return "svcs merge <branch> [--abort]";
}

bool MergeCommand::execute(const std::vector<std::string>& args) {

    if (args.empty()) {
        event_bus_->notify({Event::ERROR_MESSAGE, "Branch name required for merge", "merge"});
        showHelp();
        return false;
    }
    
    // Parse arguments
    bool abort = false;
    std::string branch_name;
    
    for (const auto& arg : args) {
        if (arg == "--abort") {
            abort = true;
        } else if (arg == "--help" || arg == "-h") {
            showHelp();
            return true;
        } else if (!arg.empty() && arg[0] != '-') {
            branch_name = arg;
        }
    }
    
    if (abort) {
        return abortMerge();
    }
    
    if (branch_name.empty()) {
        event_bus_->notify({Event::ERROR_MESSAGE, "Branch name required for merge", "merge"});
        showHelp();
        return false;
    }
    
    return mergeBranch(branch_name);
}

void MergeCommand::showHelp() const {
    event_bus_->notify({Event::HELP_MESSAGE, "Usage: " + getUsage(), "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "Description: " + getDescription(), "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "Options:", "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "  <branch>        Branch to merge into current branch", "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "  --abort         Abort the current merge operation", "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "Examples:", "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs merge feature/login    # Merge feature/login into current branch", "merge"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs merge --abort          # Abort ongoing merge", "merge"});
}

void MergeCommand::debugBranchHistory(const std::string& branch_name, const std::string& head_commit) const {
    event_bus_->notify({Event::ERROR_MESSAGE, "=== DEBUG HISTORY FOR BRANCH: " + branch_name + " ===", "merge"});
    event_bus_->notify({Event::ERROR_MESSAGE, "HEAD: " + head_commit, "merge"});
    
    std::string current = head_commit;
    int step = 0;
    
    while (!current.empty() && step < 20) {
        // Try to read commit object
        std::filesystem::path commit_path = std::filesystem::path(repo_manager_->getRepositoryPath()) / ".svcs" / "objects" / current.substr(0, 2) / current.substr(2);
        
        event_bus_->notify({Event::ERROR_MESSAGE, 
                           "Step " + std::to_string(step) + ": " + current + 
                           " (Exists: " + (std::filesystem::exists(commit_path) ? "YES" : "NO") + ")", "merge"});
        
        if (!std::filesystem::exists(commit_path)) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Commit object not found: " + commit_path.string(), "merge"});
            break;
        }
        
        // Read commit content
        std::ifstream commit_file(commit_path);
        std::string line;
        std::string parent;
        
        while (std::getline(commit_file, line)) {
            if (line.find("parent:") == 0) {
                parent = line.substr(7); // Remove "parent:"
                event_bus_->notify({Event::ERROR_MESSAGE, "  Parent: " + parent, "merge"});
            }
        }
        
        if (parent.empty() || parent == "none") {
            event_bus_->notify({Event::ERROR_MESSAGE, "  No parent (root commit)", "merge"});
            break;
        }
        
        current = parent;
        step++;
    }
}


bool MergeCommand::mergeBranch(const std::string& branch_name) const {
    try {  
        // Check if repository is initialized
        if (!repo_manager_->isRepositoryInitialized()) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Not a SVCS repository", "merge"});
            return false;
        }
        
        // Check if merge is already in progress
        if (isMergeInProgress()) {
            event_bus_->notify({Event::ERROR_MESSAGE, 
                               "Merge in progress. Please complete the merge or use --abort", "merge"});
            return false;
        }
        
        // Get current branch
        std::string current_branch = repo_manager_->getCurrentBranch();
        if (current_branch.empty()) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Cannot determine current branch", "merge"});
            return false;
        }
        
        // Check if merging into itself
        if (current_branch == branch_name) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Cannot merge a branch into itself", "merge"});
            return false;
        }
        
        // Check if branch exists
        if (!repo_manager_->branchExists(branch_name)) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Branch not found: " + branch_name, "merge"});
            return false;
        }
        
        // Get current branch head
        std::string current_head = repo_manager_->getHeadCommit();
        if (current_head.empty()) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Current branch has no commits", "merge"});
            return false;
        }
        
        // Get branch head
        std::string branch_head = repo_manager_->getBranchHead(branch_name);
        if (branch_head.empty()) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Branch '" + branch_name + "' has no commits", "merge"});
            return false;
        }

        event_bus_->notify({Event::ERROR_MESSAGE, "=== BRANCH HISTORY ANALYSIS ===", "merge"});
        debugBranchHistory("current", current_head);
        debugBranchHistory(branch_name, branch_head);

        event_bus_->notify({Event::DEBUG_MESSAGE, 
                   "Current branch head: " + current_head, "merge"});
        event_bus_->notify({Event::DEBUG_MESSAGE, 
                   "Source branch head: " + branch_head, "merge"});
        
        event_bus_->notify({Event::GENERAL_INFO, 
                           "Merging branch '" + branch_name + "' into '" + current_branch + "'", "merge"});
        
        // Check if there are staged changes
        auto staged_files = repo_manager_->getStagedFiles();
        if (!staged_files.empty()) {
            event_bus_->notify({Event::WARNING_MESSAGE, 
                               "You have uncommitted changes. Please commit or stash them first.", "merge"});
            return false;
        }
        
        // Find common ancestor
        std::string common_ancestor = findCommonAncestor(current_head, branch_head);
        if (common_ancestor.empty()) {
            event_bus_->notify({Event::ERROR_MESSAGE, "Could not find common ancestor", "merge"});
            return false;
        }
        
        // Check for fast-forward merge
        if (common_ancestor == current_head) {
            // Fast-forward merge
            event_bus_->notify({Event::GENERAL_INFO, "Fast-forward merge", "merge"});
            
            // Update current branch to point to branch_head
            repo_manager_->updateBranchReference(current_branch, branch_head);
            repo_manager_->updateHead(branch_head);
            
            event_bus_->notify({Event::GENERAL_INFO, 
                               "Fast-forward merge completed successfully", "merge"});
            return true;
        } else if (common_ancestor == branch_head) {
            // Already up-to-date
            event_bus_->notify({Event::GENERAL_INFO, 
                               "Branch '" + branch_name + "' is already up to date", "merge"});
            return true;
        } else {
            // Three-way merge required
            event_bus_->notify({Event::GENERAL_INFO, "Performing three-way merge", "merge"});
            
            // Set merge state
            repo_manager_->setMergeState(branch_name, branch_head);
            
            // Perform three-way merge
            bool success = performThreeWayMerge(common_ancestor, current_head, branch_head);
            
            if (success) {
                event_bus_->notify({Event::GENERAL_INFO, "Merge completed successfully", "merge"});
                repo_manager_->clearMergeState();
            } else {
                event_bus_->notify({Event::WARNING_MESSAGE, 
                                   "Merge conflicts detected. Please resolve them and commit.", "merge"});
            }
            
            return success;
        }
        
    } catch (const std::exception& e) {
        event_bus_->notify({Event::ERROR_MESSAGE, 
                           "Merge failed: " + std::string(e.what()), "merge"});
        repo_manager_->clearMergeState();
        return false;
    }
}

std::string MergeCommand::findCommonAncestor(const std::string& commit1, const std::string& commit2) const {
    event_bus_->notify({Event::DEBUG_MESSAGE, 
                       "Finding common ancestor between: " + commit1 + " and " + commit2, "merge"});
    
    // Simple implementation: walk both histories to find common ancestor
    std::set<std::string> visited1;
    std::set<std::string> visited2;
    
    std::string current1 = commit1;
    std::string current2 = commit2;
    
    // If commits are the same, return either
    if (commit1 == commit2) {
        event_bus_->notify({Event::DEBUG_MESSAGE, "Commits are identical", "merge"});
        return commit1;
    }
    
    // Walk first branch history
    event_bus_->notify({Event::DEBUG_MESSAGE, "Walking history of commit1: " + commit1, "merge"});
    int steps1 = 0;
    while (!current1.empty() && steps1 < 100) { // Safety limit
        visited1.insert(current1);
        event_bus_->notify({Event::DEBUG_MESSAGE, "Commit1 step " + std::to_string(steps1) + ": " + current1, "merge"});
        
        // Check if this commit is in the other branch's history
        if (visited2.find(current1) != visited2.end()) {
            event_bus_->notify({Event::DEBUG_MESSAGE, "Found common ancestor: " + current1, "merge"});
            return current1;
        }
        
        std::string parent = repo_manager_->getParentCommitHash(current1);
        event_bus_->notify({Event::DEBUG_MESSAGE, "Parent of " + current1 + ": " + parent, "merge"});
        
        if (parent.empty() || parent == "none") {
            event_bus_->notify({Event::DEBUG_MESSAGE, "Reached root of commit1 history", "merge"});
            break;
        }
        current1 = parent;
        steps1++;
    }
    
    // Walk second branch history  
    event_bus_->notify({Event::DEBUG_MESSAGE, "Walking history of commit2: " + commit2, "merge"});
    int steps2 = 0;
    while (!current2.empty() && steps2 < 100) { // Safety limit
        visited2.insert(current2);
        event_bus_->notify({Event::DEBUG_MESSAGE, "Commit2 step " + std::to_string(steps2) + ": " + current2, "merge"});
        
        // Check if this commit is in the first branch's history
        if (visited1.find(current2) != visited1.end()) {
            event_bus_->notify({Event::DEBUG_MESSAGE, "Found common ancestor: " + current2, "merge"});
            return current2;
        }
        
        std::string parent = repo_manager_->getParentCommitHash(current2);
        event_bus_->notify({Event::DEBUG_MESSAGE, "Parent of " + current2 + ": " + parent, "merge"});
        
        if (parent.empty() || parent == "none") {
            event_bus_->notify({Event::DEBUG_MESSAGE, "Reached root of commit2 history", "merge"});
            break;
        }
        current2 = parent;
        steps2++;
    }
    
    event_bus_->notify({Event::DEBUG_MESSAGE, 
                       "No common ancestor found. Visited " + std::to_string(visited1.size()) + 
                       " commits in branch1 and " + std::to_string(visited2.size()) + " in branch2", "merge"});
    
    // If no common ancestor found, return empty string
    return "";
}

bool MergeCommand::abortMerge() const {
    try {
        if (!isMergeInProgress()) {
            event_bus_->notify({Event::ERROR_MESSAGE, "No merge in progress to abort", "merge"});
            return false;
        }
        
        // Get the branch that was being merged
        std::string merge_branch = repo_manager_->getMergeBranch();
        
        // Clear merge state
        repo_manager_->clearMergeState();
        
        // Reset staging area
        repo_manager_->clearStagingArea();
        
        event_bus_->notify({Event::GENERAL_INFO, 
                           "Merge with branch '" + merge_branch + "' aborted successfully", "merge"});
        return true;
        
    } catch (const std::exception& e) {
        event_bus_->notify({Event::ERROR_MESSAGE, 
                           "Failed to abort merge: " + std::string(e.what()), "merge"});
        return false;
    }
}

bool MergeCommand::isMergeInProgress() const {
    return repo_manager_->isMergeInProgress();
}

bool MergeCommand::performThreeWayMerge(const std::string& ancestor, 
                                       const std::string& current, 
                                       const std::string& other) const {
    bool has_conflicts = false;
    
    // Get files from all three commits
    auto ancestor_files = repo_manager_->getCommitFiles(ancestor);
    auto current_files = repo_manager_->getCommitFiles(current);
    auto other_files = repo_manager_->getCommitFiles(other);
    
    // Combine all unique files
    std::set<std::string> all_files;
    all_files.insert(ancestor_files.begin(), ancestor_files.end());
    all_files.insert(current_files.begin(), current_files.end());
    all_files.insert(other_files.begin(), other_files.end());
    
    // Merge each file
    for (const auto& file_path : all_files) {
        // Get file content from each version
        std::string ancestor_content = repo_manager_->getFileContentAtCommit(ancestor, file_path);
        std::string current_content = repo_manager_->getFileContentAtCommit(current, file_path);
        std::string other_content = repo_manager_->getFileContentAtCommit(other, file_path);
        
        std::string merged_content;
        bool no_conflict = mergeFileContent(ancestor_content, current_content, other_content, merged_content);
        
        if (!no_conflict) {
            has_conflicts = true;
            event_bus_->notify({Event::WARNING_MESSAGE, 
                               "Conflict in file: " + file_path, "merge"});
        }
        
        // Write merged content to working directory
        // In real implementation, we would stage the file if no conflict
        if (!merged_content.empty()) {
            std::filesystem::path repo_path = repo_manager_->getRepositoryPath();
            std::filesystem::path full_path = repo_path / file_path;
            
            // Create directory if needed
            std::filesystem::create_directories(full_path.parent_path());
            
            std::ofstream file(full_path);
            file << merged_content;
            
            // Stage the file if no conflict
            if (no_conflict) {
                repo_manager_->addFileToStaging(file_path);
            }
        }
    }
    
    return !has_conflicts;
}

bool MergeCommand::mergeFileContent(const std::string& ancestor_content,
                                   const std::string& current_content,
                                   const std::string& other_content,
                                   std::string& merged_content) {
    // TODO: Implement file content merging
    // This is a simplified version - real implementation would be more complex
    
    if (current_content == other_content) {
        merged_content = current_content;
        return true; // No conflict
    }
    
    if (ancestor_content == current_content) {
        merged_content = other_content;
        return true; // Fast-forward
    }
    
    if (ancestor_content == other_content) {
        merged_content = current_content;
        return true; // Already up-to-date
    }
    
    // Conflict detected
    merged_content = "<<<<<<< Current\n" + current_content + 
                    "\n=======\n" + other_content + 
                    "\n>>>>>>> Other\n";
    return false; // Conflict
}