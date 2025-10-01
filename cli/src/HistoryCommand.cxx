/**
 * @file HistoryCommand.cxx
 * @brief Implementation of the HistoryCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/HistoryCommand.hxx"
#include "../../services/ISubject.hxx"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

HistoryCommand::HistoryCommand(std::shared_ptr<ISubject> subject,
                               std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(subject), repoManager_(repoManager) {
}

bool HistoryCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "history";
    
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", SOURCE});
        return false;
    }
    
    // Получаем историю коммитов
    auto commits = repoManager_->getCommitHistory();
    if (commits.empty()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "No commits yet.", SOURCE});
        return true;
    }
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Commit history (" + std::to_string(commits.size()) + " commits):", SOURCE});
    
    for (size_t i = 0; i < commits.size(); ++i) {
        const auto& commit = commits[i];
        std::stringstream commitInfo;
        commitInfo << "[" << (i + 1) << "] " << commit.hash.substr(0, 8) << " - " << commit.message;
        commitInfo << " (" << commit.files_count << " files)";
        
        eventBus_->notify({Event::GENERAL_INFO, commitInfo.str(), SOURCE});
    }
    
    return true;
}

std::string HistoryCommand::getDescription() const {
    return "Show history of saves";
}

std::string HistoryCommand::getUsage() const {
    return "svcs history [--oneline|--last N|--full]";
}

void HistoryCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, "Usage: " + getUsage(), "history"});
    eventBus_->notify({Event::GENERAL_INFO, "Description: " + getDescription(), "history"});
    eventBus_->notify({Event::GENERAL_INFO, "Shows the chronological history of all saves in the repository.", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "Options:", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  --oneline, -o    Compact single-line format", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  --last N, -n N   Show last N entries", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  --full, -f       Show full details", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "Examples:", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs history                    # Full history", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs history --oneline          # Compact view", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs history --last 5           # Last 5 saves", "history"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs history -n 3 -o            # Last 3 in compact format", "history"});
}

bool HistoryCommand::parseArguments(const std::vector<std::string>& args,
                                   bool& showOneline, int& limit, bool& showFull) const {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--oneline" || args[i] == "-o") {
            showOneline = true;
        } else if (args[i] == "--full" || args[i] == "-f") {
            showFull = true;
        } else if (args[i] == "--last" || args[i] == "-n") {
            if (i + 1 >= args.size()) {
                eventBus_->notify({Event::ERROR_MESSAGE, 
                                  "Missing value for " + args[i] + " option", "history"});
                return false;
            }
            try {
                limit = std::stoi(args[i + 1]);
                if (limit <= 0) {
                    eventBus_->notify({Event::ERROR_MESSAGE, 
                                      "Limit must be positive number", "history"});
                    return false;
                }
                ++i; // Skip the next argument since we used it
            } catch (const std::exception& e) {
                eventBus_->notify({Event::ERROR_MESSAGE, 
                                  "Invalid number for " + args[i] + " option", "history"});
                return false;
            }
        } else {
            eventBus_->notify({Event::ERROR_MESSAGE, 
                              "Unknown option: " + args[i], "history"});
            eventBus_->notify({Event::GENERAL_INFO, 
                              "Use 'svcs help history' for usage information", "history"});
            return false;
        }
    }
    
    // Validate mutually exclusive options
    if (showOneline && showFull) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Cannot use --oneline and --full together", "history"});
        return false;
    }
    
    return true;
}


std::vector<HistoryCommand::HistoryEntry> HistoryCommand::getHistory() const {
    std::vector<HistoryEntry> entries;
    std::string repoPath = repoManager_->getRepositoryPath();
    
    try {
        // Read current HEAD to find the latest commit
        fs::path headFile = fs::path(repoPath) / ".svcs" / "HEAD";
        std::string currentCommitId = readHeadFile(headFile);
        
        if (currentCommitId.empty()) {
            eventBus_->notify({Event::DEBUG_MESSAGE, 
                              "No commits found in repository", "history"});
            return entries;
        }
        
        // Traverse commit history starting from HEAD
        std::string commitId = currentCommitId;
        while (!commitId.empty()) {
            HistoryEntry entry = readCommit(commitId, repoPath);
            if (!entry.id.empty()) {
                entries.push_back(entry);
                commitId = entry.parentId; // Move to parent commit
            } else {
                break; // Stop if commit cannot be read
            }
            
            // Safety limit to prevent infinite loops
            if (entries.size() > 1000) {
                eventBus_->notify({Event::WARNING_MESSAGE,
                                  "History truncated: too many commits", "history"});
                break;
            }
        }
        
    } catch (const fs::filesystem_error& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error reading history: " + std::string(e.what()), "history"});
    }
    
    return entries;
}

std::string HistoryCommand::readHeadFile(const fs::path& headFile) const {
    if (!fs::exists(headFile)) {
        return "";
    }
    
    try {
        std::ifstream file(headFile);
        std::string line;
        if (std::getline(file, line)) {
            // Format: "ref: refs/heads/main" or direct commit hash
            if (line.find("ref: refs/heads/") == 0) {
                // Read branch file to get commit hash
                std::string branchName = line.substr(16); // After "ref: refs/heads/"
                fs::path branchFile = headFile.parent_path() / "refs" / "heads" / branchName;
                if (fs::exists(branchFile)) {
                    std::ifstream branch(branchFile);
                    if (std::getline(branch, line)) {
                        return line; // Commit hash
                    }
                }
            } else {
                // Direct commit hash in HEAD (detached HEAD state)
                return line;
            }
        }
    } catch (const std::exception& e) {
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Error reading HEAD: " + std::string(e.what()), "history"});
    }
    
    return "";
}

HistoryCommand::HistoryEntry HistoryCommand::readCommit(const std::string& commitId, 
                                                       const std::string& repoPath) const {
    HistoryEntry entry;
    entry.id = commitId;
    
    try {
        // Commit objects are stored in .svcs/objects/ab/cdef123...
        std::string objectPath = commitId.substr(0, 2) + "/" + commitId.substr(2);
        fs::path commitFile = fs::path(repoPath) / ".svcs" / "objects" / objectPath;
        
        if (!fs::exists(commitFile)) {
            eventBus_->notify({Event::DEBUG_MESSAGE, 
                              "Commit object not found: " + commitId, "history"});
            return entry;
        }
        
        std::ifstream file(commitFile);
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.find("message ") == 0) {
                entry.message = line.substr(8); // After "message "
            } else if (line.find("author ") == 0) {
                entry.author = line.substr(7); // After "author "
            } else if (line.find("timestamp ") == 0) {
                entry.timestamp = line.substr(10); // After "timestamp "
            } else if (line.find("parent ") == 0) {
                entry.parentId = line.substr(7); // After "parent "
            } else if (line == "---") {
                break; // End of commit header
            }
        }
        
    } catch (const std::exception& e) {
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Error reading commit " + commitId + ": " + std::string(e.what()), "history"});
    }
    
    return entry;
}


void HistoryCommand::showDefaultHistory(const std::vector<HistoryEntry>& entries) const {
    for (const auto& entry : entries) {
        std::stringstream ss;
        ss << "Save: " << entry.id.substr(0, 7) << " - " << entry.message;
        eventBus_->notify({Event::GENERAL_INFO, ss.str(), "history"});
        
        ss.str("");
        ss << "  Author: " << entry.author << " | Date: " << formatTimestamp(entry.timestamp);
        eventBus_->notify({Event::GENERAL_INFO, ss.str(), "history"});
        
        if (&entry != &entries.back()) {
            eventBus_->notify({Event::GENERAL_INFO, "", "history"}); // Empty line between entries
        }
    }
}

void HistoryCommand::showOnelineHistory(const std::vector<HistoryEntry>& entries) const {
    for (const auto& entry : entries) {
        std::stringstream ss;
        ss << entry.id.substr(0, 7) << " - " << truncateString(entry.message, 50);
        eventBus_->notify({Event::GENERAL_INFO, ss.str(), "history"});
    }
}

void HistoryCommand::showFullHistory(const std::vector<HistoryEntry>& entries) const {
    for (const auto& entry : entries) {
        eventBus_->notify({Event::GENERAL_INFO, "Save ID: " + entry.id, "history"});
        eventBus_->notify({Event::GENERAL_INFO, "Message:  " + entry.message, "history"});
        eventBus_->notify({Event::GENERAL_INFO, "Author:   " + entry.author, "history"});
        eventBus_->notify({Event::GENERAL_INFO, "Date:     " + formatTimestamp(entry.timestamp), "history"});
        if (!entry.parentId.empty()) {
            eventBus_->notify({Event::GENERAL_INFO, "Parent:   " + entry.parentId.substr(0, 7), "history"});
        }
        
        if (&entry != &entries.back()) {
            eventBus_->notify({Event::GENERAL_INFO, "---", "history"});
        }
    }
}

std::string HistoryCommand::formatTimestamp(const std::string& timestamp) const {
    // Simple formatting - could be enhanced with locale-specific formatting
    // For now, just return as-is or do simple reformatting
    if (timestamp.length() > 10) {
        return timestamp.substr(0, 10); // Just the date part
    }
    return timestamp;
}

std::string HistoryCommand::truncateString(const std::string& str, size_t length) const {
    if (str.length() <= length) {
        return str;
    }
    return str.substr(0, length - 3) + "...";
}