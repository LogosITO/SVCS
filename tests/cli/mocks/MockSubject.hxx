/**
 * @file MockSubject.hxx
 * @brief Declaration of the MockSubject class for testing components that depend on ISubject (Event Bus).
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once 

#include "../../../services/ISubject.hxx"

/**
 * @brief Mock implementation of ISubject.
 * * This class captures all events published by the system core 
 * without requiring the full Observer system implementation. It provides utility 
 * methods for easy assertion checks in unit tests.
 */
class MockSubject : public ISubject {
public:
    /** @brief Stores a history of all events received via the notify() method for assertion. */
    std::vector<Event> notifications;

    /** @brief Satisfies the ISubject interface, but performs no operation. */
    void attach(std::shared_ptr<IObserver> observer) override {}
    
    /** @brief Satisfies the ISubject interface, but performs no operation. */
    void detach(std::shared_ptr<IObserver> observer) override {}

    /** * @brief Captures the event into the internal notifications vector.
     * @details This is the core testing function, allowing test cases to verify 
     * that the correct events were published.
     * @param event The constant reference to the event structure to be captured.
     */
    void notify(const Event& event) const override {
        const_cast<MockSubject*>(this)->notifications.push_back(event);
    }
    
    /** @brief Clears all captured notifications. */
    void clear() {
        notifications.clear();
    }
    
    /** * @brief Checks if any captured notification's details contain the specified message substring.
     * @param message The substring to search for within notification details.
     * @return true if a matching message is found, false otherwise.
     */
    bool containsMessage(const std::string& message) const {
        for (const auto& notification : notifications) {
            if (notification.details.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    /** * @brief Checks if any captured notification matches the specified Event Type.
     * @param type The Event::Type enumeration value to search for.
     * @return true if an event of the specified type is found, false otherwise.
     */
    bool containsEventType(Event::Event::Type type) const {
        for (const auto& notification : notifications) {
            if (notification.type == type) {
                return true;
            }
        }
        return false;
    }
};