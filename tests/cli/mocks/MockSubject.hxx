/**
 * @file MockSubject.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Mock implementation of the ISubject interface for unit testing purposes.
 * @details This class captures all events published by the system core without requiring
 * the full Observer system implementation. It provides utility methods for easy assertion
 * checks in unit tests.
 *
 * @russian
 * @brief Mock реализация интерфейса ISubject для целей модульного тестирования.
 * @details Этот класс захватывает все события, публикуемые ядром системы, без необходимости
 * полной реализации системы Observer. Он предоставляет вспомогательные методы для удобства
 * проверки утверждений в модульных тестах.
 */
#pragma once

#include "../../../services/ISubject.hxx"
#include <gmock/gmock.h>
#include <vector>
#include <string>

/**
 * @english
 * @namespace svcs::test::cli::mocks
 * @brief Mock objects and test doubles for CLI command testing.
 * @details Contains mock implementations of CLI-related interfaces used
 * for testing command behavior, help systems, and user interactions.
 * These mocks provide controlled command responses and verification
 * capabilities for CLI command unit tests.
 *
 * @russian
 * @namespace svcs::test::cli::mocks
 * @brief Mock объекты и тестовые дубли для тестирования CLI команд.
 * @details Содержит mock реализации CLI-интерфейсов, используемых
 * для тестирования поведения команд, систем справки и взаимодействий с пользователем.
 * Эти моки предоставляют контролируемые ответы команд и возможности
 * верификации для модульных тестов CLI команд.
 */
namespace svcs::test::cli::mocks {

using namespace svcs::services;

/**
 * @english
 * @class MockSubject
 * @brief A mock implementation of ISubject for testing event-driven components.
 * @details This mock allows test cases to:
 * 1. **Capture** all events published via notify() method.
 * 2. **Inspect** captured events for specific types or messages.
 * 3. **Clear** the event history between test cases.
 * 4. **Verify** that specific events were published during execution.
 *
 * @russian
 * @class MockSubject
 * @brief Mock реализация ISubject для тестирования событийно-ориентированных компонентов.
 * @details Этот mock позволяет тестовым случаям:
 * 1. **Захватывать** все события, опубликованные через метод notify().
 * 2. **Проверять** захваченные события на наличие определенных типов или сообщений.
 * 3. **Очищать** историю событий между тестовыми случаями.
 * 4. **Проверять**, что определенные события были опубликованы во время выполнения.
 */
class MockSubject : public ISubject {
public:
    /**
     * @english
     * @brief Stores a history of all events received via the notify() method for assertion.
     *
     * @russian
     * @brief Хранит историю всех событий, полученных через метод notify(), для проверки утверждений.
     */
    std::vector<Event> notifications;

    /**
     * @english
     * @brief Satisfies the ISubject interface, but performs no operation.
     * @param observer The observer to attach (unused in mock).
     *
     * @russian
     * @brief Выполняет требования интерфейса ISubject, но не выполняет никаких операций.
     * @param observer Наблюдатель для присоединения (не используется в mock).
     */
    void attach(std::shared_ptr<IObserver> observer) override {}

    /**
     * @english
     * @brief Satisfies the ISubject interface, but performs no operation.
     * @param observer The observer to detach (unused in mock).
     *
     * @russian
     * @brief Выполняет требования интерфейса ISubject, но не выполняет никаких операций.
     * @param observer Наблюдатель для отсоединения (не используется в mock).
     */
    void detach(std::shared_ptr<IObserver> observer) override {}

    /**
     * @english
     * @brief Captures the event into the internal notifications vector.
     * @details This is the core testing function, allowing test cases to verify
     * that the correct events were published.
     * @param event The constant reference to the event structure to be captured.
     *
     * @russian
     * @brief Захватывает событие во внутренний вектор уведомлений.
     * @details Это основная тестовая функция, позволяющая тестовым случаям проверять,
     * что были опубликованы правильные события.
     * @param event Константная ссылка на структуру события для захвата.
     */
    void notify(const Event& event) const override {
        const_cast<MockSubject*>(this)->notifications.push_back(event);
    }

    /**
     * @english
     * @brief Clears all captured notifications.
     *
     * @russian
     * @brief Очищает все захваченные уведомления.
     */
    void clear() {
        notifications.clear();
    }

    /**
     * @english
     * @brief Checks if any captured notification's details contain the specified message substring.
     * @param message The substring to search for within notification details.
     * @return true if a matching message is found, false otherwise.
     *
     * @russian
     * @brief Проверяет, содержат ли детали любого захваченного уведомления указанную подстроку сообщения.
     * @param message Подстрока для поиска в деталях уведомлений.
     * @return true если найдено соответствующее сообщение, false в противном случае.
     */
    [[nodiscard]] bool containsMessage(const std::string& message) const {
        for (const auto& notification : notifications) {
            if (notification.details.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    /**
     * @english
     * @brief Checks if any captured notification matches the specified Event Type.
     * @param type The Event::Type enumeration value to search for.
     * @return true if an event of the specified type is found, false otherwise.
     *
     * @russian
     * @brief Проверяет, соответствует ли какое-либо захваченное уведомление указанному типу события.
     * @param type Значение перечисления Event::Type для поиска.
     * @return true если найден событие указанного типа, false в противном случае.
     */
    [[nodiscard]] bool containsEventType(Event::Type type) const {
        for (const auto& notification : notifications) {
            if (notification.type == type) {
                return true;
            }
        }
        return false;
    }

    /**
     * @english
     * @brief Stores a notification in the internal vector (alias for notify functionality).
     * @param event The event to store.
     *
     * @russian
     * @brief Сохраняет уведомление во внутреннем векторе (псевдоним для функциональности notify).
     * @param event Событие для сохранения.
     */
    void storeNotification(const Event& event) {
        notifications.push_back(event);
    }

    /**
     * @english
     * @brief Clears all notifications from the internal storage.
     *
     * @russian
     * @brief Очищает все уведомления из внутреннего хранилища.
     */
    void clearNotifications() {
        notifications.clear();
    }

    /**
     * @english
     * @brief Returns a constant reference to the notifications vector for inspection.
     * @return Constant reference to the vector of captured events.
     *
     * @russian
     * @brief Возвращает константную ссылку на вектор уведомлений для проверки.
     * @return Константная ссылка на вектор захваченных событий.
     */
    [[nodiscard]] const std::vector<Event>& getNotifications() const {
        return notifications;
    }
};

}