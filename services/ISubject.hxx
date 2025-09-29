/**
 * @file ISubject.hxx
 * @brief Defines the interface for the Subject (Publisher) pattern component.
 * @copyright 2024 (c) Simple VCS Project. All rights reserved.
 */
#pragma once

#include "IObserver.hxx"
#include <vector>

/**
 * @brief Subject (Publisher) Interface.
 * * Core classes (e.g., Repository) will implement this interface 
 * to allow observers to subscribe to events.
 */
class ISubject {
public:
    virtual ~ISubject() = default;

    /**
     * @brief Attaches an observer to the subject.
     * @param observer Pointer to the observer object.
     */
    virtual void attach(IObserver* observer) = 0;

    /**
     * @brief Detaches an observer from the subject.
     * @param observer Pointer to the observer object.
     */
    virtual void detach(IObserver* observer) = 0;

    /**
     * @brief Notifies all attached observers about an event.
     * * Made PUBLIC so that component classes (like ObjectStorage) can publish events 
     * through the injected ISubject pointer.
     * @param event The event structure to be sent.
     */
    virtual void notify(const Event& event) = 0;

protected:
    std::vector<IObserver*> observers_;
};
