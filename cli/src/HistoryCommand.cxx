/**
 * @file HistoryCommand.cxx
 * @brief Implementation of the HistoryCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/HistoryCommand.hxx"
#include "../../services/ISubject.hxx"

#include <iostream>
#include <sstream>
#include <utility>

HistoryCommand::HistoryCommand(std::shared_ptr<ISubject> subject,
                               std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(std::move(subject)), repoManager_(std::move(repoManager)) {
}

bool HistoryCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "history";

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

    bool showOneline = false;
    bool showFull = false;
    int limit = -1;
    
    if (!parseArguments(args, showOneline, limit, showFull)) {
        return false;
    }
    
    auto commits = repoManager_->getCommitHistory();
    if (commits.empty()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "No commits yet.", SOURCE});
        return true;
    }
    
    if (limit > 0) {
        if (limit < static_cast<int>(commits.size())) {
            commits.resize(limit);
        }
    }

    if (showOneline) {
        showOnelineHistory(commits);
    } else if (showFull) {
        showDetailedHistory(commits);
    } else {
        showDefaultHistory(commits);
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
    eventBus_->notify({Event::HELP_MESSAGE, "Usage: " + getUsage(), "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "Description: " + getDescription(), "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "Shows the chronological history of all saves in the repository.", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "Options:", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  --oneline, -o    Compact single-line format", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  --last N, -n N   Show last N entries", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  --full, -f       Show full details", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "Examples:", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs history                    # Full history", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs history --oneline          # Compact view", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs history --last 5           # Last 5 saves", "history"});
    eventBus_->notify({Event::HELP_MESSAGE, "  svcs history -n 3 -o            # Last 3 in compact format", "history"});
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
            } catch ([[maybe_unused]] const std::exception& e) {
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

void HistoryCommand::showDefaultHistory(const std::vector<CommitInfo>& commits) const {
    const std::string SOURCE = "history";
    
    eventBus_->notify({Event::GENERAL_INFO,
                      "Commit history (" + std::to_string(commits.size()) + " commits):", SOURCE});
    
    for (size_t i = 0; i < commits.size(); ++i) {
        const auto& commit = commits[i];
        std::stringstream commitInfo;
        commitInfo << "[" << (i + 1) << "] " << commit.hash.substr(0, 8) << " - " << commit.message;
        commitInfo << " (" << commit.files_count << " files)";
        
        eventBus_->notify({Event::GENERAL_INFO,
                          commitInfo.str(), SOURCE});
    }
}

void HistoryCommand::showOnelineHistory(const std::vector<CommitInfo>& commits) const {
    const std::string SOURCE = "history";
    
    for (const auto& commit : commits) {
        std::stringstream ss;
        ss << commit.hash.substr(0, 8) << " - " << truncateString(commit.message, 50);
        eventBus_->notify({Event::GENERAL_INFO,
                          ss.str(), SOURCE});
    }
}

void HistoryCommand::showDetailedHistory(const std::vector<CommitInfo>& commits) const {
    const std::string SOURCE = "history";
    
    for (size_t i = 0; i < commits.size(); ++i) {
        const auto& commit = commits[i];
        
        eventBus_->notify({Event::GENERAL_INFO,
                          "Commit " + std::to_string(i + 1) + ":", SOURCE});
        eventBus_->notify({Event::GENERAL_INFO, 
                          "  Hash:    " + commit.hash, SOURCE});
        eventBus_->notify({Event::GENERAL_INFO, 
                          "  Message: " + commit.message, SOURCE});
        eventBus_->notify({Event::GENERAL_INFO, 
                          "  Files:   " + std::to_string(commit.files_count), SOURCE});
        
        if (!commit.author.empty()) {
            eventBus_->notify({Event::GENERAL_INFO, 
                              "  Author:  " + commit.author, SOURCE});
        }
        
        if (!commit.timestamp.empty()) {
            eventBus_->notify({Event::GENERAL_INFO, 
                              "  Date:    " + formatTimestamp(commit.timestamp), SOURCE});
        }

        if (i < commits.size() - 1) {
            eventBus_->notify({Event::GENERAL_INFO, "", SOURCE});
        }
    }
}

std::string HistoryCommand::formatTimestamp(const std::string& timestamp) {
    // Simple formatting - could be enhanced with locale-specific formatting
    // For now, just return as-is or do simple reformatting
    if (timestamp.length() > 10) {
        return timestamp.substr(0, 10); // Just the date part
    }
    return timestamp;
}

std::string HistoryCommand::truncateString(const std::string& str, const size_t length) {
    if (str.length() <= length) {
        return str;
    }
    return str.substr(0, length - 3) + "...";
}