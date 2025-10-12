/**
 * @file HubCommand.cxx
 * @brief Implementation of the HubCommand class for creating bare repositories.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
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
        showHelp();
        return false;
    }

    if (args.size() > 1) {
        notifyError("Too many arguments");
        showHelp();
        return false;
    }

    const std::string& repo_path_str = args[0];
    fs::path repo_path(repo_path_str);

    // Добавляем расширение .svcs если не указано
    if (repo_path.extension().empty()) {
        repo_path += ".svcs";
    }

    if (!isValidHubPath(repo_path)) {
        notifyError("Invalid repository path: " + repo_path.string());
        return false;
    }

    if (!isPathAvailable(repo_path)) {
        notifyError("Path already exists or is not accessible: " + repo_path.string());
        return false;
    }

    notifyInfo("Creating hub repository at: " + repo_path.string());

    if (!initializeHubRepository(repo_path)) {
        notifyError("Failed to initialize hub repository");
        return false;
    }

    notifySuccess("Hub repository created successfully at: " + repo_path.string());
    notifyInfo("You can now use this as remote: user@host:" + fs::absolute(repo_path).string());

    return true;
}

void HubCommand::showHelp() const {
    std::cout << "Usage: " << getUsage() << "\n"
              << "Create a central hub repository for team collaboration.\n\n"
              << "Arguments:\n"
              << "  <repository-path>  Path where to create the hub repository\n"
              << "                     (will have .svcs extension added if not present)\n\n"
              << "Examples:\n"
              << "  svcs hub /srv/repos/my-project          # Creates /srv/repos/my-project.svcs\n"
              << "  svcs hub /srv/repos/my-project.svcs     # Creates /srv/repos/my-project.svcs\n"
              << std::endl;
}

bool HubCommand::initializeHubRepository(const fs::path& repo_path) const {
    try {
        // Create main repository directory
        if (!fs::create_directories(repo_path)) {
            notifyError("Failed to create repository directory: " + repo_path.string());
            return false;
        }

        fs::path svcs_path = repo_path / ".svcs";

        if (!createHubDirectoryStructure(svcs_path)) {
            return false;
        }

        if (!createHubConfigFile(svcs_path)) {
            return false;
        }

        if (!createHubHEADFile(svcs_path)) {
            return false;
        }

        return true;

    } catch (const fs::filesystem_error& ex) {
        notifyError("Filesystem error: " + std::string(ex.what()));
        return false;
    } catch (const std::exception& ex) {
        notifyError("Error: " + std::string(ex.what()));
        return false;
    }
}

bool HubCommand::createHubDirectoryStructure(const fs::path& svcs_path) const {
    try {
        std::vector<fs::path> directories = {
            svcs_path / "objects",
            svcs_path / "refs" / "heads",
            svcs_path / "refs" / "tags",
            svcs_path / "hooks",
            svcs_path / "info"
        };

        for (const auto& dir : directories) {
            if (!fs::create_directories(dir)) {
                notifyError("Failed to create directory: " + dir.string());
                return false;
            }
        }

        notifyInfo("Created repository directory structure");
        return true;

    } catch (const fs::filesystem_error& ex) {
        notifyError("Failed to create directory structure: " + std::string(ex.what()));
        return false;
    }
}

bool HubCommand::createHubConfigFile(const fs::path& svcs_path) const {
    try {
        fs::path config_path = svcs_path / "config";
        std::ofstream config_file(config_path);

        if (!config_file.is_open()) {
            notifyError("Failed to create config file: " + config_path.string());
            return false;
        }

        config_file << "[core]\n"
                   << "\trepositoryformatversion = 0\n"
                   << "\tfilemode = true\n"
                   << "\tbare = true\n"
                   << "\tlogallrefupdates = true\n"
                   << "[svcs]\n"
                   << "\thub = true\n"
                   << "\tversion = 1.0\n";

        config_file.close();
        notifyInfo("Created repository configuration");
        return true;

    } catch (const std::exception& ex) {
        notifyError("Failed to create config file: " + std::string(ex.what()));
        return false;
    }
}

bool HubCommand::createHubHEADFile(const fs::path& svcs_path) const {
    try {
        fs::path head_path = svcs_path / "HEAD";
        std::ofstream head_file(head_path);

        if (!head_file.is_open()) {
            notifyError("Failed to create HEAD file: " + head_path.string());
            return false;
        }

        head_file << "ref: refs/heads/main\n";
        head_file.close();

        notifyInfo("Created HEAD file pointing to main branch");
        return true;

    } catch (const std::exception& ex) {
        notifyError("Failed to create HEAD file: " + std::string(ex.what()));
        return false;
    }
}

bool HubCommand::isValidHubPath(const fs::path& path) const {
    if (path.empty()) {
        return false;
    }

    // Проверяем, что путь абсолютный или может быть нормализован
    try {
        fs::path normalized = fs::absolute(path);
        return true;
    } catch (const fs::filesystem_error&) {
        return false;
    }
}

bool HubCommand::isPathAvailable(const fs::path& path) const {
    try {
        if (fs::exists(path)) {
            // Если путь существует, проверяем что это директория и она пуста
            if (!fs::is_directory(path)) {
                return false;
            }
            return fs::is_empty(path);
        }
        return true; // Путь не существует - можно создавать
    } catch (const fs::filesystem_error&) {
        return false;
    }
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