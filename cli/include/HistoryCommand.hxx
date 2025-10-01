/**
 * @file HistoryCommand.hxx
 * @brief Declaration of the HistoryCommand class for viewing save history.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <vector>
#include <string>
#include <filesystem>

/**
 * @brief Command for viewing the history of saves (commits) in the repository.
 *
 * The HistoryCommand displays the chronological history of all saves
 * with their messages, authors, timestamps, and identifiers.
 */
class HistoryCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Constructs the HistoryCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     */
    HistoryCommand(std::shared_ptr<ISubject> subject,
                   std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the history command.
     *
     * @param args Command arguments. Supported options:
     *             - "--oneline" or "-o": Compact single-line format
     *             - "--last N" or "-n N": Show last N entries
     *             - "--full" or "-f": Full detailed output
     * @return true if history displayed successfully, false otherwise.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "history"
     */
    std::string getName() const override { return "history"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Show history of saves"
     */
    std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs history [--oneline|--last N|--full]"
     */
    std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Represents a single save entry in history.
     */
    struct HistoryEntry {
        std::string id;
        std::string message;
        std::string author;
        std::string timestamp;
        std::string parentId;
    };
    
    /**
     * @brief Parses command line arguments.
     *
     * @param args Command arguments.
     * @param showOneline Output parameter for oneline format.
     * @param limit Output parameter for entry limit.
     * @param showFull Output parameter for full details.
     * @return true if arguments parsed successfully, false on error.
     */
    bool parseArguments(const std::vector<std::string>& args,
                       bool& showOneline, int& limit, bool& showFull) const;
    
    /**
     * @brief Retrieves the history entries from repository.
     *
     * @return Vector of history entries in chronological order (newest first).
     */
    std::vector<HistoryEntry> getHistory() const;
    
    /**
     * @brief Reads the current HEAD reference to find the latest commit.
     *
     * @param headFile Path to the HEAD file.
     * @return Commit ID from HEAD, or empty string if not found.
     */
    std::string readHeadFile(const std::filesystem::path& headFile) const;
    
    /**
     * @brief Reads a commit object from the repository.
     *
     * @param commitId The commit ID to read.
     * @param repoPath Path to the repository root.
     * @return HistoryEntry with commit information.
     */
    HistoryEntry readCommit(const std::string& commitId, const std::string& repoPath) const;
    
    /**
     * @brief Reads commit information from a commit file.
     *
     * @param commitFile Path to the commit file.
     * @return HistoryEntry with commit information.
     */
    HistoryEntry readCommitFile(const std::filesystem::path& commitFile) const;
    
    /**
     * @brief Reads commit history from objects directory (Git-like structure).
     *
     * @param objectsDir Path to the objects directory.
     * @return Vector of history entries.
     */
    std::vector<HistoryEntry> readCommitHistoryFromObjects(const std::filesystem::path& objectsDir) const;
    
    /**
     * @brief Reads a commit object file.
     *
     * @param objectFile Path to the object file.
     * @return HistoryEntry with commit information.
     */
    HistoryEntry readCommitObject(const std::filesystem::path& objectFile) const;
    
    /**
     * @brief Displays history in default format.
     *
     * @param entries History entries to display.
     */
    void showDefaultHistory(const std::vector<HistoryEntry>& entries) const;
    
    /**
     * @brief Displays history in compact oneline format.
     *
     * @param entries History entries to display.
     */
    void showOnelineHistory(const std::vector<HistoryEntry>& entries) const;
    
    /**
     * @brief Displays history with full details.
     *
     * @param entries History entries to display.
     */
    void showFullHistory(const std::vector<HistoryEntry>& entries) const;
    
    /**
     * @brief Formats a timestamp for display.
     *
     * @param timestamp Raw timestamp string.
     * @return Formatted timestamp.
     */
    std::string formatTimestamp(const std::string& timestamp) const;
    
    /**
     * @brief Truncates a string to specified length with ellipsis.
     *
     * @param str String to truncate.
     * @param length Maximum length.
     * @return Truncated string.
     */
    std::string truncateString(const std::string& str, size_t length) const;
};