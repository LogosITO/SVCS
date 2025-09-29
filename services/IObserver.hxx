/**
 * @file IObserver.hxx
 * @brief Defines the interface for the Observer pattern component.
 *
 * This interface establishes the contract for any class that wishes to subscribe
 * to and receive notifications from a Subject (e.g., EventBus, Repository).
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "Event.hxx"

/**
 * @class IObserver
 * @brief The Observer Interface (Abstract Base Class).
 * * Any class that wants to receive notifications from the system core (e.g., Repository, Command)
 * must implement this interface to be registered with a Subject.
 */
class IObserver {
public:
    /**
     * @brief Default virtual destructor.
     * * Ensures correct cleanup of derived observer classes via a base class pointer.
     */
    virtual ~IObserver() = default;

    /**
     * @brief Method called by the Subject to notify the observer.
     * @param event The Event structure containing information about the occurrence.
     * @deprecated Note: This method appears redundant with update(const Event& event).
     */
    virtual void notify(const Event& event) = 0;

     /**
     * @brief Method called by the Subject to update the observer.
     * @param event The Event structure containing information about the occurrence.
     */
    virtual void update(const Event& event) = 0;
};