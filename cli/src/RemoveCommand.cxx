/**
 * @file RemoveCommand.cxx
 * @brief Implementation of the RemoveCommand class for removing files from staging area.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/RemoveCommand.hxx"
#include <algorithm>
#include <fstream>
#include <iostream>

RemoveCommand::RemoveCommand(std::shared_ptr<ISubject> subject,
                           std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(subject), repoManager_(repoManager) {
}

bool RemoveCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "remove";
    
    // Check if repository is initialized
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", SOURCE});
        return false;
    }
    
    // Check if there are any arguments
    if (args.empty()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "No files specified. Use 'svcs remove <file>' or 'svcs remove --all' for all files.", SOURCE});
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Use 'svcs remove --help' for more information.", SOURCE});
        return false;
    }
    
    // Parse flags first
    bool remove_all = false;
    bool force_remove = false;
    bool show_help = false;
    std::vector<std::string> filesToRemove;
    
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            show_help = true;
        } else if (arg == "--all" || arg == "-a") {
            remove_all = true;
        } else if (arg == "--force" || arg == "-f") {
            force_remove = true;
        } else if (arg == ".") {
            remove_all = true;
            force_remove = true; // Legacy behavior - no confirmation
        } else {
            filesToRemove.push_back(arg);
        }
    }
    
    // Handle help
    if (show_help) {
        showHelp();
        return true;
    }
    
    // Check if there are staged files
    auto stagedFiles = repoManager_->getStagedFiles();
    if (stagedFiles.empty()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "No files in staging area.", SOURCE});
        return true;
    }
    
    // Handle remove all
    if (remove_all) {
        if (force_remove || confirmRemoveAll()) {
            if (removeAllFromStaging()) {
                eventBus_->notify({Event::GENERAL_INFO, 
                                  "Removed all files from staging area.", SOURCE});
                return true;
            } else {
                eventBus_->notify({Event::ERROR_MESSAGE, 
                                  "Failed to remove all files from staging area.", SOURCE});
                return false;
            }
        } else {
            eventBus_->notify({Event::GENERAL_INFO, 
                              "Remove operation cancelled.", SOURCE});
            return false;
        }
    }
    
    // Handle specific file removal
    if (filesToRemove.empty()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "No files specified for removal.", SOURCE});
        return false;
    }
    
    bool any_removed = false;
    bool any_errors = true;

    for (const auto& file : filesToRemove) {
        if (removeFileFromStaging(file)) {
            eventBus_->notify({Event::GENERAL_INFO, 
                              "Removed from staging: " + file, SOURCE});
            any_removed = true;
        } else {
            any_errors = true;
        }
    }
    
    return any_removed || !any_errors;
}

std::string RemoveCommand::getDescription() const {
    return "Remove files from staging area";
}

std::string RemoveCommand::getUsage() const {
    return "svcs remove <file> [file2 ...] [--all]";
}

void RemoveCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Usage: " + getUsage(), "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Description: " + getDescription(), "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Removes files from the staging area (index).", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "The actual files on disk are not affected.", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Options:", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  <file>           Remove specific file from staging", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  --all, -a        Remove all files from staging area", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  --force, -f      Skip confirmation (use with --all)", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  .                Remove all files (legacy, no confirmation)", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  --help, -h       Show this help message", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Examples:", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs remove file.txt                 Remove single file", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs remove file1.txt file2.txt      Remove multiple files", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs remove --all                    Remove all files (with confirmation)", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs remove --all --force            Remove all files (without confirmation)", "remove"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs remove .                        Remove all files (legacy, no confirmation)", "remove"});
}

bool RemoveCommand::removeFileFromStaging(const std::string& filePath) {
    // Get current staged files
    auto stagedFiles = repoManager_->getStagedFiles();
    
    // Check if file is actually staged
    auto it = std::find(stagedFiles.begin(), stagedFiles.end(), filePath);
    if (it == stagedFiles.end()) {
        eventBus_->notify({Event::WARNING_MESSAGE, 
                          "File not in staging area: " + filePath, "remove"});
        return false;
    }
    
    // Remove file from staging by recreating index without this file
    std::filesystem::path repoPath = repoManager_->getRepositoryPath();
    std::filesystem::path indexFile = repoPath / ".svcs" / "index";
    
    try {
        std::ofstream index(indexFile, std::ios::trunc);
        if (!index) {
            eventBus_->notify({Event::ERROR_MESSAGE, 
                              "Failed to open index file", "remove"});
            return false;
        }
        
        // Write all staged files except the one to remove
        for (const auto& stagedFile : stagedFiles) {
            if (stagedFile != filePath) {
                index << stagedFile << "\n";
            }
        }
        
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Removed from staging: " + filePath, "remove"});
        return true;
        
    } catch (const std::exception& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error removing file from staging: " + std::string(e.what()), "remove"});
        return false;
    }
}

bool RemoveCommand::removeAllFromStaging() {
    return repoManager_->clearStagingArea();
}

bool RemoveCommand::confirmRemoveAll() const {
    auto stagedFiles = repoManager_->getStagedFiles();
    
    if (stagedFiles.empty()) {
        return true; // Nothing to remove, no confirmation needed
    }
    
    eventBus_->notify({Event::WARNING_MESSAGE, 
                      "This will remove " + std::to_string(stagedFiles.size()) + 
                      " file(s) from staging area:", "remove"});
    
    // Show which files will be removed
    for (const auto& file : stagedFiles) {
        eventBus_->notify({Event::GENERAL_INFO, "  " + file, "remove"});
    }
    
    std::cout << "Are you sure you want to remove all files from staging? [y/N]: ";
    std::string response;
    std::getline(std::cin, response);
    
    return (response == "y" || response == "Y" || response == "yes");
}