/**
 * @file BranchManager.hxx
 * @brief Declaration of the BranchManager class for handling version control branches.
 *
 * @details The BranchManager is responsible for all operations related to managing 
 * branches within the SVCS repository, including creation, deletion, renaming, and 
 * switching. It manages branch metadata in memory and persists changes to the 
 * underlying file system using utility methods for loading and saving state.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../services/ISubject.hxx"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

/**
 * @class BranchManager
 * @brief Core service for managing version control branches (creation, deletion, switching).
 *
 * @ingroup Core
 *
 * @details This class abstracts the details of branch storage and provides a 
 * high-level interface for branch manipulation. It uses an internal map to 
 * track branches and communicates status/errors via the ISubject event bus.
 */
class BranchManager {
public:
    /**
     * @struct Branch
     * @brief Structure to hold the metadata for a single branch.
     */
    struct Branch {
        /// @brief The unique name of the branch.
        std::string name;
        /// @brief The hash of the latest commit on this branch.
        std::string head_commit;
        /// @brief Flag indicating if this is the currently active branch.
        bool is_current;
        
        /**
         * @brief Constructor for the Branch structure.
         * @param name The branch name.
         * @param head_commit The hash of the commit this branch points to.
         * @param is_current Flag indicating if the branch is active.
         */
        Branch(const std::string& name, const std::string& head_commit, bool is_current = false)
            : name(name), head_commit(head_commit), is_current(is_current) {}
    };

    /**
     * @brief Constructs a BranchManager.
     * @param event_bus A shared pointer to the event bus (ISubject) for communication.
     */
    explicit BranchManager(std::shared_ptr<ISubject> event_bus);
    
    // --- Public Branch Operations ---
    
    /**
     * @brief Creates a new branch pointing to a specific commit.
     * @param name The name of the new branch.
     * @param commit_hash The hash of the commit the new branch will point to.
     * @return \c true if the branch was created successfully, \c false otherwise.
     */
    bool createBranch(const std::string& name, const std::string& commit_hash);

    /**
     * @brief Deletes an existing branch.
     * @details Prevents deletion of the current branch unless \c force is true.
     * @param name The name of the branch to delete.
     * @param force If \c true, forces deletion even if the branch is not fully merged (implementation dependent).
     * @return \c true if the branch was deleted successfully, \c false otherwise.
     */
    bool deleteBranch(const std::string& name, bool force = false);

    /**
     * @brief Renames an existing branch.
     * @param old_name The current name of the branch.
     * @param new_name The desired new name for the branch.
     * @return \c true if the branch was renamed successfully, \c false otherwise.
     */
    bool renameBranch(const std::string& old_name, const std::string& new_name);

    /**
     * @brief Switches the active branch to the specified name.
     * @details This typically involves updating the HEAD reference and checking out files 
     * (the file checkout logic may reside in the RepositoryManager).
     * @param name The name of the branch to switch to.
     * @return \c true if the switch was successful, \c false otherwise.
     */
    bool switchBranch(const std::string& name);
    
    // --- Public Branch Queries ---
    
    /**
     * @brief Retrieves a list of all known branches, including their state.
     * @return A vector of Branch structures.
     */
    std::vector<Branch> getAllBranches() const;

    /**
     * @brief Retrieves the name of the currently active branch.
     * @return The string name of the current branch.
     */
    std::string getCurrentBranch() const;

    bool updateBranchHead(const std::string& branch_name, const std::string& commit_hash);

    /**
     * @brief Checks if a branch with the given name exists.
     * @param name The name of the branch to check.
     * @return \c true if the branch exists, \c false otherwise.
     */
    bool branchExists(const std::string& name) const;

    /**
     * @brief Retrieves the commit hash that a specific branch points to.
     * @param branch_name The name of the branch.
     * @return The HEAD commit hash of the branch, or an empty string if the branch doesn't exist.
     */
    std::string getBranchHead(const std::string& branch_name) const;
    
    // --- Public Utility Methods ---

    /**
     * @brief Checks if the given string is a valid name for a new branch.
     * @param name The string to validate.
     * @return \c true if the name is valid, \c false otherwise.
     */
    static bool isValidBranchName(const std::string& name);

    /**
     * @brief Creates a new branch from a specific commit
     * @param name Branch name
     * @param commit_hash Hash of the commit to branch from
     * @return true if successful, false otherwise
     */
    bool createBranchFromCommit(const std::string& name, const std::string& commit_hash);
    
    /**
     * @brief Checks if a commit exists
     * @param commit_hash Hash to check
     * @return true if commit exists, false otherwise
     */
    bool commitExists(const std::string& commit_hash) const;
    
private:
    // --- Private Persistence Methods ---

    /**
     * @brief Loads branch data from the repository's file system into the internal map.
     */
    void loadBranches();

    /**
     * @brief Persists the current state of the internal branch map to the file system.
     */
    void saveBranches();

    /**
     * @brief Loads the name of the currently active branch from the file system.
     */
    void loadCurrentBranch();

    /**
     * @brief Persists the name of the currently active branch to the file system (updates HEAD).
     */
    void saveCurrentBranch();
    
    // --- Private File System Abstraction Methods ---

    /**
     * @brief Reads the entire content of a file.
     * @param path The path to the file.
     * @return The content of the file.
     */
    std::string readFile(const std::string& path) const;

    /**
     * @brief Writes content to a file, overwriting existing content.
     * @param path The path to the file.
     * @param content The string content to write.
     */
    void writeFile(const std::string& path, const std::string& content) const;

    /**
     * @brief Checks if a file exists at the given path.
     * @param path The path to the file.
     * @return \c true if the file exists, \c false otherwise.
     */
    bool fileExists(const std::string& path) const;

    /**
     * @brief Creates a directory.
     * @param path The path of the directory to create.
     */
    void createDirectory(const std::string& path) const;
    
    // --- Private Path Generation Methods ---

    /**
     * @brief Generates the full path to the file where branch metadata is stored.
     * @return The file path string.
     */
    std::string getBranchesFilePath() const;

    /**
     * @brief Generates the full path to the HEAD file, which indicates the current branch.
     * @return The file path string.
     */
    std::string getHeadFilePath() const;

    /**
     * @brief Generates the full path to the directory containing branch data.
     * @return The directory path string.
     */
    std::string getBranchesDirectory() const;
    
    /// @brief Internal map storing all branches, indexed by branch name.
    std::unordered_map<std::string, Branch> branches;

    /// @brief The name of the currently active branch.
    std::string current_branch;

    /// @brief Shared pointer to the event bus for communication.
    std::shared_ptr<ISubject> event_bus;
};