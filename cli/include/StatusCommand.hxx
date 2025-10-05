/**
 * @file StatusCommand.hxx
 * @brief Declaration of the StatusCommand class for showing repository status.
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
 * @brief Command for showing the current status of the repository.
 *
 * The StatusCommand displays:
 * - Current branch
 * - Staged changes (ready to save)
 * - Unstaged changes (not yet added)
 * - Untracked files (new files)
 * - Status of specific files when provided as arguments
 */
class StatusCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Constructs the StatusCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     */
    StatusCommand(std::shared_ptr<ISubject> subject,
                  std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the status command.
     *
     * @param args Command arguments - if provided, shows status for specific files.
     * @return true always, as status command cannot fail.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "status"
     */
    [[nodiscard]] std::string getName() const override { return "status"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Show the working tree status"
     */
    [[nodiscard]] std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs status [file1 file2 ...]"
     */
    [[nodiscard]] std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Shows the full repository status.
     */
    [[nodiscard]] bool showFullStatus() const;
    
    /**
     * @brief Shows status for specific files.
     * @param files List of files to show status for.
     */
    [[nodiscard]] bool showFileStatus(const std::vector<std::string>& files) const;
    
    /**
     * @brief Shows the current branch information.
     */
    void showBranchInfo() const;
    
    /**
     * @brief Shows staged changes (files ready to save).
     */
    void showStagedChanges() const;
    
    /**
     * @brief Shows unstaged changes (modified files not yet added).
     */
    void showUnstagedChanges() const;
    
    /**
     * @brief Shows untracked files (new files not yet added).
     */
    void showUntrackedFiles() const;
    
    /**
     * @brief Gets the current branch name.
     * @return Current branch name, or "main" by default.
     */
    [[nodiscard]] std::string getCurrentBranch() const;
    
    /**
     * @brief Checks if a file has been modified compared to staged version.
     *
     * @param filePath The path to the file to check.
     * @return true if file has been modified, false otherwise.
     */
    [[nodiscard]] bool isFileModified(const std::filesystem::path& filePath) const;
    
    /**
     * @brief Finds all untracked files in the repository.
     *
     * @return Vector of paths to untracked files.
     */
    [[nodiscard]] std::vector<std::filesystem::path> findUntrackedFiles() const;
    
    /**
     * @brief Formats file status with colors/icons (simple version).
     *
     * @param status The status character.
     * @param filePath The file path.
     * @return Formatted status string.
     */
    [[nodiscard]] static std::string formatFileStatus(char status, const std::string& filePath) ;
    
    /**
     * @brief Gets the status of a specific file.
     * @param filePath The file to check.
     * @return Status character and description.
     */
    [[nodiscard]] std::pair<char, std::string> getFileStatus(const std::string& filePath) const;
};