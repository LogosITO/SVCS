/**
 * @file StatusCommand.cxx
 * @brief Implementation of the StatusCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/StatusCommand.hxx"
#include "../../services/ISubject.hxx"

#include <algorithm>
#include <fstream>
#include <set>
#include <utility>

namespace fs = std::filesystem;

StatusCommand::StatusCommand(std::shared_ptr<ISubject> subject,
                             std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(std::move(std::move(subject))), repoManager_(std::move(repoManager)) {
}

bool StatusCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "status";

    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return true;
        }
    }
    
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", SOURCE});
        return false;
    }
    
    if (!args.empty()) {
        return showFileStatus(args);
    }
    
    return showFullStatus();
}

bool StatusCommand::showFullStatus() const {
    const std::string SOURCE = "status";
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Repository status:", SOURCE});
    
    showBranchInfo();
    showStagedChanges();
    showUnstagedChanges();
    showUntrackedFiles();
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Use 'svcs add <file>' to stage changes, 'svcs save' to commit.", SOURCE});
    
    return true;
}

bool StatusCommand::showFileStatus(const std::vector<std::string>& files) const {
    const std::string SOURCE = "status";
    
    int shownCount = 0;
    int missingCount = 0;
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "File status:", SOURCE});
    
    for (const auto& file : files) {
        // Проверяем существует ли файл
        if (!fs::exists(file)) {
            eventBus_->notify({Event::WARNING_MESSAGE, 
                              "  [missing] " + file, SOURCE});
            missingCount++;
            continue;
        }
        
        // Получаем статус файла
        auto [status, description] = getFileStatus(file);
        
        // Форматируем вывод с цветами
        std::string color;
        switch (status) {
            case 'A': color = "\033[92m"; break; // Зеленый для staged
            case 'M': color = "\033[93m"; break; // Желтый для modified
            case '?': color = "\033[90m"; break; // Серый для untracked
            case 'D': color = "\033[91m"; break; // Красный для deleted
            default:  color = "\033[0m";  break; // Стандартный цвет
        }
        
        eventBus_->notify({Event::GENERAL_INFO, 
                          "  " + color + description + "\033[0m", SOURCE});
        shownCount++;
    }
    
    // Сводка
    if (shownCount > 0 || missingCount > 0) {
        std::string summary = "Shown " + std::to_string(shownCount) + " file(s)";
        if (missingCount > 0) {
            summary += ", " + std::to_string(missingCount) + " missing";
        }
        eventBus_->notify({Event::GENERAL_INFO, summary, SOURCE});
    } else {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "No files to display.", SOURCE});
    }
    
    return true;
}

std::string StatusCommand::getDescription() const {
    return "Show the working tree status";
}

std::string StatusCommand::getUsage() const {
    return "svcs status [file1 file2 ...]";
}

void StatusCommand::showHelp() const {
    eventBus_->notify({Event::HELP_MESSAGE, "Usage: " + getUsage(), "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "Description: " + getDescription(), "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "Shows the current state of the repository.", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "Options:", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "  [no args]       Show full repository status", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "  <file>          Show status for specific files", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "Examples:", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs status                    Show full status", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs status file.txt           Show status for file.txt", "status"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs status file1.txt file2.txt Show status for multiple files", "status"});
}

// Остальные методы остаются без изменений...

std::pair<char, std::string> StatusCommand::getFileStatus(const std::string& filePath) const {
    auto stagedFiles = repoManager_->getStagedFiles();
    
    // Проверяем staged статус
    bool isStaged = std::find(stagedFiles.begin(), stagedFiles.end(), filePath) != stagedFiles.end();
    
    // Проверяем существует ли файл
    if (!fs::exists(filePath)) {
        if (isStaged) {
            return {'D', "[deleted]   " + filePath + " (staged but file missing)"};
        } else {
            return {'?', "[missing]   " + filePath};
        }
    }
    
    // Проверяем modified статус
    bool isModified = isFileModified(fs::path(filePath));
    
    if (isStaged) {
        if (isModified) {
            return {'M', "[modified]  " + filePath + " (staged but modified)"};
        } else {
            return {'A', "[staged]    " + filePath};
        }
    } else {
        return {'?', "[untracked] " + filePath};
    }
}

// Остальные существующие методы остаются без изменений...
void StatusCommand::showBranchInfo() const {
    std::string branch = getCurrentBranch();
    eventBus_->notify({Event::GENERAL_INFO, "On branch: " + branch, "status"});
}

void StatusCommand::showStagedChanges() const {
    auto stagedFiles = repoManager_->getStagedFiles();
    
    if (!stagedFiles.empty()) {
        eventBus_->notify({Event::GENERAL_INFO, "Staged changes (ready to save):", "status"});
        for (const auto& file : stagedFiles) {
            eventBus_->notify({Event::GENERAL_INFO, "  " + formatFileStatus('A', file), "status"});
        }
    } else {
        eventBus_->notify({Event::GENERAL_INFO, "No staged changes", "status"});
    }
}

void StatusCommand::showUnstagedChanges() const {
    std::string repoPath = repoManager_->getRepositoryPath().string();
    
    try {
        std::vector<std::string> unstagedFiles;
        auto stagedFiles = repoManager_->getStagedFiles();
        
        // Check each staged file for modifications
        for (const auto& stagedFile : stagedFiles) {
            fs::path filePath = fs::path(repoPath) / stagedFile;
            if (fs::exists(filePath) && isFileModified(filePath)) {
                unstagedFiles.push_back(stagedFile);
            } else if (!fs::exists(filePath)) {
                // File was staged but now doesn't exist - it was deleted
                unstagedFiles.push_back(stagedFile + " (deleted)");
            }
        }
        
        if (!unstagedFiles.empty()) {
            eventBus_->notify({Event::GENERAL_INFO, "Unstaged changes (modified files):", "status"});
            for (const auto& file : unstagedFiles) {
                // Determine status based on file name
                char status = 'M'; // Modified by default
                if (file.find("(deleted)") != std::string::npos) {
                    status = 'D';
                }
                eventBus_->notify({Event::GENERAL_INFO, "  " + formatFileStatus(status, file), "status"});
            }
        } else {
            eventBus_->notify({Event::GENERAL_INFO, "No unstaged changes", "status"});
        }
        
    } catch (const fs::filesystem_error& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error checking unstaged changes: " + std::string(e.what()), "status"});
    }
}

void StatusCommand::showUntrackedFiles() const {
    auto untrackedFiles = findUntrackedFiles();
    
    if (!untrackedFiles.empty()) {
        eventBus_->notify({Event::GENERAL_INFO, "Untracked files:", "status"});
        for (const auto& file : untrackedFiles) {
            eventBus_->notify({Event::GENERAL_INFO, "  " + formatFileStatus('?', file.string()), "status"});
        }
    } else {
        eventBus_->notify({Event::GENERAL_INFO, "No untracked files", "status"});
    }
}

std::string StatusCommand::getCurrentBranch() const {
    std::string repoPath = repoManager_->getRepositoryPath().string();
    fs::path headFile = fs::path(repoPath) / ".svcs" / "HEAD";
    
    try {
        if (fs::exists(headFile)) {
            std::ifstream file(headFile);
            std::string line;
            if (std::getline(file, line)) {
                // HEAD file format: "ref: refs/heads/main"
                if (line.find("ref: refs/heads/") == 0) {
                    return line.substr(16); // Extract branch name after "ref: refs/heads/"
                }
            }
        }
    } catch (const std::exception& e) {
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Error reading HEAD file: " + std::string(e.what()), "status"});
    }
    
    return "main"; // Fallback
}

bool StatusCommand::isFileModified(const fs::path& filePath) const { 
    try {
        if (!fs::exists(filePath)) {
            return false; // File doesn't exist - handled separately in showUnstagedChanges
        }
        std::string repoPath = repoManager_->getRepositoryPath().string();
        std::string relativePath = fs::relative(filePath, repoPath).string();
        
        auto stagedFiles = repoManager_->getStagedFiles();
        for (const auto& stagedFile : stagedFiles) {
            if (stagedFile == relativePath) {
                return true; // File is in staging, consider it potentially modified
            }
        }
        
        return false;
        
    } catch (const fs::filesystem_error& e) {
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Error checking file modification: " + std::string(e.what()), "status"});
        return false;
    }
}

std::vector<fs::path> StatusCommand::findUntrackedFiles() const {
    std::vector<fs::path> untrackedFiles;
    std::string repoPath = repoManager_->getRepositoryPath().string();
    
    try {
        auto stagedFiles = repoManager_->getStagedFiles();
        
        // Convert staged files to set for faster lookup
        std::set<std::string> stagedSet(stagedFiles.begin(), stagedFiles.end());
        
        for (const auto& entry : fs::recursive_directory_iterator(repoPath)) {
            if (entry.is_regular_file()) {
                std::string fullPath = entry.path().string();
                
                // Skip .svcs directory and its contents
                if (fullPath.find("/.svcs/") != std::string::npos || 
                    fullPath.find("\\.svcs\\") != std::string::npos) {
                    continue;
                }
                
                // Get relative path from repo root
                fs::path relativePath = fs::relative(entry.path(), repoPath);
                std::string relativeStr = relativePath.string();
                
                // Normalize path separators for comparison
                std::replace(relativeStr.begin(), relativeStr.end(), '\\', '/');
                
                // Check if file is already staged
                if (stagedSet.find(relativeStr) == stagedSet.end()) {
                    untrackedFiles.push_back(relativePath);
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error scanning repository: " + std::string(e.what()), "status"});
    }
    
    return untrackedFiles;
}

std::string StatusCommand::formatFileStatus(char status, const std::string& filePath) {
    switch (status) {
        case 'A': return "[staged]    " + filePath;
        case 'M': return "[modified]  " + filePath;
        case '?': return "[untracked] " + filePath;
        case 'D': return "[deleted]   " + filePath;
        default:  return "[unknown]   " + filePath;
    }
}