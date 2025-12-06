/**
 * @file BranchCommand.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the BranchCommand for managing branches.
 * @details This command acts as the command-line interface for branch operations.
 * It parses command-line arguments to determine the required action (list, create,
 * delete, rename, switch, or show current) and calls the appropriate method
 * on the core BranchManager.
 *
 * @russian
 * @brief Реализация BranchCommand для управления ветками.
 * @details Эта команда действует как интерфейс командной строки для операций с ветками.
 * Она разбирает аргументы командной строки для определения требуемого действия (список, создание,
 * удаление, переименование, переключение или показ текущей) и вызывает соответствующий метод
 * в основном BranchManager.
 */

#include "../include/BranchCommand.hxx"
#include "../../services/Event.hxx"

#include <sstream>

namespace svcs::cli {

using namespace svcs::core;
using namespace svcs::services;

BranchCommand::BranchCommand(std::shared_ptr<ISubject> event_bus,
                           std::shared_ptr<BranchManager> branch_manager)
    : event_bus_(std::move(event_bus))
    , branch_manager_(std::move(branch_manager)) {}

std::string BranchCommand::getName() const {
    return "branch";
}

std::string BranchCommand::getDescription() const {
    return "List, create, delete, or rename branches";
}

std::string BranchCommand::getUsage() const {
    return "svcs branch [<branch-name>] [-d | --delete <branch-name>] "
           "[-m | --move <old-name> <new-name>] [-c | --show-current] "
           "[-f | --force]";
}

bool BranchCommand::createBranchFromCommit(const std::string& branch_name, const std::string& commit_hash) const {
    if (!isValidBranchName(branch_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Invalid branch name: " + branch_name, "branch"});
        }
        return false;
    }
    
    if (branchExists(branch_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Branch already exists: " + branch_name, "branch"});
        }
        return false;
    }
    
    try {
        if (branch_manager_->createBranchFromCommit(branch_name, commit_hash)) {
            std::string short_hash = commit_hash.length() >= 8 ? commit_hash.substr(0, 8) : commit_hash;
            if (event_bus_) {
                event_bus_->notify({Event::Type::GENERAL_INFO, "Created branch '" + branch_name + "' from commit " + short_hash, "branch"});
            }
            return true;
        } else {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to create branch from commit: " + commit_hash, "branch"});
            }
            return false;
        }
        
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to create branch: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::isValidCommitHash(const std::string& hash) {
    // Базовая проверка - хеш должен содержать только hex символы и быть достаточно длинным
    if (hash.length() < 7) return false;
    
    for (char c : hash) { // NOLINT(*-use-anyofallof)
        if (!std::isxdigit(c)) return false;
    }
    
    return true;
}

bool BranchCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return listBranches();
    }
    
    std::string subcommand;
    std::vector<std::string> branch_names;
    std::string commit_hash;
    bool force_delete = false;
    bool show_current = false;
    bool create_from_commit = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const auto& arg = args[i];
        
        if (arg == "-d" || arg == "--delete") {
            subcommand = "delete";
        } else if (arg == "-h" || arg == "--help") {
            showHelp();
            return true;
        } else if (arg == "-D") {
            subcommand = "delete";
            force_delete = true;
        } else if (arg == "-m" || arg == "--move") {
            subcommand = "rename";
        } else if (arg == "-c" || arg == "--show-current") {
            show_current = true;
        } else if (arg == "-f" || arg == "--force") {
            force_delete = true;
        } else if (arg == "-C" || arg == "--commit") {
            create_from_commit = true;
            if (i + 1 < args.size()) {
                commit_hash = args[++i];
            }
        } else if (!arg.empty() && arg[0] != '-') {
            branch_names.push_back(arg);
        } else {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Unknown option: " + arg, "branch"});
            }
            return false;
        }
    }
    
    if (show_current) {
        return showCurrentBranch();
    }
    
    if (subcommand == "delete") {
        if (branch_names.empty()) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Branch name required for delete operation", "branch"});
            }
            return false;
        }
        return deleteBranch(branch_names[0], force_delete);
    }
    
    if (subcommand == "rename") {
        if (branch_names.size() < 2) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Both old and new branch names required for rename", "branch"});
            }
            return false;
        }
        return renameBranch(branch_names[0], branch_names[1]);
    }

    if (create_from_commit && !branch_names.empty()) {
        if (commit_hash.empty()) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Commit hash required with --commit flag", "branch"});
            }
            return false;
        }
        return createBranchFromCommit(branch_names[0], commit_hash);
    }

    if (branch_names.size() == 2) {
        if (isValidCommitHash(branch_names[1])) {
            return createBranchFromCommit(branch_names[0], branch_names[1]);
        }
    }
    
    // Стандартное поведение
    if (branch_names.size() == 1) {
        if (branchExists(branch_names[0])) {
            return switchBranch(branch_names[0]);
        } else {
            return createBranch(branch_names[0]);
        }
    }
    
    if (event_bus_) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "Invalid arguments for branch command", "branch"});
    }
    showHelp();
    return false;
}

