/**
 * @file MockSubject.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Mock implementation of ISubject for testing purposes.
 *
 * @russian
 * @brief Mock реализация ISubject для целей тестирования.
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "../../services/IObserver.hxx"
#include "../../services/Event.hxx"

#include <vector>
#include <memory>
#include <string>
#include <algorithm>

/**
 * @english
 * @namespace svcs::test::core::mocks
 * @brief Mock objects and test doubles for core component testing.
 * @details Contains mock implementations of core interfaces used
 * for isolating and testing core VCS components. These mocks provide
 * controlled behavior and verification capabilities for unit tests.
 *
 * @russian
 * @namespace svcs::test::core::mocks
 * @brief Mock объекты и тестовые дубли для тестирования основных компонентов.
 * @details Содержит mock реализации основных интерфейсов, используемых
 * для изоляции и тестирования основных компонентов СКВ. Эти моки предоставляют
 * контролируемое поведение и возможности верификации для модульных тестов.
 */
namespace svcs::test::core::mocks {

class MockSubject : public svcs::services::ISubject {
public:
    struct RecordedNotification {
        svcs::services::Event event;
        std::shared_ptr<svcs::services::IObserver> observer;
    };

    MockSubject() = default;
    ~MockSubject() override = default;

    void attach(std::shared_ptr<svcs::services::IObserver> observer) override {
        observers.push_back(observer.get());
        observer_pointers.push_back(observer);
    }

    void detach(std::shared_ptr<svcs::services::IObserver> observer) override {
        auto it = std::find(observers.begin(), observers.end(), observer.get());
        if (it != observers.end()) {
            observers.erase(it);
        }

        auto ptr_it = std::find(observer_pointers.begin(), observer_pointers.end(), observer);
        if (ptr_it != observer_pointers.end()) {
            observer_pointers.erase(ptr_it);
        }
    }

    void notify(const svcs::services::Event& event) const override {
        recorded_notifications.push_back({event, nullptr});
        for (auto* observer : observers) {
            observer->update(event);
            recorded_notifications.push_back({event, nullptr});
        }
    }

    void clearNotifications() {
        recorded_notifications.clear();
    }

    size_t getNotificationCount() const {
        return recorded_notifications.size();
    }

    const std::vector<RecordedNotification>& getNotifications() const {
        return recorded_notifications;
    }

    bool hasNotification(svcs::services::Event::Type type) const {
        for (const auto& notification : recorded_notifications) {
            if (notification.event.type == type) {
                return true;
            }
        }
        return false;
    }

private:
    std::vector<svcs::services::IObserver*> observers;
    std::vector<std::shared_ptr<svcs::services::IObserver>> observer_pointers;
    mutable std::vector<RecordedNotification> recorded_notifications;
};

}