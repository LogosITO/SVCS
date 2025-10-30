/**
 * @file HubCommand.cxx
 * @brief Implementation of the HubCommand class for creating bare repositories.
 */

#include "../include/HubCommand.hxx"
#include "../../services/Event.hxx"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

HubCommand::HubCommand(std::shared_ptr<ISubject> event_bus,
                       std::shared_ptr<RepositoryManager> repository_manager)
    : event_bus_(std::move(event_bus))
    , repository_manager_(std::move(repository_manager))
{
}

std::string HubCommand::getName() const {
    return "hub";
}

std::string HubCommand::getDescription() const {
    return "Create a central hub repository for team collaboration";
}

std::string HubCommand::getUsage() const {
    return "svcs hub <repository-path>";
}

bool HubCommand::execute(const std::vector<std::string>& args) {
    // === Argument validation ===
    if (args.empty()) {
        notifyError("Repository path is required");
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Path argument required"});
        return false;
    }

    if (args.size() > 1) {
        notifyError("Too many arguments");
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Too many arguments provided"});
        return false;
    }

    fs::path repo_path(args[0]);
    if (repo_path.empty()) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Invalid repository path"});
        return false;
    }
    if (fs::exists(repo_path)) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Repository already exists"});
        return false;
    }

    notifyInfo("Creating hub repository at " + repo_path.string());

    try {
        fs::create_directories(repo_path / ".svcs" / "objects");
        fs::create_directories(repo_path / ".svcs" / "refs" / "heads");
        fs::create_directories(repo_path / ".svcs" / "refs" / "tags");
        fs::create_directories(repo_path / ".svcs" / "hooks");
        fs::create_directories(repo_path / ".svcs" / "info");

        // === Write config file ===
        std::ofstream config(repo_path / ".svcs" / "config");
        config << "[core]\n"
               << "bare = true\n"
               << "[svcs]\n"
               << "hub = true\n";
        config.close();

        std::ofstream head(repo_path / ".svcs" / "HEAD");
        head << "ref: refs/heads/main\n";
        head.close();

    } catch (const std::exception& e) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE,
                            std::string("Initialization failed: ") + e.what()});
        return false;
    }

    event_bus_->notify({Event::Type::REPOSITORY_INIT_SUCCESS,
                        "Hub repository successfully created"});

    return true;
}

void HubCommand::showHelp() const {
    std::cout << "Usage: " << getUsage() << "\n"
              << "Create a central hub repository for team collaboration.\n\n"
              << "Arguments:\n"
              << "  <repository-path>  Path where to create the hub repository\n\n"
              << "Examples:\n"
              << "  svcs hub /srv/repos/my-project\n"
              << std::endl;
}

void HubCommand::notifyInfo(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::GENERAL_INFO, message});
    } else {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void HubCommand::notifyError(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, message});
    } else {
        std::cerr << "[ERROR] " << message << std::endl;
    }
}

void HubCommand::notifySuccess(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::REPOSITORY_INIT_SUCCESS, message});
    } else {
        std::cout << "[SUCCESS] " << message << std::endl;
    }
}
