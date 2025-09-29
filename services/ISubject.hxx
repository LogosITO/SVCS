/**
 * @file ISubject.hxx
 * @brief Defines the interface for the Subject (Publisher) component of the Observer pattern.
 *
 * This interface establishes the contract for any class that wishes to act as an 
 * event publisher, allowing Observers to register for notifications. It mandates 
 * the use of smart pointers for managing observer lifetimes.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */
#pragma once

#include "IObserver.hxx"
#include <memory>
#include <vector>

/**
 * @class ISubject
 * @brief The Subject (Publisher) Interface (Abstract Base Class).
 * * Core classes (e.g., Repository, EventBus) implement this interface to allow 
 * observers to subscribe to events. The contract requires all management functions 
 * (`attach`, `detach`) to handle `std::shared_ptr<IObserver>`.
 */
class ISubject {
protected:
    std::vector<IObserver*> observers; 
public:
    /**
     * @brief Virtual destructor.
     * * Ensures correct cleanup of derived subject classes.
     */
    virtual ~ISubject() = default;

    /**
     * @brief Attaches an observer to the subject.
     * @param observer A smart pointer to the observer object to attach.
     */
    virtual void attach(std::shared_ptr<IObserver> observer) = 0;

    /**
     * @brief Detaches an observer from the subject.
     * @param observer A smart pointer to the observer object to detach.
     */
    virtual void detach(std::shared_ptr<IObserver> observer) = 0;

    /**
     * @brief Notifies all attached observers about an event.
     * @details This method is made public so that component classes (like ObjectStorage) 
     * can publish events through the injected ISubject pointer. The method is declared 
     * as `const` because event notification is a logically non-mutating action on the Subject's state.
     * @param event The constant reference to the event structure to be sent.
     */
    virtual void notify(const Event& event) const = 0;
};