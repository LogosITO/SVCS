/**
 * @file RepositoryManager.hxx
 * @brief Declaration of the RepositoryManager class, managing all repository-level file operations.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once 

#include "../../services/ISubject.hxx"

#include <string>
#include <filesystem>
#include <vector>
#include <memory>

struct CommitInfo {
    std::string hash;
    std::string message;
    int files_count = 0;
    std::string author;
    std::string timestamp;
};

/**
 * @brief Manages all physical file and directory operations within the SVCS repository.
 *
 * The RepositoryManager is the core layer responsible for interacting directly with 
 * the file system for tasks like initialization, path management, staging, and logging.
 * It is typically a singleton or a globally available service.
 */
class RepositoryManager {
private:
    /** @brief The full path to the root of the currently active SVCS repository (.svcs directory). */
    std::string currentRepoPath;
    
    /** @brief Shared pointer to the event bus for internal logging and notifications. */
    std::shared_ptr<ISubject> eventBus;
    
    // --- Private Utility Methods for Logging ---
    
    /**
     * @brief Notifies the event bus with an error message.
     * @param message The error message to be logged.
     */
    void logError(const std::string& message);
    
    /**
     * @brief Notifies the event bus with a debug message.
     * @param message The debug message to be logged.
     */
    void logDebug(const std::string& message);
    
    /**
     * @brief Notifies the event bus with an informational message.
     * @param message The information message to be logged.
     */
    void logInfo(const std::string& message);
    
    // --- Private Utility Methods for File System Operations ---
    
    /**
     * @brief Attempts to create a directory at the specified path.
     * @param path The filesystem path where the directory should be created.
     * @return \c true if creation succeeded or the directory already exists, \c false otherwise.
     */
    bool createDirectory(const std::filesystem::path& path);
    
    /**
     * @brief Attempts to create a file at the specified path with optional content.
     * @param path The filesystem path where the file should be created.
     * @param content Optional string content to write to the file.
     * @return \c true if the file was created or opened successfully, \c false otherwise.
     */
    bool createFile(const std::filesystem::path& path, const std::string& content = "");

public:
    /**
     * @brief Constructs a RepositoryManager.
     * @param bus A shared pointer to the event bus (ISubject) for communication.
     */
    explicit RepositoryManager(std::shared_ptr<ISubject> bus);
    
    // --- Public Repository Management Methods ---
    
    /**
     * @brief Initializes a new SVCS repository in the specified path.
     * * Creates the necessary internal directories and files (e.g., .svcs/).
     * @param path The directory path where the repository should be created.
     * @param force If true, forces initialization even if the directory exists. (Currently unused in method signature, but common pattern).
     * @return \c true if initialization was successful, \c false otherwise.
     */
    bool initializeRepository(const std::string& path, bool force = false);
    
    /**
     * @brief Checks if a SVCS repository is initialized in the given path or its parents.
     * @param path The path to start searching from (defaults to the current directory ".").
     * @return \c true if a repository structure is found, \c false otherwise.
     */
    bool isRepositoryInitialized(const std::string& path = ".");
    
    /**
     * @brief Returns the determined root path of the currently active repository.
     * @return The string path to the repository root.
     */
    std::string getRepositoryPath() const { return currentRepoPath; }
    
    // --- Public Staging Management Methods ---
    
    /**
     * @brief Attempts to add a file to the staging area (index).
     * * This typically involves reading the file and updating the index state.
     * @param filePath The path to the file to be staged.
     * @return \c true if the file was successfully added, \c false otherwise.
     */
    bool addFileToStaging(const std::string& filePath);

    /**
     * @brief Get the current HEAD commit hash
     */
    std::string getHeadCommit();

    /**
     * @brief Create a commit from staged files
     * @param message Commit message
     * @return Commit hash if successful, empty string otherwise
     */
    std::string createCommit(const std::string& message);

    /**
     * @brief Clear the staging area
     */
    bool clearStagingArea();

    /**
     * @brief Save staged changes (convenience method for SaveCommand)
     */
    bool saveStagedChanges(const std::string& message);
    
    /**
     * @brief Retrieves a list of all files currently marked as staged (indexed).
     * @return A vector of strings containing the file paths in the staging area.
     */
    std::vector<std::string> getStagedFiles();

    /**
     * @brief Get the commit history
     * @return Vector of CommitInfo objects representing the commit history
     */
    std::vector<CommitInfo> getCommitHistory();

    /**
     * @brief Attempts to recursively remove the repository structure (e.g., the .svcs directory).
     * @param path The root path of the repository to remove.
     * @return \c true if removal was successful, \c false otherwise.
     */
    bool removeRepository(const std::filesystem::path& path);
};