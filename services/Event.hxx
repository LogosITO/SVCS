/**
 * @file Event.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Defines the event structure used for the Observer pattern notifications.
 */
#pragma once

#include <string>

/**
 * @brief Structure describing an event published by the VCS core.
 * * This is the "message" that the Subject (Publisher, e.g., Repository) 
 * sends to all its Observers.
 */
struct Event {
    /**
     * @brief Types of events that can be published.
     */
    enum Type {
        // Error Levels
        FATAL_ERROR,            ///< Critical error leading to program termination
        RUNTIME_ERROR,          ///< Error preventing an operation (e.g., File I/O error)
        
        // Information and Debugging Levels
        REPOSITORY_INIT_SUCCESS,///< Repository successfully initialized
        OBJECT_WRITE_SUCCESS,   ///< Object successfully written to storage
        OBJECT_READ_SUCCESS,    ///< Object successfully read
        INDEX_UPDATE,           ///< Working area index updated
        COMMIT_CREATED,         ///< New commit successfully created
        
        // General Information
        GENERAL_INFO,           ///< General informational message
        DEBUG_MESSAGE           ///< Message for debugging
    };

    Type type;                  ///< The type of event that occurred
    std::string details;        ///< Detailed description or payload (e.g., object hash)
};
