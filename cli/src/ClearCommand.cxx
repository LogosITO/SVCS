/**
 * @file ClearCommand.cxx
 * @brief Implementation of the ClearCommand class for removing SVCS repository.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/ClearCommand.hxx"
#include <iostream>

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
    
    eventBus_->notify({Event::WARNING_MESSAGE, 
                      "This will permanently remove the SVCS repository from:", SOURCE});
    eventBus_->notify({Event::WARNING_MESSAGE, 
                      "  " + repoPath + "/.svcs", SOURCE});
    
    // Ask for confirmation
    if (!confirmClear()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Clear operation cancelled.", SOURCE});
        return false;
    }
    
    // Remove repository
    std::filesystem::path svcsDir = std::filesystem::path(repoPath) / ".svcs";
    
    try {
        if (repoManager_->removeRepository(svcsDir)) {
            eventBus_->notify({Event::GENERAL_INFO, 
                              "SVCS repository successfully removed.", SOURCE});
            return true;
        } else {
            eventBus_->notify({Event::ERROR_MESSAGE, 
                              "Failed to remove repository.", SOURCE});
            return false;
        }
    } catch (const std::exception& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error removing repository: " + std::string(e.what()), SOURCE});
        return false;
    }
}

std::string ClearCommand::getDescription() const {
    return "Remove SVCS repository from current directory";
}

std::string ClearCommand::getUsage() const {
    return "svcs clear";
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
                      "The command will ask for confirmation before proceeding.", "clear"});
}

bool ClearCommand::confirmClear() const {
    std::cout << "Are you sure you want to remove the SVCS repository? [y/N]: ";
    std::string response;
    std::getline(std::cin, response);
    
    return (response == "y" || response == "Y" || response == "yes");
}