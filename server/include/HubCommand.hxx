/**
 * @file HubCommand.hxx  
 * @brief Declaration of the HubCommand class for creating central bare repositories.
 *
 * @details The HubCommand implements the 'svcs hub' command for creating 
 * central bare repositories that serve as collaboration hubs for development teams.
 * These repositories lack working directories and are optimized for remote access.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../services/ISubject.hxx" 
#include "../../core/include/RepositoryManager.hxx"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

/**
 * @class HubCommand
 * @brief Command for creating central hub repositories for team collaboration.
 *
 * @ingroup CLI
 *
 * @details Implements the 'svcs hub' functionality to initialize bare repositories
 * that act as central points for pushing, pulling and collaborating on code.
 */
class HubCommand : public ICommand {
public:
    HubCommand(std::shared_ptr<ISubject> event_bus,
               std::shared_ptr<RepositoryManager> repository_manager);
    
    ~HubCommand() override = default;
    
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] std::string getDescription() const override; 
    [[nodiscard]] std::string getUsage() const override;
    bool execute(const std::vector<std::string>& args) override;
    void showHelp() const override;
    
private:
    bool initializeHubRepository(const std::filesystem::path& repo_path) const;
    bool createHubDirectoryStructure(const std::filesystem::path& svcs_path) const;
    bool createHubConfigFile(const std::filesystem::path& svcs_path) const;
    bool createHubHEADFile(const std::filesystem::path& svcs_path) const;
    
    [[nodiscard]] bool isValidHubPath(const std::filesystem::path& path) const;
    [[nodiscard]] bool isPathAvailable(const std::filesystem::path& path) const;
    
    void notifyInfo(const std::string& message) const;
    void notifyError(const std::string& message) const;
    void notifySuccess(const std::string& message) const;
    
    std::shared_ptr<ISubject> event_bus_;
    std::shared_ptr<RepositoryManager> repository_manager_;
};