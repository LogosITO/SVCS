/**
 * @file Event.hxx
 * @brief Defines the event structure used for the Observer pattern notifications.
 *
 * This header defines the `Event` structure and its associated `Type` enum,
 * serving as the message payload for the Subject-Observer communication channel (Event Bus).
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include <string>

/**
 * @struct Event
 * @brief Structure describing an event published by the VCS core.
 * * This is the "message" that the Subject (Publisher, e.g., Repository or Command) 
 * sends to all its Observers (Subscribers, e.g., Logger).
 */
struct Event {
    /**
     * @brief Enumeration of event types that can be published.
     * * Categories include Error Levels, Success/Core Process Notifications, and General Information.
     */
    enum Type {
        // --- Error Levels ---
        FATAL_ERROR,            ///< Critical error leading to program termination.
        RUNTIME_ERROR,          ///< Error preventing an operation from completing (e.g., File I/O error).
        
        // --- Core Process Notifications ---
        REPOSITORY_INIT_SUCCESS,///< Repository successfully initialized.
        OBJECT_WRITE_SUCCESS,   ///< Object successfully written to storage.
        OBJECT_READ_SUCCESS,    ///< Object successfully read from storage.
        INDEX_UPDATE,           ///< The working area index has been updated.
        COMMIT_CREATED,         ///< A new commit object was successfully created.
        
        // --- General Information and Debugging ---
        GENERAL_INFO,           ///< General informational message relevant to the user.
        DEBUG_MESSAGE,          ///< Message intended for low-level debugging and tracing.
        ERROR_MESSAGE           ///< General error message (less severe than RUNTIME_ERROR, often used for debugging context).
    };

    Type type;                  ///< The specific type of event that occurred, defined by the Type enum.
    std::string details;        ///< Detailed description or payload (e.g., file path, object hash, error text).
    
    /**
     * @brief Source of the event (e.g., "main", "InitCommand", "Repository").
     * @details Added to support three-argument aggregate initialization in notify() calls
     * and to help observers filter or format messages based on the sender module.
     */
    std::string source_name;    ///< The module or command that generated the event.
};