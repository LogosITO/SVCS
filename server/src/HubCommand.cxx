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
    if (args.empty()) {
        notifyError("Repository path is required");
        return false;
    }

    if (args.size() > 1) {
        notifyError("Too many arguments");
        return false;
    }

    fs::path repo_path(args[0]);
    if (repo_path.empty()) {
        notifyError("Invalid repository path");
        return false;
    }

    if (repo_path.is_relative()) {
        repo_path = fs::absolute(repo_path);
    }

    fs::path repo_dir = repo_path / ".svcs";

    if (fs::exists(repo_dir)) {
        notifyError("Repository already exists at: " + repo_dir.string());
        return false;
    }

    if (!fs::exists(repo_path)) {
        fs::create_directories(repo_path);
    }

    notifyInfo("Creating hub repository at " + repo_dir.string());

    try {
        fs::create_directories(repo_dir / "objects");
        fs::create_directories(repo_dir / "refs" / "heads");
        fs::create_directories(repo_dir / "refs" / "tags");
        fs::create_directories(repo_dir / "hooks");
        fs::create_directories(repo_dir / "info");

        std::ofstream config(repo_dir / "config");
        config << "[core]\n"
               << "bare = true\n"
               << "repositoryformatversion = 0\n"
               << "[svcs]\n"
               << "hub = true\n";
        config.close();

        std::ofstream head(repo_dir / "HEAD");
        head << "ref: refs/heads/main\n";
        head.close();

        std::ofstream description(repo_dir / "description");
        description << "Unnamed repository; edit this file to name the repository.\n";
        description.close();

    } catch (const std::exception& e) {
        notifyError(std::string("Initialization failed: ") + e.what());
        fs::remove_all(repo_dir);
        return false;
    }

    notifySuccess("Hub repository successfully created at " + repo_dir.string());
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
