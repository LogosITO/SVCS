/**
 * @file HubCommand.hxx  
 * @brief Declaration of the HubCommand class for creating central bare repositories.
 *
 * @details The HubCommand implements the 'svcs hub' command for creating 
 * central bare repositories that serve as collaboration hubs for development teams.
 * These repositories lack working directories and are optimized for remote access.
 * The command creates the necessary directory structure and configuration files
 * required for a functional central repository.
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
 * This command is essential for setting up remote collaboration infrastructure.
 */
class HubCommand : public ICommand {
public:
    /**
     * @brief Constructs a HubCommand with necessary dependencies.
     *
     * @param event_bus The event bus for notification system, used for user feedback.
     * @param repository_manager The repository manager for core repository operations.
     */
    HubCommand(std::shared_ptr<ISubject> event_bus,
               std::shared_ptr<RepositoryManager> repository_manager);

    /**
     * @brief Default destructor.
     */
    ~HubCommand() override = default;

    /**
     * @brief Gets the name of the command.
     *
     * @return std::string The command name "hub".
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @brief Gets a brief description of the command's purpose.
     *
     * @return std::string Description of the hub command functionality.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @brief Gets the usage syntax for the command.
     *
     * @return std::string The usage pattern for the hub command.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @brief Executes the hub command with provided arguments.
     *
     * @param args Command line arguments passed to the hub command.
     * @return bool True if command executed successfully, false otherwise.
     *
     * @throws std::filesystem_error If filesystem operations fail.
     * @throws std::runtime_error If repository initialization fails.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @brief Displays help information for the hub command.
     *
     * Shows usage examples, argument descriptions, and command purpose
     * to assist users in proper command usage.
     */
    void showHelp() const override;

private:
    /**
     * @brief Initializes a new hub repository at the specified path.
     *
     * @param repo_path The filesystem path where to create the hub repository.
     * @return bool True if repository was successfully initialized, false otherwise.
     *
     * @details Creates the complete directory structure and configuration files
     * required for a functional bare repository.
     */
    bool initializeHubRepository(const std::filesystem::path& repo_path) const;

    /**
     * @brief Creates the directory structure for a hub repository.
     *
     * @param svcs_path The path to the .svcs directory of the repository.
     * @return bool True if directory structure was created successfully.
     *
     * @details Creates objects, refs/heads, refs/tags, hooks, and info directories.
     */
    bool createHubDirectoryStructure(const std::filesystem::path& svcs_path) const;

    /**
     * @brief Creates the configuration file for the hub repository.
     *
     * @param svcs_path The path to the .svcs directory of the repository.
     * @return bool True if config file was created successfully.
     *
     * @details Creates a config file with repository format version,
     * file mode settings, and bare repository flag.
     */
    bool createHubConfigFile(const std::filesystem::path& svcs_path) const;

    /**
     * @brief Creates the HEAD file pointing to the default branch.
     *
     * @param svcs_path The path to the .svcs directory of the repository.
     * @return bool True if HEAD file was created successfully.
     *
     * @details Creates HEAD file that references the main branch as default.
     */
    bool createHubHEADFile(const std::filesystem::path& svcs_path) const;

    /**
     * @brief Validates if the provided path is suitable for a hub repository.
     *
     * @param path The filesystem path to validate.
     * @return bool True if path is valid for repository creation.
     *
     * @details Checks if path is not empty and can be normalized to absolute path.
     */
    [[nodiscard]] bool isValidHubPath(const std::filesystem::path& path) const;

    /**
     * @brief Checks if the target path is available for repository creation.
     *
     * @param path The filesystem path to check for availability.
     * @return bool True if path is available (doesn't exist or is empty directory).
     *
     * @details Ensures we don't overwrite existing non-empty directories.
     */
    [[nodiscard]] bool isPathAvailable(const std::filesystem::path& path) const;

    /**
     * @brief Sends an informational message through the event bus.
     *
     * @param message The informational message to display.
     */
    void notifyInfo(const std::string& message) const;

    /**
     * @brief Sends an error message through the event bus.
     *
     * @param message The error message to display.
     */
    void notifyError(const std::string& message) const;

    /**
     * @brief Sends a success message through the event bus.
     *
     * @param message The success message to display.
     */
    void notifySuccess(const std::string& message) const;

    /// @brief Event bus for user notifications and feedback.
    std::shared_ptr<ISubject> event_bus_;

    /// @brief Repository manager for core repository operations.
    std::shared_ptr<RepositoryManager> repository_manager_;
};