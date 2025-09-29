/**
 * @file EventBus.hxx
 * @brief Implementation of the central event bus (EventBus), acting as the ISubject.
 *
 * This implementation provides a thread-safe Subject capable of managing observers
 * using smart pointers to prevent circular dependencies.
 * * @copyright 2025 LogosITO Under the MIT-License
 */

#pragma once

#include "IObserver.hxx"
#include "ISubject.hxx"
#include <algorithm>
#include <vector>
#include <memory>
#include <mutex>

/**
 * @class EventBus
 * @brief A thread-safe implementation of ISubject for centralized event distribution.
 * * @details This class uses std::weak_ptr to store observers, preventing circular dependencies
 * and ensuring automatic cleanup of destroyed observers. It inherits from 
 * std::enable_shared_from_this to ensure that valid shared pointers to the bus 
 * can be created even inside its member functions.
 */
class EventBus : public ISubject, public std::enable_shared_from_this<EventBus> {
private:
    /**
     * @brief Collection of registered observers. 
     * * Stored as weak pointers to prevent the EventBus from keeping observers alive indefinitely.
     */
    std::vector<std::weak_ptr<IObserver>> observers_;
    
    /**
     * @brief Mutex to ensure thread-safe access to the observers_ vector.
     */
    mutable std::mutex observers_mutex_; // Changed to mutable to allow locking inside const notify

public:
    /**
     * @brief Default virtual destructor.
     */
    ~EventBus() override = default;

    /**
     * @brief Registers a new Observer.
     * * The observer's shared pointer is stored as a weak pointer internally.
     * @param observer The smart pointer to the Observer to be registered.
     */
    void attach(std::shared_ptr<IObserver> observer) override {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.push_back(observer);
    }

    /**
     * @brief Deregisters a specific Observer.
     * * Compares the provided shared_ptr against all stored weak_ptr objects.
     * @param observer_to_remove The smart pointer to the Observer to be removed.
     */
    void detach(std::shared_ptr<IObserver> observer_to_remove) override {
        std::lock_guard<std::mutex> lock(observers_mutex_);

        auto it = std::remove_if(observers_.begin(), observers_.end(),
            [&](const std::weak_ptr<IObserver>& weak_obs) {
                auto shared_obs = weak_obs.lock();
                return shared_obs && (shared_obs == observer_to_remove);
            }
        );
        observers_.erase(it, observers_.end());
    }

    /**
     * @brief Sends an event to all active Observers.
     * * Iterates over the list, safely locking each weak pointer before calling update(). 
     * Invalid (expired) weak pointers are effectively ignored.
     * * @note Your implementation of `notify` uses `const_cast<std::mutex&>(observers_mutex_)` 
     * because `notify` is `const`. A better practice is to declare `observers_mutex_` as **`mutable`**.
     * * @param event The constant reference to the Event to be published.
     */
    void notify(const Event& event) const override { 
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(observers_mutex_));
        for (const auto& wp : observers_) {
            if (auto observer = wp.lock()) {
                observer->update(event);
            }
        }
    }
};