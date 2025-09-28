/**
 * @file ObjectStorage.hxx
 * @brief Manages the reading, writing, and lifecycle of VCS objects on the disk using the Git format.
 * @details This class handles file path construction, serialization, Zlib compression/decompression,
 * and acts as a factory for restoring concrete VcsObject types (Blob, Tree, Commit).
 */
#pragma once

#include "VcsObject.hxx"
#include <filesystem>
#include <string>
#include <memory>

class ObjectStorage {
private:
    /// @brief The base path to the object directory (e.g., ".svcs/objects").
    const std::filesystem::path objects_dir;

    /**
     * @brief Compresses data using Zlib's raw deflate standard (Git format).
     * @param data The raw data (including header) to compress.
     * @return std::string The compressed binary data.
     * @throw std::runtime_error if Zlib compression fails.
     */
    std::string compress(const std::string& data) const;

    /**
     * @brief Decompresses Zlib raw deflate data.
     * @param compressed_data The compressed binary data.
     * @return std::string The decompressed data (including header).
     * @throw std::runtime_error if Zlib decompression fails or stream is corrupt.
     */
    std::string decompress(const std::string& compressed_data) const;
    
    /**
     * @brief Object Factory: Creates a concrete VcsObject from its type and content.
     * @param type The object type ("blob", "tree", or "commit").
     * @param content The object's serialized data (without header).
     * @return std::unique_ptr<VcsObject> A pointer to the newly created object.
     * @throw std::runtime_error if the type is unknown.
     */
    std::unique_ptr<VcsObject> createObjectFromContent(
        const std::string& type, 
        const std::string& content
    ) const;

public:
    /**
     * @brief Constructor for ObjectStorage.
     * @param root_path The root path of the repository (e.g., the directory containing ".svcs").
     */
    ObjectStorage(const std::string& root_path);

    /**
     * @brief Forms the full filesystem path for an object based on its hash.
     * @details Uses the first two characters of the hash for the subdirectory name.
     * @param hash The full 40-character SHA-1 hash ID.
     * @return std::string The full, platform-agnostic file path.
     */
    std::string getObjectPath(const std::string& hash) const;

    /**
     * @brief Saves a VcsObject to the object database.
     * @details Serializes the object, prefixes it with a header, compresses it, and writes it to a file.
     * @param obj The VcsObject to save.
     * @return bool True if saving was successful.
     * @throw std::runtime_error if hash is invalid or file IO fails.
     */
    bool saveObject(const VcsObject& obj) const;

    /**
     * @brief Loads an object from the disk by its hash ID.
     * @details Reads the compressed file, decompresses it, performs integrity checks, and deserializes the object.
     * @param hash The hash ID of the object to load.
     * @return std::unique_ptr<VcsObject> The restored object instance.
     * @throw std::runtime_error if the object is not found, corrupted, or invalid.
     */
    std::unique_ptr<VcsObject> loadObject(const std::string& hash) const;

    // --- Convenience and Utility Methods ---

    /**
     * @brief Checks if an object with the given hash exists on disk.
     * @param hash The hash ID to check.
     * @return bool True if the object file exists.
     */
    bool objectExists(const std::string& hash) const;
};