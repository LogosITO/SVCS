/**
 * @file SaveCommand.hxx
 * @brief Declaration of the SaveCommand class for saving (committing) staged changes.
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
 * @brief Command for saving (committing) staged changes to the repository.
 *
 * The SaveCommand creates a permanent snapshot of all staged changes
 * with a descriptive message.
 */
class SaveCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Constructs the SaveCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     */
    SaveCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the save command.
     *
     * @param args Command arguments. Expected format: ["-m", "message"] or ["--message", "message"]
     * @return true if save succeeded, false otherwise.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "save"
     */
    [[nodiscard]] std::string getName() const override { return "save"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Save staged changes to the repository"
     */
    [[nodiscard]] std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs save -m \"message\""
     */
    [[nodiscard]] std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Parses command line arguments to extract the commit message.
     *
     * @param args Command arguments.
     * @return The commit message, or empty string if not found.
     */
    [[nodiscard]] static std::string parseMessage(const std::vector<std::string>& args) ;
    
    /**
     * @brief Validates that a message is provided and not empty.
     *
     * @param message The commit message to validate.
     * @return true if message is valid, false otherwise.
     */
    [[nodiscard]] bool validateMessage(const std::string& message) const;
    
    /**
     * @brief Checks if there are any staged changes to save.
     *
     * @return true if there are staged changes, false otherwise.
     */
    [[nodiscard]] bool hasStagedChanges() const;
    
    /**
     * @brief Creates a save point (commit) with the given message.
     *
     * @param message The commit message.
     * @return true if save succeeded, false otherwise.
     */
    [[nodiscard]] bool createSavePoint(const std::string& message) const;
};