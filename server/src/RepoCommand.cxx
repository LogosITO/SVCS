#include "../include/RepoCommand.hxx"
#include "../../services/Event.hxx"
#include <iostream>

RepoCommand::RepoCommand(std::shared_ptr<ISubject> event_bus,
                       std::shared_ptr<RepositoryManager> repo_manager)
    : event_bus_(std::move(event_bus))
    , repo_manager_(std::move(repo_manager))
{
}

std::string RepoCommand::getName() const {
    return "repo";
}

std::string RepoCommand::getDescription() const {
    return "Manage remote repository connections";
}

std::string RepoCommand::getUsage() const {
    return "svcs repo [add <name> <url> | remove <name> | rename <old> <new> | list]";
}

bool RepoCommand::execute(const std::vector<std::string>& args) {
    // Проверяем, что мы в репозитории
    if (!repo_manager_->isRepositoryInitialized()) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Not in a SVCS repository"});
        return false;
    }

    // Получаем путь к репозиторию и создаем RemoteManager напрямую
    fs::path repo_path = repo_manager_->getRepositoryPath();
    RemoteManager remote_manager(repo_path);

    if (args.empty()) {
        return handleList(remote_manager);
    }

    std::string subcommand = args[0];

    if (subcommand == "add") {
        return handleAdd(remote_manager, args);
    } else if (subcommand == "remove") {
        return handleRemove(remote_manager, args);
    } else if (subcommand == "rename") {
        return handleRename(remote_manager, args);
    } else if (subcommand == "list") {
        return handleList(remote_manager);
    } else {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Unknown subcommand: " + subcommand});
        return false;
    }
}

bool RepoCommand::handleAdd(RemoteManager& remote_manager, const std::vector<std::string>& args) {
    if (args.size() != 3) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Usage: svcs repo add <name> <url>"});
        return false;
    }

    if (remote_manager.addRemote(args[1], args[2])) {
        std::cout << "Remote repository '" << args[1] << "' added: " << args[2] << "\n";
        return true;
    } else {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to add remote repository"});
        return false;
    }
}

bool RepoCommand::handleRemove(RemoteManager& remote_manager, const std::vector<std::string>& args) {
    if (args.size() != 2) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Usage: svcs repo remove <name>"});
        return false;
    }

    if (remote_manager.removeRemote(args[1])) {
        std::cout << "Remote repository '" << args[1] << "' removed\n";
        return true;
    } else {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Remote repository '" + args[1] + "' not found"});
        return false;
    }
}

bool RepoCommand::handleRename(RemoteManager& remote_manager, const std::vector<std::string>& args) {
    if (args.size() != 3) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Usage: svcs repo rename <old-name> <new-name>"});
        return false;
    }

    const std::string& old_name = args[1];
    const std::string& new_name = args[2];

    if (!remote_manager.hasRemote(old_name)) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Remote repository '" + old_name + "' not found"});
        return false;
    }

    if (remote_manager.hasRemote(new_name)) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Remote repository '" + new_name + "' already exists"});
        return false;
    }

    // Get the URL and remove old remote
    std::string url = remote_manager.getRemoteUrl(old_name);
    if (!remote_manager.removeRemote(old_name)) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to remove old remote"});
        return false;
    }

    // Add with new name
    if (remote_manager.addRemote(new_name, url)) {
        std::cout << "Remote repository '" << old_name << "' renamed to '" << new_name << "'\n";
        return true;
    } else {
        // Try to restore old remote if rename failed
        remote_manager.addRemote(old_name, url);
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to rename remote repository"});
        return false;
    }
}

bool RepoCommand::handleList(RemoteManager& remote_manager) {
    auto remotes = remote_manager.listRemotes();

    if (remotes.empty()) {
        std::cout << "No remote repositories configured.\n";
        std::cout << "Use 'svcs repo add <name> <url>' to add one.\n";
    } else {
        std::cout << "Remote repositories:\n";
        for (const auto& remote_name : remotes) {
            std::cout << "  " << remote_name << "\t" << remote_manager.getRemoteUrl(remote_name) << "\n";
        }
    }

    return true;
}

void RepoCommand::showHelp() const {
    std::cout << "Usage: " << getUsage() << "\n\n"
              << "Manage connections to remote repositories.\n\n"
              << "Subcommands:\n"
              << "  add <name> <url>     Add a new remote repository\n"
              << "  remove <name>        Remove a remote repository\n"
              << "  rename <old> <new>   Rename a remote repository\n"
              << "  list                 List all remote repositories\n\n"
              << "Examples:\n"
              << "  svcs repo add origin user@server.com:/repos/project.git\n"
              << "  svcs repo add backup user@backup.com:/backup.git\n"
              << "  svcs repo rename origin primary\n"
              << "  svcs repo remove backup\n"
              << "  svcs repo list\n"
              << std::endl;
}