/**
 * @file Repository.hxx
 * @brief Manages the entire version control repository, acts as a Subject for event logging, 
 * and controls all major VCS operations.
 * * @details This class is responsible for initializing the repository structure, 
 * managing the ObjectStorage, and coordinating all file system and object database interactions.
 * It implements the ISubject interface to broadcast events about its operations.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "ObjectStorage.hxx"
#include "Index.hxx"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

/**
 * @class Repository
 * @brief Manages the version control directory structure and core operations.
 * * Implements ISubject to serve as an event publisher for the core VCS operations.
 */
class Repository : public ISubject {
private:
    /**
     * @brief The root directory path of the repository (where the .svcs directory lives).
     */
    const std::filesystem::path root_path;
    
    /**
     * @brief Unique pointer to the ObjectStorage instance, responsible for managing the object database.
     */
    std::unique_ptr<ObjectStorage> objects;
    
    /**
     * @brief Collection of weak pointers to registered observers, inherited from ISubject's concept.
     * @note This member is part of the concrete Subject implementation.
     */
    std::vector<std::weak_ptr<IObserver>> observers;

    std::unique_ptr<Index> index; 
public:
    /**
     * @brief Virtual destructor.
     * * Ensures correct cleanup of managed resources (ObjectStorage).
     */
    ~Repository() override;

    /**
     * @brief Constructs the repository manager.
     * @param rpath The root directory of the repository.
     */
    explicit Repository(std::filesystem::path  rpath);

    /**
     * @brief Initializes the repository's internal dependencies.
     * * This typically involves instantiating and setting up internal components like ObjectStorage.
     */
    void initializeDependencies();

    /**
     * @brief Initializes the basic repository structure (.svcs, objects, refs) on the disk.
     * @return bool True if initialization succeeded, false otherwise.
     */
    bool init();

    /**
     * @brief Checks if the repository structure is fully initialized and valid.
     * @return true if all necessary directories and files exist.
     */
    [[nodiscard]] bool is_initialized() const;

    /**
     * @brief Gets the root path of the repository.
     * @return const std::filesystem::path& The repository root path.
     */
    [[nodiscard]] const std::filesystem::path& getPath() const { return root_path; }

    /**
     * @brief Gets the raw pointer to the managed ObjectStorage instance for database interaction.
     * @return ObjectStorage* Pointer to the managed ObjectStorage instance.
     */
    [[nodiscard]] ObjectStorage* getObjectStorage() const { return objects.get(); }

    /**
     * @brief Attaches an observer to receive events.
     * @param observer Shared pointer to the observer instance.
     */
    void attach(std::shared_ptr<IObserver> observer) override;

    /**
     * @brief Detaches a registered observer.
     * @param observer Shared pointer to the observer instance to detach.
     */
    void detach(std::shared_ptr<IObserver> observer) override;

    /**
     * @brief Notifies all attached observers about an event.
     * @param event The constant reference to the event data structure.
     */
    void notify(const Event& event) const override;

    /**
     * @brief Statically searches for an existing repository starting from a given path and moving up.
     * @param start_path The directory path to begin the search from (defaults to ".").
     * @return A shared pointer to the found Repository instance, or nullptr if none is found.
     */
    static std::shared_ptr<Repository> findRepository(const std::string& start_path = ".");
    
    /**
     * @brief Statically checks if a given path is the root of an SVCS repository.
     * @param path The path to check.
     * @return true if the path contains the required repository marker files/directories.
     */
    static bool isRepository(const std::string& path);

    /**
     * @brief Stages a file by hashing its content, storing it in the object database, 
     * and updating the staging index.
     * @param path The path to the file to be staged (relative to the repository root).
     * @return bool True if the file was successfully staged, false otherwise (e.g., file ignored, permission error).
     * @throws std::runtime_error if file system or object storage operations fail.
     */
    bool stageFile(const std::string& path); 
};