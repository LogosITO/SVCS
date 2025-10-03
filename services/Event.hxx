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
        FATAL_ERROR, 
        RUNTIME_ERROR,         

        REPOSITORY_INIT_SUCCESS,
        STAGE_SUCCESS,
        SAVE_SUCCESS,
        OBJECT_WRITE_SUCCESS,
        OBJECT_READ_SUCCESS, 
        INDEX_UPDATE, 
        COMMIT_CREATED, 
        
        GENERAL_INFO,  
        HELP_MESSAGE,
        DEBUG_MESSAGE,          
        ERROR_MESSAGE,          
        WARNING_MESSAGE
    };

    Type type;                  ///< The type of event that occurred
    std::string details;        ///< Detailed description or payload (e.g., object hash)
    
    /**
     * @brief Source of the event (e.g., "main", "InitCommand", "Repository").
     * @details Added to support three-argument aggregate initialization in notify() calls.
     */
    std::string source_name;    ///< The module or command that generated the event
};