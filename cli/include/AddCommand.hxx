/**
 * @file AddCommand.hxx
 * @brief Declaration of the AddCommand class.
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

/**
 * @brief Defines the status of a file within the SVCS.
 */
enum class FileStatus {
    UNTRACKED, ///< File is present but not tracked by SVCS.
    MODIFIED,  ///< File is tracked and has been modified since the last commit.
    DELETED,   ///< File is tracked but has been deleted from the working directory.
    UNMODIFIED ///< File is tracked and has no changes since the last commit.
};

/**
 * @brief Implements the "add" command for the Simple Version Control System (SVCS).
 *
 * This command is responsible for adding (staging) the specified files
 * to the staging area of the repository for a subsequent commit.
 *
 * The class inherits from ICommand and uses the Command pattern
 * to execute its operations. It depends on ISubject (event bus)
 * for user notifications and RepositoryManager for repository
 * operations.
 */
class AddCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

    /**
     * @brief Structure to hold options for the "add" command.
     */
    struct AddOptions {
        bool dryRun = false;      ///< Don't actually add files, just show what would be done.
        bool interactive = false; ///< Interactively stage changes.
        bool patch = false;       ///< Interactively choose hunks of changes to stage.
        bool update = false;      ///< Only stage files already tracked.
        bool force = false;       ///< Allow adding ignored files.
        bool showHelp = false;    ///< Show help message.
        std::vector<std::string> files;             ///< List of files/directories to add.
        std::vector<std::string> excludePatterns;   ///< Patterns to exclude from adding.
    };

    // New methods
    /**
     * @brief Handles the interactive staging mode.
     * @return true if successful, false otherwise.
     */
    [[nodiscard]] bool addInteractive() const;

    /**
     * @brief Handles the patch mode for staging specific hunks.
     * @param filePath The path to the file to stage hunks from.
     * @return true if successful, false otherwise.
     */
    [[nodiscard]] bool addPatch(const std::string& filePath) const;

    /**
     * @brief Shows the result of a dry run without modifying the index.
     * @param files The list of files that would be processed.
     * @return true if successful, false otherwise.
     */
    [[nodiscard]] bool showDryRun(const std::vector<std::string>& files) const;

    /**
     * @brief Determines if a file should be added based on its status and force flag.
     * @param file The path to the file.
     * @param force True if force flag is set.
     * @return true if the file should be added, false otherwise.
     */
    [[nodiscard]] bool shouldAddFile(const std::string& file, bool force) const;

    // Argument Parsing
    /**
     * @brief Parses the command line arguments into AddOptions structure.
     * @param args The vector of string arguments passed to the command.
     * @return The populated AddOptions structure.
     */
    [[nodiscard]] AddOptions parseArguments(const std::vector<std::string>& args) const;

public:
    /**
     * @brief Constructor for AddCommand.
     * @param subject A shared pointer to the ISubject (event bus).
     * @param repoManager A shared pointer to the RepositoryManager.
     */
    AddCommand(std::shared_ptr<ISubject> subject,
               std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @brief Executes the "add" command with the given arguments.
     * @param args The vector of string arguments for the command.
     * @return true if the command executed successfully, false otherwise.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @brief Retrieves all file paths within a given directory, recursively.
     * @param directory The path to the directory.
     * @return A vector of strings containing all file paths.
     */
    [[nodiscard]] std::vector<std::string> getAllFilesInDirectory(const std::string& directory) const;

    /**
     * @brief Gets the name of the command.
     * @return The command name, "add".
     */
    [[nodiscard]] std::string getName() const override { return "add"; }

    /**
     * @brief Gets a brief description of the command.
     * @return A string describing the command's purpose.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @brief Gets the command's usage syntax.
     * @return A string showing how to use the command.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @brief Displays the detailed help information for the command.
     */
    void showHelp() const override;
};