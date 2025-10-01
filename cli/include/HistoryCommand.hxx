/**
 * @file HistoryCommand.hxx
 * @brief Declaration of the HistoryCommand class for viewing save history (commits).
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

// Assuming CommitInfo is a structure defined elsewhere, like:
/*
struct CommitInfo {
    std::string id;
    std::string parentId;
    std::string author;
    std::string timestamp;
    std::string message;
};
*/

/**
 * @brief Command for viewing the history of saves (commits) in the repository.
 *
 * The **HistoryCommand** displays the chronological history of all saves
 * with their messages, authors, timestamps, and unique identifiers. It supports
 * different output formats, including default, one-line, and detailed views.
 */
class HistoryCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;        ///< Shared pointer to the event bus for user notifications.
    std::shared_ptr<RepositoryManager> repoManager_; ///< Shared pointer to the repository manager for accessing commit data.

public:
    /**
     * @brief Constructor for HistoryCommand.
     * @param subject A shared pointer to the ISubject (event bus).
     * @param repoManager A shared pointer to the RepositoryManager.
     */
    HistoryCommand(std::shared_ptr<ISubject> subject,
                   std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @brief Executes the "history" command with the given arguments.
     *
     * This method is the main entry point for the command, handling argument parsing
     * and delegating to the appropriate history display function.
     *
     * @param args The vector of string arguments for the command (e.g., limit, format options).
     * @return true if the command executed successfully, false otherwise.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @brief Gets the name of the command.
     * @return The command name, "history".
     */
    std::string getName() const override { return "history"; }

    /**
     * @brief Gets a brief description of the command.
     * @return A string describing the command's purpose.
     */
    std::string getDescription() const override;

    /**
     * @brief Gets the command's usage syntax.
     * @return A string showing how to use the command.
     */
    std::string getUsage() const override;

    /**
     * @brief Displays the detailed help information for the command.
     */
    void showHelp() const override;

private:
    /**
     * @brief Parses the command line arguments to determine display options.
     * @param args The command line arguments.
     * @param showOneline Output parameter: true to show history in one line format.
     * @param limit Output parameter: the maximum number of commits to display (0 for no limit).
     * @param showFull Output parameter: true to show history in detailed/full format.
     * @return true if arguments were parsed successfully, false otherwise.
     */
    bool parseArguments(const std::vector<std::string>& args,
                       bool& showOneline, int& limit, bool& showFull) const;

    /**
     * @brief Displays the commit history using the default format.
     * @param entries A vector of CommitInfo objects to display.
     */
    void showDefaultHistory(const std::vector<CommitInfo>& entries) const;

    /**
     * @brief Displays the commit history using the concise one-line format.
     * @param entries A vector of CommitInfo objects to display.
     */
    void showOnelineHistory(const std::vector<CommitInfo>& entries) const;

    /**
     * @brief Displays the commit history using the detailed/full format.
     * @param entries A vector of CommitInfo objects to display.
     */
    void showDetailedHistory(const std::vector<CommitInfo>& entries) const;

    /**
     * @brief Formats a raw timestamp string into a user-friendly readable format.
     * @param timestamp The raw timestamp string (e.g., ISO 8601).
     * @return The formatted timestamp string.
     */
    std::string formatTimestamp(const std::string& timestamp) const;

    /**
     * @brief Truncates a string to a specified maximum length, adding an ellipsis if truncated.
     * @param str The string to truncate.
     * @param length The maximum desired length.
     * @return The truncated string.
     */
    std::string truncateString(const std::string& str, size_t length) const;
};