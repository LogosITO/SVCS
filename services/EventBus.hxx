/**
 * @file EventBus.hxx
 * @brief Implementation of the central event bus (EventBus), acting as the ISubject.
 *
 * This implementation provides a thread-safe Subject capable of managing observers
 * using smart pointers to prevent circular dependencies.
 * * @copyright 2025 LogosITO Under the MIT-License
 */

#pragma once

#include "Event.hxx"
#include "IObserver.hxx"
#include "ISubject.hxx"
#include <algorithm>
#include <vector>
#include <memory>
#include <mutex>

namespace ConsoleColor {
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";
    
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string ITALIC = "\033[3m";
    const std::string UNDERLINE = "\033[4m";
}

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

    static std::string getEventColor(Event::Type type) {
        switch (type) {
            case Event::REPOSITORY_INIT_SUCCESS:
                return ConsoleColor::BRIGHT_GREEN;
            case Event::SAVE_SUCCESS:
                return ConsoleColor::BRIGHT_GREEN;
            case Event::ERROR_MESSAGE:
                return ConsoleColor::BRIGHT_RED;
            case Event::WARNING_MESSAGE:
                return ConsoleColor::BRIGHT_YELLOW;
            case Event::GENERAL_INFO:
                return ConsoleColor::BRIGHT_CYAN;
            case Event::DEBUG_MESSAGE:
                return ConsoleColor::BRIGHT_BLUE;
            case Event::HELP_MESSAGE:
                return ConsoleColor::GREEN;
            default:
                return ConsoleColor::WHITE;
        }
    }

    static std::string getEventIcon(Event::Type type) {
        switch (type) {
            case Event::REPOSITORY_INIT_SUCCESS:
                return "✅";
            case Event::SAVE_SUCCESS:
                return "💾";
            case Event::ERROR_MESSAGE:
                return "❌";
            case Event::WARNING_MESSAGE:
                return "⚠️ ";
            case Event::GENERAL_INFO:
                return "ℹ️ ";
            case Event::DEBUG_MESSAGE:
                return "🐛";
            case Event::HELP_MESSAGE:
                return "💡";
            default:
                return "";
        }
    };

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
     * @note Your implementation of `notify` uses `const_cast<std::mutex&>(observers_mutex_)` 
     * because `notify` is `const`. A better practice is to declare `observers_mutex_` as **`mutable`**.
     * @param event The constant reference to the Event to be published.
     */
    void notify(const Event& event) const override{
        std::string color = getEventColor(event.type);
        std::string icon = getEventIcon(event.type);
        std::string sourceColor = ConsoleColor::DIM + ConsoleColor::BLACK;
        
        // Форматируем сообщение с цветами
        std::string formattedMessage = color + icon + " " + event.details + 
                                      sourceColor + " [" + event.source_name + "]" + 
                                      ConsoleColor::RESET;
        
        // Создаем копию события с отформатированным сообщением
        Event coloredEvent = event;
        coloredEvent.details = formattedMessage;
        
        for (auto& observer : observers_) {
            observer.lock()->update(coloredEvent);
        }
    }

};