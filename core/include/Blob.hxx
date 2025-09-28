/**
 * @file Blob.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Defines the Blob class, which represents raw file content in the VCS object database.
 * @details This file provides the contract for storing file data as immutable objects, 
 * central to the data integrity of the system.
 */
#pragma once

#include "VcsObject.hxx" 
#include <string>

/**
 * @class Blob
 * @brief Represents the content of a file (Binary Large Object).
 * * This is the lowest-level object in the VCS, storing only raw file bytes.
 * The Blob object is immutable; its hash is solely dependent on its content.
 */
class Blob : public VcsObject {
private:
    /// @brief The raw content data of the file.
    std::string data; 

public:
    /**
     * @brief Constructor for the Blob object.
     * * Initializes the object and immediately computes its hash ID based on the raw_data.
     * @param raw_data The raw file content to be stored and hashed.
     */
    Blob(const std::string& raw_data);

    // VcsObject overrides
    
    /**
     * @brief Returns the type of the VCS object.
     * @return std::string Always returns "blob".
     * @copydoc VcsObject::getType()
     */
    std::string getType() const override;
    
    /**
     * @brief Serializes the object's core data for hashing and storage.
     * @return std::string Returns the raw file content (data) itself.
     * @copydoc VcsObject::serialize()
     */
    std::string serialize() const override;

    /**
     * @brief Returns the raw data content stored in the Blob.
     * @return const std::string& A constant reference to the file content.
     */
    const std::string& getData() const;
};