/**
 * @file Commit.hxx
 * @brief Defines the Commit class, representing a permanent snapshot in the VCS history.
 * @details A Commit object links a Tree object (the repository state) with parent commits 
 * and essential metadata like author, timestamp, and the commit message, forming the basis of the project's history.
 */
#pragma once

#include "VcsObject.hxx"
#include <string>
#include <vector>
#include <ctime>

/**
 * @class Commit
 * @brief Represents a single, immutable historical point (snapshot) in the repository's history.
 * * This object binds the content structure (Tree hash) with the metadata (author, message, parent history).
 */
class Commit : public VcsObject {
private:
    /// @brief Hash ID of the root Tree object representing the state of the repository at this commit.
    std::string tree_hash;
    
    /// @brief Hash IDs of the parent commits (usually one for normal commits, or two for merge commits).
    std::vector<std::string> parent_hashes;
    
    /// @brief Name and email of the author (e.g., "John Doe <john.doe@example.com>").
    std::string author;
    
    /// @brief Timestamp (UNIX time) of the commit creation.
    std::time_t timestamp;
    
    /// @brief The commit message provided by the user, often spanning multiple lines.
    std::string message;

public:
    /**
     * @brief Main constructor for the Commit object.
     * @details Initializes the metadata fields, sorts the parent hashes for consistency, and immediately computes the object's hash ID.
     * @param thash Hash of the root Tree object.
     * @param phashes Hashes of the immediate parent commit(s).
     * @param auth Author and email string.
     * @param msg User-defined commit message.
     * @param tstamp Time when the commit was created (default is current time).
     */
    Commit(
        std::string thash,
        std::vector<std::string> phashes,
        std::string auth,
        std::string msg,
        std::time_t tstamp = std::time(nullptr)
    );

    // VcsObject overrides
    
    /**
     * @brief Returns the type of the VCS object.
     * @return std::string Always returns "commit".
     * @copydoc VcsObject::getType()
     */
    std::string getType() const override;

    /**
     * @brief Serializes the Commit metadata into a standardized, canonical format for hashing and storage.
     * @details The serialized format includes key/value pairs (tree, parent, author) followed by the message.
     * @return std::string The standardized, serialized commit data.
     * @copydoc VcsObject::serialize()
     */
    std::string serialize() const override;
    
    /**
     * @brief Creates a Commit object from a serialized string read from the object database.
     * @param raw_content The serialized string data, typically read from a decompressed object file.
     * @return Commit A fully reconstructed Commit object.
     */
    static Commit deserialize(const std::string& raw_content);

    // Getters

    /**
     * @brief Returns the hash ID of the associated root Tree object.
     * @return const std::string& The tree hash.
     */
    const std::string& getTreeHash() const;

    /**
     * @brief Returns the hash IDs of the parent commits.
     * @return const std::vector<std::string>& The list of parent hashes (sorted).
     */
    const std::vector<std::string>& getParentHashes() const;

    /**
     * @brief Returns the author and email string for the commit.
     * @return const std::string& The author string.
     */
    const std::string& getAuthor() const;

    /**
     * @brief Returns the UNIX timestamp of the commit creation.
     * @return std::time_t The timestamp value.
     */
    std::time_t getTimestamp() const;

    /**
     * @brief Returns the user-defined commit message.
     * @return const std::string& The commit message.
     */
    const std::string& getMessage() const;
};