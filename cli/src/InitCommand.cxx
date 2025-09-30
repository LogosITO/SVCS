/**
 * @file InitCommand.cxx
 * @brief Implementation of the InitCommand class, responsible for processing the 'svcs init' command and initializing a new SVCS repository.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/InitCommand.hxx"

InitCommand::InitCommand(std::shared_ptr<ISubject> subject, 
                         std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(subject), repoManager_(repoManager) {
}

bool InitCommand::execute(const std::vector<std::string>& args) {
    std::string path = ".";
    bool force = false;
    
    // Парсим аргументы
    for (const auto& arg : args) {
        if (arg == "--force" || arg == "-f") {
            force = true;
        } else if (arg != ".") {
            path = arg;
        }
    }
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Initializing new repository in: " + path, "init"});
    
    if (force) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Force flag enabled - will reinitialize if repository exists", "init"});
    }
    
    if (repoManager_->initializeRepository(path, force)) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Initialized empty SVCS repository in " + path + "/.svcs", "init"});
        return true;
    } else {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Failed to initialize repository", "init"});
        return false;
    }
}

std::string InitCommand::getDescription() const {
    return "Initialize a new SVCS repository";
}

std::string InitCommand::getUsage() const {
    return "svcs init [path] [--force]";
}

void InitCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Usage: " + getUsage(), "init"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Description: " + getDescription(), "init"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Options:", "init"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  --force, -f    Reinitialize even if repository exists", "init"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "If no path is provided, uses current directory", "init"});
}