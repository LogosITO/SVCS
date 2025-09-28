/***********************************************************************************************
 * @file VcsObject.h
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Definition of the abstract base class for all Version Control System objects.
 * * This class establishes the contract and core properties (like the hash ID) 
 * required for any object stored in the SVCS object database (e.g., Commit, Blob, Tree).
 ***********************************************************************************************/

#pragma once 

#include <string>

/**
 * @class VcsObject
 * @brief Abstract base class representing any storable, addressable object within the VCS.
 * * VcsObject enforces the mandatory presence of a hash ID, serialization logic, 
 * and type identification for all derived classes. It serves as the core domain 
 * entity for the object database.
 */
class VcsObject {
protected:
    /// @brief The unique cryptographic hash identifier (e.g., SHA-256) for this object. 
    /// Protected to allow internal setting via computeHash() but prevent external modification.
    std::string hash_id;

    /**
     * @brief Computes the cryptographic hash ID for the object's content.
     * * This method should prepend the object's type and content length (Git-style)
     * before hashing the full content and setting the internal hash_id.
     * @param content The serialized string data of the object (usually returned by serialize()).
     */
    void computeHash(const std::string& content);
public:
    /// @brief Default constructor. Required for derived classes.
    VcsObject() = default; 

    /// @brief Copy constructor. Required for derived classes to correctly copy the hash_id.
    /// @param other The VcsObject to copy from.
    VcsObject(const VcsObject& other) = default;

    /**
     * @brief Virtual destructor.
     * Ensures proper cleanup when deleting derived objects via a base class pointer.
     */
    virtual ~VcsObject() = default;

    /**
     * @brief Pure virtual method to serialize the object's core data.
     * * The returned string content is what will be hashed and stored in the database.
     * @return std::string The standardized, serialized data of the object.
     */
    virtual std::string serialize() const = 0;

    /**
     * @brief Pure virtual method to get the type of the VCS object.
     * * This is crucial for object storage and reconstruction (e.g., "commit", "blob", "tree").
     * @return std::string The type identifier of the object.
     */
    virtual std::string getType() const = 0;

    /**
     * @brief Вычисляет SHA-1 хеш для заданного канонического содержимого.
     * @param content Содержимое объекта Git-формата (header + raw_data).
     * @return std::string Вычисленный 40-символьный хеш.
     */
    static std::string calculateHash(const std::string& content); 

    /**
     * @brief Returns the unique hash identifier of the object.
     * @return std::string The cryptographic hash ID.
     */
    std::string getHashId() const;
};


/**
 * @class TestableObject
 * @brief Helper class for testing the non-abstract methods of VcsObject.
 * * This class implements all pure virtual methods of VcsObject with simple stubs, 
 * allowing its instances to be created for unit testing the hashing logic 
 * in VcsObject::computeHash.
 */
class TestableObject : public VcsObject {
private:
    /// @brief The object type name returned by getType().
    std::string type_name;
    /// @brief The content returned by serialize().
    std::string content_data;
public:
    /**
     * @brief Primary constructor.
     * @param type The name of the object type (e.g., "test_blob").
     * @param data The content that will be serialized and hashed.
     */
    TestableObject(const std::string& type, const std::string& data);
    
    /**
     * @brief Implementation of the pure virtual method. Returns the stored content.
     * @copydoc VcsObject::serialize()
     */
    std::string serialize() const override;

    /**
     * @brief Implementation of the pure virtual method. Returns the stored type name.
     * @copydoc VcsObject::getType()
     */
    std::string getType() const override;
};