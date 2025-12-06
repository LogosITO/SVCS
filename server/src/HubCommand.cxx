/**
 * @file HubCommand.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the HubCommand class for creating bare repositories.
 * @details This file provides the concrete implementation for the @ref HubCommand class.
 * The `execute` method handles the logic for:
 * 1.  **Argument Validation**: Ensures exactly one valid path is provided.
 * 2.  **Filesystem Checks**: Verifies that a repository doesn't already exist at the target location.
 * 3.  **Directory Creation**: Creates the target directory and the full `.svcs` "bare" repository structure (objects, refs, hooks, etc.).
 * 4.  **File Seeding**: Writes the initial `config`, `HEAD`, and `description` files.
 * 5.  **Notifications**: Uses the @ref ISubject event bus to report success, info, or error messages.
 * This command uses the `RepositoryManager` dependency indirectly, as the logic
 * is simple enough to be self-contained filesystem operations, but it relies on
 * the manager's path conventions.
 *
 * @russian
 * @brief Реализация класса HubCommand для создания голых репозиториев.
 * @details Этот файл предоставляет конкретную реализацию для класса @ref HubCommand.
 * Метод `execute` обрабатывает логику для:
 * 1.  **Проверки аргументов**: Обеспечивает предоставление ровно одного валидного пути.
 * 2.  **Проверок файловой системы**: Проверяет, что репозиторий еще не существует в целевом местоположении.
 * 3.  **Создания директорий**: Создает целевую директорию и полную структуру "голого" репозитория `.svcs` (objects, refs, hooks и т.д.).
 * 4.  **Инициализации файлов**: Записывает начальные файлы `config`, `HEAD` и `description`.
 * 5.  **Уведомлений**: Использует шину событий @ref ISubject для отчетов об успехе, информации или сообщениях об ошибках.
 * Эта команда использует зависимость `RepositoryManager` косвенно, так как логика
 * достаточно проста для самостоятельных операций файловой системы, но полагается на
 * соглашения о путях менеджера.
 */

#include "../include/HubCommand.hxx"
#include "../../services/Event.hxx"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace svcs::server::cli {

namespace fs = std::filesystem;

HubCommand::HubCommand(std::shared_ptr<ISubject> event_bus,
                       std::shared_ptr<RepositoryManager> repository_manager)
    : ServerBaseCommand(std::move(event_bus), std::move(repository_manager))
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

    // Use notifyInfo for success since ServerBaseCommand doesn't have notifySuccess
    notifyInfo("Hub repository successfully created at " + repo_dir.string());
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

bool HubCommand::initializeHubRepository(const std::filesystem::path& repo_path) const {
    // This method is kept for interface compatibility but cannot call execute() directly
    // since execute() is not const. We'll implement the logic here instead.

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
        if (!createHubDirectoryStructure(repo_dir)) {
            return false;
        }

        if (!createHubConfigFile(repo_dir)) {
            return false;
        }

        if (!createHubHEADFile(repo_dir)) {
            return false;
        }

        // Create description file
        std::ofstream description(repo_dir / "description");
        description << "Unnamed repository; edit this file to name the repository.\n";
        description.close();

    } catch (const std::exception& e) {
        notifyError(std::string("Initialization failed: ") + e.what());
        fs::remove_all(repo_dir);
        return false;
    }

    notifyInfo("Hub repository successfully created at " + repo_dir.string());
    return true;
}

bool HubCommand::createHubDirectoryStructure(const std::filesystem::path& svcs_path) const {
    try {
        fs::create_directories(svcs_path / "objects");
        fs::create_directories(svcs_path / "refs" / "heads");
        fs::create_directories(svcs_path / "refs" / "tags");
        fs::create_directories(svcs_path / "hooks");
        fs::create_directories(svcs_path / "info");
        return true;
    } catch (const std::exception& e) {
        notifyError(std::string("Failed to create directory structure: ") + e.what());
        return false;
    }
}

bool HubCommand::createHubConfigFile(const std::filesystem::path& svcs_path) const {
    try {
        std::ofstream config(svcs_path / "config");
        config << "[core]\n"
               << "bare = true\n"
               << "repositoryformatversion = 0\n"
               << "[svcs]\n"
               << "hub = true\n";
        return true;
    } catch (const std::exception& e) {
        notifyError(std::string("Failed to create config file: ") + e.what());
        return false;
    }
}

bool HubCommand::createHubHEADFile(const std::filesystem::path& svcs_path) const {
    try {
        std::ofstream head(svcs_path / "HEAD");
        head << "ref: refs/heads/main\n";
        return true;
    } catch (const std::exception& e) {
        notifyError(std::string("Failed to create HEAD file: ") + e.what());
        return false;
    }
}

bool HubCommand::isValidHubPath(const std::filesystem::path& path) const {
    return !path.empty();
}

bool HubCommand::isPathAvailable(const std::filesystem::path& path) const {
    if (!fs::exists(path)) {
        return true;
    }

    if (fs::is_directory(path)) {
        return fs::is_empty(path);
    }

    return false;
}

}