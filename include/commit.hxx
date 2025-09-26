/***********************************************************************************************
 * @file Commit.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Definition of the class Commit.
 ***********************************************************************************************/

#pragma once

#include <string>

/**
 * @class Commit
 * @brief It is an immutable snapshot of a file version and its metadata.
 * * The Commit object is the core data structure of the VCS, containing all 
 * necessary information to identify and retrieve a specific version of a file.
 */
class Commit {
private:
    /// @brief Unique commit ID (metadata hash).
    std::string hash_id;
    /// @brief User-provided message when creating the commit.
    std::string user_message;
    /// @brief Exact creation timestamp in YYYY-MM-DD HH:MM:SS format.
    std::string timestamp;
    /// @brief File content hash (BLOB), necessary for integrity checking and deduplication.
    std::string content_hash;
    /// @brief Path to the stored file content (object) in the repository.
    std::string file_path;
public:
    /**
     * @brief Copy constructor.
     * @param c The Commit object to copy.
     */
    Commit(const Commit& c);
    
    /**
     * @brief Primary constructor for creating a Commit object.
     * * @param hid Unique commit ID.
     * @param umsg User message.
     * @param tstamp Timestamp.
     * @param chash File content hash.
     * @param fpath Path to the stored file content.
     */
    Commit(std::string hid, std::string umsg, std::string tstamp, 
           std::string chash, std::string fpath);

    /**
     * @brief Calculates the cryptographic SHA-256 hash for the given content.
     * @param content The string content to be hashed.
     * @return std::string The SHA-256 hash as a 64-character hexadecimal string.
     */
    std::string calculate_hash(std::string content);

    /**
     * @brief Returns the unique commit identifier (hash).
     * @return std::string Commit ID hash.
     */
    std::string get_hash_id() const;
    
    /**
     * @brief Returns the message associated with the commit.
     * @return std::string Commit message.
     */
    std::string get_user_message() const;
    
    /**
     * @brief Returns the commit creation timestamp.
     * @return std::string Timestamp.
     */
    std::string get_timestamp() const;
    
    /**
     * @brief Returns the file content hash (BLOB).
     * @return std::string Content hash.
     */
    std::string get_content_hash() const;
    
    /**
     * @brief Returns the path to the stored file content in the repository.
     * @return std::string File path.
     */
    std::string get_file_path() const;
};