/**
 * @file ClearCommand.cxx
 * @brief Implementation of the ClearCommand class for removing SVCS repository.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/ClearCommand.hxx"
#include <iostream>
#include <algorithm>

ClearCommand::ClearCommand(std::shared_ptr<ISubject> subject,
                           std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(subject), repoManager_(repoManager) {
}

bool ClearCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "clear";
    
    // Check if repository exists
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "No SVCS repository found in current directory.", SOURCE});
        return false;
    }
    
    std::string repoPath = repoManager_->getRepositoryPath();
    
    // Check for --help flag
    if (std::find(args.begin(), args.end(), "--help") != args.end() ||
        std::find(args.begin(), args.end(), "-h") != args.end()) {
        showHelp();
        return true;
    }
    
    // Check for --force flag
    bool force = std::find(args.begin(), args.end(), "--force") != args.end() ||
                 std::find(args.begin(), args.end(), "-f") != args.end();
    
    eventBus_->notify({Event::WARNING_MESSAGE, 
                      "This will permanently remove the SVCS repository from:", SOURCE});
    eventBus_->notify({Event::WARNING_MESSAGE, 
                      "  " + repoPath + "/.svcs", SOURCE});
    
    // Count files that will be removed
    std::filesystem::path svcsDir = std::filesystem::path(repoPath) / ".svcs";
    size_t fileCount = 0;
    size_t dirCount = 0;
    
    try {
        if (std::filesystem::exists(svcsDir)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(svcsDir)) {
                if (entry.is_regular_file()) {
                    fileCount++;
                } else if (entry.is_directory()) {
                    dirCount++;
                }
            }
        }
        
        if (fileCount > 0) {
            eventBus_->notify({Event::WARNING_MESSAGE, 
                              "This will remove " + std::to_string(fileCount) + " files and " + 
                              std::to_string(dirCount) + " directories.", SOURCE});
        }
    } catch (const std::exception& e) {
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Error counting repository files: " + std::string(e.what()), SOURCE});
    }
    
    // Ask for confirmation unless --force is used
    if (!force && !confirmClear()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Clear operation cancelled.", SOURCE});
        return false;
    }
    
    // Remove repository
    if (removeRepository()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "SVCS repository successfully removed.", SOURCE});
        return true;
    } else {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Failed to remove repository.", SOURCE});
        return false;
    }
}

std::string ClearCommand::getDescription() const {
    return "Remove SVCS repository from current directory";
}

std::string ClearCommand::getUsage() const {
    return "svcs clear [--force]";
}

void ClearCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Usage: " + getUsage(), "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Description: " + getDescription(), "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Permanently removes the .svcs directory and all repository data.", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "This action cannot be undone!", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Options:", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  --force, -f    Skip confirmation prompt", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  --help, -h     Show this help message", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Examples:", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs clear              Remove repository (with confirmation)", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs clear --force      Remove repository (without confirmation)", "clear"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs clear -f           Remove repository (without confirmation)", "clear"});
}

bool ClearCommand::confirmClear() const {
    std::cout << "Are you sure you want to remove the SVCS repository? [y/N]: ";
    std::string response;
    std::getline(std::cin, response);
    
    return (response == "y" || response == "Y" || response == "yes");
}

bool ClearCommand::removeRepository() const {
    std::string repoPath = repoManager_->getRepositoryPath();
    std::filesystem::path svcsDir = std::filesystem::path(repoPath) / ".svcs";
    
    try {
        return repoManager_->removeRepository(svcsDir);
    } catch (const std::exception& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error removing repository: " + std::string(e.what()), "clear"});
        return false;
    }
}