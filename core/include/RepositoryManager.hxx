/**
 * @file RepositoryManager.hxx
 * @brief Declaration of the RepositoryManager class, managing all repository-level file operations.
 *
 * @details This class is the core service provider for the SVCS application, handling 
 * all low-level file system interactions related to the repository structure, staging area, 
 * commits, and history. It relies on the ISubject interface for all internal logging and 
 * error reporting.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once 

#include "../../services/ISubject.hxx"

#include <optional>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

/**
 * @struct CommitInfo
 * @brief Structure containing essential metadata for a single commit.
 */
struct CommitInfo {
    /// @brief The unique identifier (hash) of the commit.
    std::string hash;
    /// @brief The message provided by the user when creating the commit.
    std::string message;
    /// @brief The count of files included in this commit.
    int files_count = 0;
    /// @brief The author or user who created the commit.
    std::string author;
    /// @brief Timestamp of when the commit was created.
    std::string timestamp;
    /// @brief The branch this commit belongs to.
    std::string branch;
};

/**
 * @class RepositoryManager
 * @brief Manages all physical file and directory operations within the SVCS repository.
 *
 * @ingroup Core
 *
 * @details The RepositoryManager is the core layer responsible for interacting directly with 
 * the file system for tasks like initialization, path management, staging, committing, and logging.
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

    /**
     * @brief Gets the current branch name from HEAD file.
     * @return The current branch name.
     */
    std::string getCurrentBranchFromHead();

public:
    /**
     * @brief Constructs a RepositoryManager.
     * @param bus A shared pointer to the event bus (ISubject) for communication.
     */
    explicit RepositoryManager(std::shared_ptr<ISubject> bus);
    
    /**
     * @brief Updates HEAD reference.
     * @param commit_hash The commit hash to set as HEAD.
     */
    void updateHead(const std::string& commit_hash);
    
    /**
     * @brief Initializes a new SVCS repository in the specified path.
     * @details Creates the necessary internal directories and files (e.g., .svcs/).
     * @param path The directory path where the repository should be created.
     * @param force If true, forces initialization even if the directory exists (implementation dependent).
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
     * @brief Updates branch reference file with commit hash.
     * @param branchName The name of the branch to update.
     * @param commitHash The commit hash to set as branch head.
     */
    void updateBranchReference(const std::string& branchName, const std::string& commitHash);
    
    /**
     * @brief Returns the determined root path of the currently active repository.
     * @return The string path to the repository root.
     */
    std::filesystem::path getRepositoryPath() const;
    
    // --- Staging and History Management Methods ---
    
    /**
     * @brief Attempts to add a file to the staging area (index).
     * @details This typically involves reading the file and updating the index state.
     * @param filePath The path to the file to be staged.
     * @return \c true if the file was successfully added, \c false otherwise.
     */
    bool addFileToStaging(const std::string& filePath);

    /**
     * @brief Retrieves the hash of the current HEAD commit in the repository.
     * @return The commit hash string.
     */
    std::string getHeadCommit();

    /**
     * @brief Creates a commit from the currently staged files.
     * @param message Commit message provided by the user.
     * @return The newly created commit hash if successful, empty string otherwise.
     */
    std::string createCommit(const std::string& message);

    /**
     * @brief Updates commit references when a commit is removed.
     * @param removedCommitHash The hash of the commit being removed.
     * @param newParentHash The new parent hash for dependent commits.
     */
    void updateCommitReferences(const std::string& removedCommitHash, const std::string& newParentHash);

    /**
     * @brief Reverts the repository state to a previous commit.
     * @details This is typically used to undo the effects of a previous 'save' (commit).
     * @param commit_hash The hash of the commit to revert to (or the one before the commit to undo).
     * @return \c true if the revert was successful, \c false otherwise.
     */
    bool revertCommit(const std::string& commit_hash);

    /**
     * @brief Retrieves the CommitInfo structure for a given commit hash.
     * @param commit_hash The hash of the commit to retrieve.
     * @return An optional containing the CommitInfo if found, or std::nullopt otherwise.
     */
    std::optional<CommitInfo> getCommitByHash(const std::string& commit_hash);

    /**
     * @brief Retrieves the hash of the parent commit for a given commit.
     * @param commit_hash The hash of the child commit.
     * @return The parent commit hash string (empty if it is the initial commit).
     */
    std::string getParentCommitHash(const std::string& commit_hash);

    std::string generateCommitHash(const std::string& content);
    /**
     * @brief Restores the working directory files to the state recorded in a specific commit.
     * @param commit The CommitInfo structure representing the target state.
     * @return \c true if files were restored successfully, \c false otherwise.
     */
    bool restoreFilesFromCommit(const CommitInfo& commit);

    /**
     * @brief Clears the contents of the staging area (index).
     * @return \c true if the staging area was successfully cleared, \c false otherwise.
     */
    bool clearStagingArea();

    /**
     * @brief Creates a commit from staged changes. (Convenience method often used by the SaveCommand).
     * @param message Commit message.
     * @return \c true if the save (commit) was successful, \c false otherwise.
     */
    bool saveStagedChanges(const std::string& message);
    
    /**
     * @brief Retrieves a list of all files currently marked as staged (indexed).
     * @return A vector of strings containing the file paths in the staging area.
     */
    std::vector<std::string> getStagedFiles();

    /**
     * @brief Retrieves the complete commit history for the current branch.
     * @return Vector of CommitInfo objects representing the commit history, typically newest first.
     */
    std::vector<CommitInfo> getCommitHistory();

    /**
     * @brief Retrieves the commit history for a specific branch.
     * @param branch_name The name of the branch.
     * @return Vector of CommitInfo objects for the branch.
     */
    std::vector<CommitInfo> getBranchHistory(const std::string& branch_name);

    /**
     * @brief Gets the current branch name.
     * @return The current branch name.
     */
    std::string getCurrentBranch();

    /**
     * @brief Attempts to recursively remove the entire SVCS repository structure (e.g., the .svcs directory) and its contents.
     * @param path The root path of the repository to remove.
     * @return \c true if removal was successful, \c false otherwise.
     */
    bool removeRepository(const std::filesystem::path& path);

    /**
     * @brief Checks if a branch exists.
     * @param branch_name The branch name to check.
     * @return true if branch exists, false otherwise.
     */
    bool branchExists(const std::string& branch_name);
    
    /**
     * @brief Gets the head commit of a branch.
     * @param branch_name The branch name.
     * @return The commit hash of the branch head.
     */
    std::string getBranchHead(const std::string& branch_name);
    
    /**
     * @brief Gets the content of a file at a specific commit.
     * @param commit_hash The commit hash.
     * @param file_path The file path.
     * @return The file content.
     */
    std::string getFileContentAtCommit(const std::string& commit_hash, const std::string& file_path);
    
    /**
     * @brief Gets all files changed in a commit.
     * @param commit_hash The commit hash.
     * @return Vector of file paths.
     */
    std::vector<std::string> getCommitFiles(const std::string& commit_hash);
    
    /**
     * @brief Sets merge state (for conflict resolution).
     * @param branch_name Branch being merged.
     * @param commit_hash Commit being merged.
     */
    void setMergeState(const std::string& branch_name, const std::string& commit_hash);
    
    /**
     * @brief Clears merge state (after merge completion/abort).
     */
    void clearMergeState();
    
    /**
     * @brief Checks if a merge is in progress.
     * @return true if merge is in progress, false otherwise.
     */
    bool isMergeInProgress();
    
    /**
     * @brief Gets the branch being merged.
     * @return Branch name if merge in progress, empty string otherwise.
     */
    std::string getMergeBranch();
};