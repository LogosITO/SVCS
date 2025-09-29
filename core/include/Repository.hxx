/**
 * @file Repository.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Manages the entire version control repository, acts as a Subject for event logging, 
 * and controls all major VCS operations.
 * @details This class is responsible for initializing the repository structure, 
 * managing the ObjectStorage, and coordinating all file system and object database interactions.
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "ObjectStorage.hxx"
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

class Repository : public ISubject {
private:
    const std::filesystem::path root_path;
    std::unique_ptr<ObjectStorage> objects;
    std::vector<IObserver*> observers;
public:
    /**
     * @brief Constructs the repository manager.
     * @param root_path The root directory of the repository.
     */
    Repository(const std::string& rpath);   

    /**
     * @brief Initializing dependencies.
     */
    void initializeDependencies();

    /**
     * @brief Initializes the basic repository structure (.svcs, objects, refs).
     * @return bool True if initialization succeeded, false otherwise.
     */
    bool init();

    /**
     * @brief Gets the root path of the repository.
     * @return const std::filesystem::path& The repository root path.
     */
    const std::filesystem::path& getPath() const { return root_path; }

    /**
     * @brief Gets the ObjectStorage instance for database interaction.
     * @return ObjectStorage* Pointer to the managed ObjectStorage instance.
     */
    ObjectStorage* getObjectStorage() const { return objects.get(); }

    /**
     * @brief Attaches an observer to receive events.
     * @param observer Pointer to the observer instance.
     */
    void attach(IObserver* observer) override;

    /**
     * @brief Detaches an observer.
     * @param observer Pointer to the observer instance to detach.
     */
    void detach(IObserver* observer) override;

    /**
     * @brief Notifies all attached observers about an event.
     * @param event The event data structure.
     */
    void notify(const Event& event) override;
};