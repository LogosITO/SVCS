/**
 * @file RemoveCommand.hxx
 * @brief Declaration of the RemoveCommand class for removing files from staging area.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <string>
#include <vector>

/**
 * @brief Command for removing files from the staging area.
 *
 * The RemoveCommand removes files from the staging area (index),
 * effectively unstaging them while keeping the actual files intact.
 */
class RemoveCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Constructs the RemoveCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     */
    RemoveCommand(std::shared_ptr<ISubject> subject,
                  std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the remove command.
     *
     * @param args Command arguments - file paths to remove from staging.
     * @return true if remove succeeded, false otherwise.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "remove"
     */
    std::string getName() const override { return "remove"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Remove files from staging area"
     */
    std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs remove <file> [file2 ...]"
     */
    std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Removes a single file from staging area.
     *
     * @param filePath The file to remove from staging.
     * @return true if successful, false otherwise.
     */
    bool removeFileFromStaging(const std::string& filePath);
    
    /**
     * @brief Removes all files from staging area.
     *
     * @return true if successful, false otherwise.
     */
    bool removeAllFromStaging();
    
    /**
     * @brief Shows confirmation prompt for removing all files.
     *
     * @return true if user confirms, false otherwise.
     */
    bool confirmRemoveAll() const;
};