void BranchCommand::showHelp() const {
    event_bus_->notify({Event::HELP_MESSAGE, "Usage: " + getUsage(), "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "Description: " + getDescription(), "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "Options:", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  <branch-name>           Create new branch or switch to existing branch", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  <branch-name> <commit>  Create branch from specific commit", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  -d, --delete <branch>   Delete a branch", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  -D                      Force delete a branch", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  -m, --move <old> <new>  Rename a branch", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  -c, --show-current      Show current branch name", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  -f, --force             Force operation", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  -C, --commit <hash>     Create branch from specific commit", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "Examples:", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs branch                          # List all branches", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs branch feature/new              # Create/switch to branch", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs branch fix-bug abc123def        # Create from commit", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs branch -C abc123def hotfix      # Create from commit", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs branch -d old-branch            # Delete branch", "branch"});
    event_bus_->notify({Event::HELP_MESSAGE, "  svcs branch -c                       # Show current branch", "branch"});
}

bool BranchCommand::listBranches() const {
    try {
        auto branches = branch_manager_->getAllBranches();
        
        if (branches.empty()) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::WARNING_MESSAGE, "No branches found", "branch"});
            }
            return true;
        }
        
        std::stringstream ss;
        ss << "Available branches:\n";
        
        for (const auto& branch : branches) {
            if (branch.is_current) {
                ss << "* " << branch.name << "\n";
            } else {
                ss << "  " << branch.name << "\n";
            }
        }
        
        if (event_bus_) {
            event_bus_->notify({Event::Type::GENERAL_INFO, ss.str(), "branch"});
        }
        return true;
        
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to list branches: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::createBranch(const std::string& branch_name) const {
    if (!isValidBranchName(branch_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Invalid branch name: " + branch_name, "branch"});
        }
        return false;
    }
    
    if (branchExists(branch_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Branch already exists: " + branch_name, "branch"});
        }
        return false;
    }
    
    try {
        // Получаем текущий HEAD коммит через BranchManager
        std::string current_head = branch_manager_->getHeadCommit();
        
        if (current_head.empty()) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Cannot create branch: no commits in repository", "branch"});
            }
            return false;
        }

        if (branch_manager_->createBranchFromCommit(branch_name, current_head)) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::GENERAL_INFO, "Created branch '" + branch_name + "' from current HEAD", "branch"});
            }
            return true;
        } else {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to create branch '" + branch_name + "'", "branch"});
            }
            return false;
        }
        
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to create branch: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::deleteBranch(const std::string& branch_name, bool force) const {
    if (!branchExists(branch_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Branch not found: " + branch_name, "branch"});
        }
        return false;
    }
    
    try {
        if (branch_manager_->deleteBranch(branch_name, force)) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::GENERAL_INFO, "Deleted branch: " + branch_name, "branch"});
            }
            return true;
        } else {
            if (!force) {
                if (event_bus_) {
                    event_bus_->notify({Event::Type::WARNING_MESSAGE, "Branch contains unmerged changes. Use -D to force delete.", "branch"});
                }
            }
            return false;
        }
        
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to delete branch: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::renameBranch(const std::string& old_name, const std::string& new_name) const {
    if (!branchExists(old_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Branch not found: " + old_name, "branch"});
        }
        return false;
    }
    
    if (!isValidBranchName(new_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Invalid new branch name: " + new_name, "branch"});
        }
        return false;
    }
    
    if (branchExists(new_name)) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Branch already exists: " + new_name, "branch"});
        }
        return false;
    }
    
    try {
        if (branch_manager_->renameBranch(old_name, new_name)) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::GENERAL_INFO, "Renamed branch " + old_name + " to " + new_name, "branch"});
            }
            return true;
        } else {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to rename branch", "branch"});
            }
            return false;
        }
        
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to rename branch: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::showCurrentBranch() const {
    try {
        auto current_branch = branch_manager_->getCurrentBranch();
        if (event_bus_) {
            event_bus_->notify({Event::Type::GENERAL_INFO, "Current branch: " + current_branch, "branch"});
        }
        return true;
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to get current branch: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::switchBranch(const std::string& branch_name) const {
    try {
        if (branch_manager_->switchBranch(branch_name)) {
            if (event_bus_) {
                event_bus_->notify({Event::Type::GENERAL_INFO, "Switched to branch: " + branch_name, "branch"});
            }
            return true;
        } else {
            if (event_bus_) {
                event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to switch to branch: " + branch_name, "branch"});
            }
            return false;
        }
    } catch (const std::exception& e) {
        if (event_bus_) {
            event_bus_->notify({Event::Type::ERROR_MESSAGE, "Failed to switch branch: " + std::string(e.what()), "branch"});
        }
        return false;
    }
}

bool BranchCommand::isValidBranchName(const std::string& name) {
    return BranchManager::isValidBranchName(name);
}

bool BranchCommand::branchExists(const std::string& name) const {
    return branch_manager_->branchExists(name);
}

}