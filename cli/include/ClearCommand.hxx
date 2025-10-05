/**
 * @file ClearCommand.hxx
 * @brief Declaration of the ClearCommand class for removing SVCS repository.
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
 * @brief Command for removing SVCS repository structure.
 *
 * The ClearCommand safely removes the .svcs directory and all repository data.
 */
class ClearCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @brief Constructs the ClearCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     */
    ClearCommand(std::shared_ptr<ISubject> subject,
                 std::shared_ptr<RepositoryManager> repoManager);
    
    /**
     * @brief Executes the clear command.
     *
     * @param args Command arguments (--force to skip confirmation).
     * @return true if clear succeeded, false otherwise.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "clear"
     */
    [[nodiscard]] std::string getName() const override { return "clear"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Remove SVCS repository from current directory"
     */
    [[nodiscard]] std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs clear [--force]"
     */
    [[nodiscard]] std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Confirms the clear operation with user.
     *
     * @return true if user confirms, false otherwise.
     */
    [[nodiscard]] static bool confirmClear() ;
    
    /**
     * @brief Removes the repository.
     *
     * @return true if removal succeeded, false otherwise.
     */
    [[nodiscard]] bool removeRepository() const;
};