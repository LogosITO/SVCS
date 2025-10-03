/**
 * @file UndoCommand.cxx
 * @brief Implementation of the UndoCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/UndoCommand.hxx"

#include <algorithm>
#include <iostream>
#include <fstream>

UndoCommand::UndoCommand(std::shared_ptr<ISubject> subject,
                         std::shared_ptr<RepositoryManager> repoManager)
    : event_bus(subject), repo_manager(repoManager) {
}

std::string UndoCommand::getName() const {
    return "undo";
}

std::string UndoCommand::getDescription() const {
    return "Undo changes from previous saves";
}

std::string UndoCommand::getUsage() const {
    return "svcs undo [--last|commit-hash] [--force]";
}

bool UndoCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "undo";
    
    // Check for help first
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return true;
        }
    }
    
    if (!repo_manager->isRepositoryInitialized()) {
        event_bus->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", SOURCE});
        return false;
    }
    
    // Parse arguments
    bool force = false;
    std::string targetCommit;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const auto& arg = args[i];
        
        if (arg == "--force" || arg == "-f") {
            force = true;
        }
        else if (arg == "--last" || arg == "-l") {
            // Undo last commit
            targetCommit = "last";
        }
        else if (arg == "--commit" || arg == "-c") {
            if (i + 1 < args.size()) {
                targetCommit = args[i + 1];
                i++; // Skip next argument
            } else {
                event_bus->notify({Event::ERROR_MESSAGE, 
                                  "Missing commit hash for --commit option", SOURCE});
                return false;
            }
        }
        else {
            // Assume it's a commit hash
            targetCommit = arg;
        }
    }
    
    // Execute undo based on target
    if (targetCommit.empty() || targetCommit == "last") {
        return undoLastCommit(force);
    } else {
        return undoSpecificCommit(targetCommit, force);
    }
}

bool UndoCommand::undoLastCommit(bool force) {
    const std::string SOURCE = "undo";
    
    auto commits = repo_manager->getCommitHistory();
    if (commits.empty()) {
        event_bus->notify({Event::GENERAL_INFO, 
                          "No commits to undo.", SOURCE});
        return true;
    }
    
    // Check if it's the initial commit
    if (commits.size() == 1) {
        if (force) {
            event_bus->notify({Event::WARNING_MESSAGE, 
                              "Force undoing initial commit - this will reset repository", SOURCE});
            return forceResetRepository();
        } else {
            event_bus->notify({Event::ERROR_MESSAGE, 
                              "Cannot undo the initial commit. Use --force to reset repository.", SOURCE});
            return false;
        }
    }
    
    const auto& lastCommit = commits[0];
    std::string shortHash = lastCommit.hash.substr(0, 8);
    
    event_bus->notify({Event::WARNING_MESSAGE, 
                      "About to undo last commit:", SOURCE});
    event_bus->notify({Event::GENERAL_INFO, 
                      "   Commit: " + shortHash + " - " + lastCommit.message, SOURCE});
    event_bus->notify({Event::GENERAL_INFO, 
                      "   Files: " + std::to_string(lastCommit.files_count), SOURCE});
    
    if (force) {
        event_bus->notify({Event::WARNING_MESSAGE, 
                          "Force mode enabled - skipping confirmation", SOURCE});
    }
    
    if (!confirmUndo(lastCommit.message, lastCommit.hash, force)) {
        event_bus->notify({Event::GENERAL_INFO, 
                          "Undo cancelled.", SOURCE});
        return false;
    }
    
    // Perform undo operation
    bool success = repo_manager->revertCommit(lastCommit.hash);
    
    if (success) {
        event_bus->notify({Event::GENERAL_INFO, 
                          "Successfully undone commit: " + lastCommit.message, SOURCE});
        return true;
    } else {
        if (force) {
            event_bus->notify({Event::WARNING_MESSAGE, 
                              "Normal undo failed, attempting force reset", SOURCE});
            return forceResetRepository();
        } else {
            event_bus->notify({Event::ERROR_MESSAGE, 
                              "Failed to undo commit. Use --force to attempt repository reset.", SOURCE});
            return false;
        }
    }
}

bool UndoCommand::undoSpecificCommit(const std::string& commitHash, bool force) {
    const std::string SOURCE = "undo";
    
    auto commits = repo_manager->getCommitHistory();
    
    // Find the commit
    auto commitIt = std::find_if(commits.begin(), commits.end(),
        [&commitHash](const CommitInfo& commit) {
            return commit.hash.find(commitHash) == 0;
        });
    
    if (commitIt == commits.end()) {
        event_bus->notify({Event::ERROR_MESSAGE, 
                          "Commit not found: " + commitHash, SOURCE});
        return false;
    }
    
    const auto& commit = *commitIt;
    
    // Check if it's the initial commit
    if (commits.size() == 1) {
        if (force) {
            event_bus->notify({Event::WARNING_MESSAGE, 
                              "Force undoing initial commit - this will reset repository", SOURCE});
            return forceResetRepository();
        } else {
            event_bus->notify({Event::ERROR_MESSAGE, 
                              "Cannot undo the initial commit. Use --force to reset repository.", SOURCE});
            return false;
        }
    }
    
    event_bus->notify({Event::WARNING_MESSAGE, 
                      "About to undo commit:", SOURCE});
    event_bus->notify({Event::GENERAL_INFO, 
                      "   Commit: " + commit.hash.substr(0, 8) + " - " + commit.message, SOURCE});
    event_bus->notify({Event::GENERAL_INFO, 
                      "   Files: " + std::to_string(commit.files_count), SOURCE});
    
    if (force) {
        event_bus->notify({Event::WARNING_MESSAGE, 
                          "Force mode enabled - skipping confirmation", SOURCE});
    }
    
    if (!confirmUndo(commit.message, commit.hash, force)) {
        event_bus->notify({Event::GENERAL_INFO, 
                          "Undo cancelled.", SOURCE});
        return false;
    }
    
    bool success = repo_manager->revertCommit(commit.hash);
    
    if (success) {
        event_bus->notify({Event::GENERAL_INFO, 
                          "Successfully undone commit: " + commit.message, SOURCE});
        return true;
    } else {
        if (force) {
            event_bus->notify({Event::WARNING_MESSAGE, 
                              "Normal undo failed, attempting force reset", SOURCE});
            return forceResetRepository();
        } else {
            event_bus->notify({Event::ERROR_MESSAGE, 
                              "Failed to undo commit. Use --force to attempt repository reset.", SOURCE});
            return false;
        }
    }
}

bool UndoCommand::confirmUndo(const std::string& commitMessage, const std::string& commitHash, bool force) {
    if (force) {
        return true;
    }
    
    std::cout << "Are you sure you want to undo this commit? [y/N]: ";
    std::string response;
    std::getline(std::cin, response);
    
    return (response == "y" || response == "Y" || response == "yes");
}

bool UndoCommand::forceResetRepository() {
    const std::string SOURCE = "undo";
    
    event_bus->notify({Event::WARNING_MESSAGE, 
                      "Performing force reset of repository...", SOURCE});
    
    try {
        std::string repoPath = repo_manager->getRepositoryPath();
        
        repo_manager->updateHead("");
        
        repo_manager->clearStagingArea();
        
        std::filesystem::path mainBranch = std::filesystem::path(repoPath) / ".svcs" / "refs" / "heads" / "main";
        if (std::filesystem::exists(mainBranch)) {
            std::ofstream branchFile(mainBranch, std::ios::trunc);
        }
        
        event_bus->notify({Event::GENERAL_INFO, 
                          "Repository reset successfully", SOURCE});
        return true;
        
    } catch (const std::exception& e) {
        event_bus->notify({Event::ERROR_MESSAGE, 
                          "Force reset failed: " + std::string(e.what()), SOURCE});
        return false;
    }
}

void UndoCommand::showHelp() const {
    event_bus->notify({Event::HELP_MESSAGE, 
                      "Usage: " + getUsage(), "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "Description: " + getDescription(), "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "Reverts changes from a previous save (commit).", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "Options:", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  [no args]         Undo last commit", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  --last, -l        Undo last commit (explicit)", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  <commit-hash>     Undo specific commit", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  --commit, -c hash Undo specific commit", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  --force, -f       Force undo (skip confirmation, reset on failure)", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "Examples:", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  svcs undo                    # Undo last commit with confirmation", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  svcs undo --force            # Force undo last commit", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  svcs undo abc123             # Undo specific commit", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  svcs undo -c abc123 --force  # Force undo specific commit", "undo"});
    event_bus->notify({Event::HELP_MESSAGE, 
                      "  svcs undo --last -f          # Force undo last commit", "undo"});
}