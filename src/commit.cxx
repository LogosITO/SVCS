/***********************************************************************************************
 * @file Commit.сxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Implementation of the class Commit.
 ***********************************************************************************************/

#include "../include/commit.hxx"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

// --- Constructors ---

/**
 * @brief Copy constructor.
 * @param c The Commit object to copy.
 */
Commit::Commit(const Commit& c) {
    this->hash_id = c.hash_id;
    this->user_message = c.user_message;
    this->timestamp = c.timestamp;
    this->content_hash = c.content_hash;
    this->file_path = c.file_path;
}

/**
 * @brief Primary constructor for initializing a Commit object.
 * @param hid Unique commit ID (hash of metadata).
 * @param umsg User-provided commit message.
 * @param tstamp Commit creation timestamp.
 * @param chash SHA-256 hash of the file content.
 * @param fpath Path where the content (BLOB) is stored on disk.
 */
Commit::Commit(
    std::string hid, std::string umsg, std::string tstamp, 
    std::string chash, std::string fpath
) {
    this->hash_id = hid;
    this->user_message = umsg;
    this->timestamp = tstamp;
    this->content_hash = chash;
    this->file_path = fpath;
}

// --- Utility Functions ---

/**
 * @brief Calculates the cryptographic SHA-256 hash for the given content.
 *
 * This function uses the OpenSSL library to produce a unique and tamper-proof 
 * identifier for the file content, represented as a 64-character hexadecimal string.
 * * @param content The string content to be hashed.
 * @return std::string The SHA-256 hash.
 */
std::string Commit::calculate_hash(std::string content) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*) content.data(), content.length(), hash);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << (int) hash[i];
    }
    return ss.str();
}

// --- Getters ---

/**
 * @brief Returns the unique commit identifier (hash).
 * @return std::string Commit ID hash.
 */
std::string Commit::get_hash_id() const { return this->hash_id; }   

/**
 * @brief Returns the message associated with the commit.
 * @return std::string Commit message.
 */
std::string Commit::get_user_message() const { return this->user_message; }

/**
 * @brief Returns the commit creation timestamp.
 * @return std::string Timestamp.
 */
std::string Commit::get_timestamp() const { return this->timestamp; }

/**
 * @brief Returns the file content hash (BLOB).
 * @return std::string Content hash.
 */
std::string Commit::get_content_hash() const { return this->content_hash; }

/**
 * @brief Returns the path to the stored file content in the repository.
 * @return std::string File path.
 */
std::string Commit::get_file_path() const { return this->file_path; }
