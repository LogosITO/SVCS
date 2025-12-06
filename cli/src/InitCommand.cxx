/**
 * @file InitCommand.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the InitCommand class, responsible for processing the 'svcs init' command and initializing a new SVCS repository.
 *
 * @russian
 * @brief Реализация класса InitCommand, отвечающего за обработку команды 'svcs init' и инициализацию нового репозитория SVCS.
 */

#include <utility>

#include "../include/InitCommand.hxx"

namespace svcs::cli {

using namespace svcs::core;
using namespace svcs::services;

InitCommand::InitCommand(std::shared_ptr<ISubject> subject, 
                         std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(std::move(subject)), repoManager_(std::move(repoManager)) {
}

bool InitCommand::execute(const std::vector<std::string>& args) {
    std::string path = ".";
    bool force = false;
    bool showHelpFlag = false;

    for (const auto& arg : args) {
        if (arg == "--force" || arg == "-f") {
            force = true;
        } else if (arg == "--help" || arg == "-h") {
            showHelpFlag = true;
        } else if (arg.find("-") == 0) {
            // Неизвестная опция
            eventBus_->notify({Event::WARNING_MESSAGE, 
                              "Unknown option: " + arg, "init"});
        } else {
            // Это путь (может быть ".")
            path = arg;
        }
    }
    
    if (showHelpFlag) {
        showHelp();
        return true;
    }
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Initializing new repository in: " + path, "init"});
    
    if (force) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Force flag enabled - will reinitialize if repository exists", "init"});
    }
    
    if (repoManager_->initializeRepository(path, force)) {
        eventBus_->notify({Event::REPOSITORY_INIT_SUCCESS, 
                          "Initialized empty SVCS repository in " + path + "/.svcs", "init"});
        return true;
    } else {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Failed to initialize repository", "init"});
        return false;
    }
}

std::string InitCommand::getName() const {
    return "init";
}

std::string InitCommand::getDescription() const {
    return "Initialize a new SVCS repository";
}

std::string InitCommand::getUsage() const {
    return "svcs init [path] [--force]";
}

void InitCommand::showHelp() const {
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Usage: " + getUsage(), "init"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Description: " + getDescription(), "init"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Options:", "init"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  --force, -f    Reinitialize even if repository exists", "init"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  --help, -h     Show this help", "init"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "If no path is provided, uses current directory", "init"});
}

}