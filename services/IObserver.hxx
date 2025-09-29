/**
 * @file IObserver.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Defines the interface for the Observer pattern component.
 */
#pragma once

#include "Event.hxx"

/**
 * @brief Observer Interface.
 * * Any class that wants to receive notifications from the core (Repository, ObjectStorage)
 * must implement this interface.
 */
class IObserver {
public:
    /**
     * @brief Default virtual destructor.
     */
    virtual ~IObserver() = default;

    /**
     * @brief Method called by the subject to notify the observer.
     * @param event Structure Event with information about what happened.
     */
    virtual void notify(const Event& event) = 0;

     /**
     * @brief Method to update the observer status.
     * @param event Structure Event with information about what happened.
     */
    virtual void update(const Event& event) = 0;
};
