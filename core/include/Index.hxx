/**
 * @file Index.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Definition of IndexEntry data structure and Index struct
 */
#pragma once

#include "../include/ObjectStorage.hxx"

#include <filesystem>
#include <string>
#include <map>

namespace fs = std::filesystem;

/**
 * @brief Represents a single file entry in the staging area (Index).
 * * An IndexEntry stores information about a file that is staged for the next commit, 
 * including its object ID (hash) and file metadata used to detect modifications.
 */
struct IndexEntry {
    /** @brief The SHA-256 hash of the file's content (the Blob object ID). */
    std::string blob_hash;

    /** @brief The file's path, relative to the repository's root directory. */
    fs::path file_path;

    /** @brief The time of the file's last modification, used to detect changes. */
    fs::file_time_type last_modified; 

    /** @brief The size of the file in bytes. */
    long long file_size = 0;
};

/**
 * @brief Manages the Staging Area (Index) of the VCS repository.
 * * The Index class is responsible for reading and writing the list of staged files 
 * to the index file (.svcs/index), managing the 'svcs add' operation, and providing 
 * the necessary structure for commit creation.
 */
class Index {
private:
    /** @brief The map holding all staged entries, keyed by file_path. */
    std::map<fs::path, IndexEntry> entries;
    
    /** @brief The full path to the index file (e.g., .svcs/index). */
    fs::path index_file_path;
    
    /** @brief The root path of the working directory. */
    fs::path repo_root_path;

    /** @brief Reference to the ObjectStorage manager for saving Blobs. */
    ObjectStorage& storage_;

    /**
     * @brief Retrieves the size and last write time of a file from the disk.
     * @param full_path The absolute path to the file.
     * @param size Reference to store the file size.
     * @param mtime Reference to store the last modified time.
     */
    static void getFileMetaData(const fs::path& full_path, 
        long long& size, fs::file_time_type& mtime);
        
public:
    /**
     * @brief Constructs the Index manager, setting up paths and loading the index.
     * * @param vcs_root_path The path to the VCS control directory (e.g., .svcs).
     * @param repo_root_path The path to the repository's working directory root.
     * @param storage Reference to the ObjectStorage instance.
     */
    Index(const fs::path& vcs_root_path, const fs::path& repo_root_path, ObjectStorage& storage);
    
    /** @brief Destructor. */
    ~Index();

    /**
     * @brief Adds or updates an entry in the staging area.
     * @param entry The IndexEntry object to be added or updated.
     */
    void addEntry(const IndexEntry& entry);

    /**
     * @brief Retrieves an entry from the index by its relative file path.
     * @param file_path The relative path of the file.
     * @return Pointer to the constant IndexEntry, or nullptr if not found.
     */
    const IndexEntry* getEntry(const fs::path& file_path) const;

    /**
     * @brief Loads the index entries from the index file on disk (temporary text format).
     * @throw std::runtime_error If the index file cannot be read.
     */
    void load();
    
    /**
     * @brief Saves the current state of the index entries to the index file (temporary text format).
     * @throw std::runtime_error If the index file cannot be written.
     */
    void save() const;
    /**
     * @brief Another saving function (like load) but returns bool.
     * @throw std::runtime_error If the index file cannot be written.
     */
    bool write() const;

    /**
     * @brief Checks if a file in the working directory has been modified since it was last staged.
     * @param file_path The relative path of the file to check.
     * @return true If the file is modified, deleted, or untracked; false otherwise.
     */
    bool isFileModified(const fs::path& file_path) const;
    
    /**
     * @brief Stages a file by creating a Blob object, saving it to storage, and 
     * updating the Index entry (Core logic of 'svcs add').
     * @param relative_path The path to the file, relative to the repository's root directory.
     * @throw std::runtime_error If the path is invalid or if read/write operations fail.
     */
    void stage_file(const fs::path& relative_path); 

    /**
     * @brief Creates the hierarchical Tree structure based on the current staged entries 
     * and saves the Tree objects to ObjectStorage.
     * @return The SHA-1 hash (ID) of the final root Tree object.
     * @throw std::runtime_error If the index is empty or tree creation fails.
     */
    std::string createTreeObject();
};
