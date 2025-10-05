/**
 * @file ObjectStorage.hxx
 * @brief Manages the reading, writing, and lifecycle of VCS objects on the disk using the Git format.
 *
 * This class handles file path construction, serialization, Zlib compression/decompression,
 * and acts as a factory for restoring concrete VcsObject types (Blob, Tree, Commit).
 * It uses an injected ISubject for internal event logging.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "VcsObject.hxx"
#include <filesystem>
#include <string>
#include <memory>
#include <utility>
#include "../../services/ISubject.hxx"

/**
 * @class ObjectStorage
 * @brief Manages the version control object database, including persistence and object restoration.
 *
 * Implements low-level file system operations (path generation, reading, writing), 
 * compression, decompression, and object deserialization.
 */
class ObjectStorage {
private:
    /// @brief The base path to the object directory (e.g., ".svcs/objects").
    const std::filesystem::path objects_dir;
    
    /// @brief Smart pointer to the ISubject interface for publishing internal events (e.g., success, error).
    std::shared_ptr<ISubject> subject;

    /**
     * @brief Compresses data using Zlib's raw deflate standard (Git format).
     * @param data The raw data (including header) to compress.
     * @return std::string The compressed binary data.
     * @throw std::runtime_error if Zlib compression fails.
     */
    [[nodiscard]] std::string compress(const std::string& data) const;

    /**
     * @brief Decompresses Zlib raw deflate data.
     * @param compressed_data The compressed binary data.
     * @return std::string The decompressed data (including header).
     * @throw std::runtime_error if Zlib decompression fails or stream is corrupt.
     */
    [[nodiscard]] std::string decompress(const std::string& compressed_data) const;
    
    /**
     * @brief Object Factory: Creates a concrete VcsObject from its type and content.
     * * This internal factory is responsible for mapping type strings to concrete VcsObject subclasses.
     * @param type The object type ("blob", "tree", or "commit").
     * @param content The object's serialized data (without header).
     * @return std::unique_ptr<VcsObject> A pointer to the newly created object.
     * @throw std::runtime_error if the type is unknown.
     */
    [[nodiscard]] std::unique_ptr<VcsObject> createObjectFromContent(
        const std::string& type, 
        const std::string& content
    ) const;

public:
    /**
     * @brief Constructor for ObjectStorage.
     * @param root_path The root path of the repository (e.g., the directory containing ".svcs").
     * @param subject Shared pointer to the ISubject interface for event logging. Defaults to nullptr if logging is not required yet.
     */
    explicit ObjectStorage(const std::string& root_path, const std::shared_ptr<ISubject>& subject = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~ObjectStorage();

    /**
     * @brief Sets or replaces the event subject used by the ObjectStorage.
     * @param subj The new ISubject pointer to use for publishing events.
     */
    void setSubject(std::shared_ptr<ISubject> subj) { subject = std::move(subj); };

    /**
     * @brief Forms the full filesystem path for an object based on its hash.
     * @details Uses the first two characters of the hash for the subdirectory name 
     * and the remaining 38 characters for the filename.
     * @param hash The full 40-character SHA-1 hash ID.
     * @return std::string The full, platform-agnostic file path.
     */
    [[nodiscard]] std::string getObjectPath(const std::string& hash) const;

    /**
     * @brief Saves a VcsObject to the object database.
     * @details Serializes the object, prefixes it with a header, compresses it, and writes it to a file. 
     * Publishes events on success/failure.
     * @param obj The VcsObject to save.
     * @return bool True if saving was successful.
     * @throw std::runtime_error if hash is invalid or file IO fails.
     */
    [[nodiscard]] virtual bool saveObject(const VcsObject& obj) const;

    /**
     * @brief Loads an object from the disk by its hash ID.
     * @details Reads the compressed file, decompresses it, performs integrity checks, and deserializes the object 
     * using the internal factory method.
     * @param hash The hash ID of the object to load.
     * @return std::unique_ptr<VcsObject> The restored object instance.
     * @throw std::runtime_error if the object is not found, corrupted, or invalid.
     */
    [[nodiscard]] virtual std::unique_ptr<VcsObject> loadObject(const std::string& hash) const;

    /**
     * @brief Checks if an object with the given hash exists on disk.
     * @param hash The hash ID to check.
     * @return bool True if the object file exists.
     */
    [[nodiscard]] bool objectExists(const std::string& hash) const;
};