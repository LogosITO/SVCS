/**
 * @file EventBus.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the central event bus (EventBus), acting as the ISubject.
 * @details This implementation provides a thread-safe Subject capable of managing observers
 * using smart pointers to prevent circular dependencies.
 *
 * @russian
 * @brief Реализация центральной шины событий (EventBus), действующей как ISubject.
 * @details Эта реализация предоставляет потокобезопасный Subject, способный управлять наблюдателями
 * с использованием умных указателей для предотвращения циклических зависимостей.
 */
#pragma once

#include "Event.hxx"
#include "IObserver.hxx"
#include "ISubject.hxx"
#include <algorithm>
#include <vector>
#include <memory>
#include <mutex>

/**
 * @english
 * @namespace svcs::services
 * @brief Service layer components and infrastructure services.
 * @details Contains core infrastructure services including event system,
 * notification mechanism, observer pattern implementation, and other
 * cross-cutting concerns that support the VCS operations.
 *
 * @russian
 * @namespace svcs::services
 * @brief Компоненты сервисного слоя и инфраструктурные сервисы.
 * @details Содержит основные инфраструктурные сервисы, включая систему событий,
 * механизм уведомлений, реализацию шаблона Observer и другие сквозные задачи,
 * которые поддерживают операции СКВ.
 */
namespace svcs::services {

/**
 * @english
 * @namespace ConsoleColor
 * @brief Defines ANSI color codes for console output formatting.
 *
 * @russian
 * @namespace ConsoleColor
 * @brief Определяет ANSI коды цветов для форматирования вывода в консоль.
 */
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
 * @english
 * @class EventBus
 * @brief A thread-safe implementation of ISubject for centralized event distribution.
 * @details This class uses std::weak_ptr to store observers, preventing circular dependencies
 * and ensuring automatic cleanup of destroyed observers. It inherits from
 * std::enable_shared_from_this to ensure that valid shared pointers to the bus
 * can be created even inside its member functions.
 *
 * @russian
 * @class EventBus
 * @brief Потокобезопасная реализация ISubject для централизованного распределения событий.
 * @details Этот класс использует std::weak_ptr для хранения наблюдателей, предотвращая циклические зависимости
 * и обеспечивая автоматическую очистку уничтоженных наблюдателей. Он наследуется от
 * std::enable_shared_from_this, чтобы гарантировать, что валидные общие указатели на шину
 * могут быть созданы даже внутри его функций-членов.
 */
class EventBus : public ISubject, public std::enable_shared_from_this<EventBus> {
private:
    /**
     * @english
     * @brief Collection of registered observers.
     * @details Stored as weak pointers to prevent the EventBus from keeping observers alive indefinitely.
     *
     * @russian
     * @brief Коллекция зарегистрированных наблюдателей.
     * @details Хранится как слабые указатели, чтобы предотвратить бесконечное поддержание наблюдателей шиной событий.
     */
    std::vector<std::weak_ptr<IObserver>> observers_;

    /**
     * @english
     * @brief Mutex to ensure thread-safe access to the observers_ vector.
     *
     * @russian
     * @brief Мьютекс для обеспечения потокобезопасного доступа к вектору observers_.
     */
    mutable std::mutex observers_mutex_;

    /**
     * @english
     * @brief Gets the console color for a specific event type.
     * @param type The event type.
     * @return ANSI color code string.
     *
     * @russian
     * @brief Получает цвет консоли для конкретного типа события.
     * @param type Тип события.
     * @return Строка с ANSI кодом цвета.
     */
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
            case Event::PROTOCOL_START:
                return ConsoleColor::BRIGHT_MAGENTA;
            case Event::PROTOCOL_SUCCESS:
                return ConsoleColor::BRIGHT_GREEN;
            case Event::PROTOCOL_ERROR:
                return ConsoleColor::BRIGHT_RED;
            case Event::NETWORK_SEND:
                return ConsoleColor::BRIGHT_CYAN;
            case Event::NETWORK_RECEIVE:
                return ConsoleColor::BRIGHT_BLUE;
            case Event::OBJECT_TRANSFER:
                return ConsoleColor::BRIGHT_YELLOW;
            case Event::REFERENCE_UPDATE:
                return ConsoleColor::BRIGHT_GREEN;
            case Event::NEGOTIATION_PHASE:
                return ConsoleColor::BRIGHT_MAGENTA;
            default:
                return ConsoleColor::WHITE;
        }
    }

    /**
     * @english
     * @brief Gets the icon for a specific event type.
     * @param type The event type.
     * @return Icon string (emoji).
     *
     * @russian
     * @brief Получает иконку для конкретного типа события.
     * @param type Тип события.
     * @return Строка с иконкой (эмодзи).
     */
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
            case Event::PROTOCOL_START:
                return "🚀";
            case Event::PROTOCOL_SUCCESS:
                return "✅";
            case Event::PROTOCOL_ERROR:
                return "💥";
            case Event::NETWORK_SEND:
                return "📤";
            case Event::NETWORK_RECEIVE:
                return "📥";
            case Event::OBJECT_TRANSFER:
                return "📦";
            case Event::REFERENCE_UPDATE:
                return "🔗";
            case Event::NEGOTIATION_PHASE:
                return "🤝";
            default:
                return "";
        }
    }

public:
    /**
     * @english
     * @brief Default virtual destructor.
     *
     * @russian
     * @brief Виртуальный деструктор по умолчанию.
     */
    ~EventBus() override = default;

    /**
     * @english
     * @brief Registers a new Observer.
     * @details The observer's shared pointer is stored as a weak pointer internally.
     * @param observer The smart pointer to the Observer to be registered.
     *
     * @russian
     * @brief Регистрирует нового Observer.
     * @details Общий указатель наблюдателя хранится как слабый указатель внутри.
     * @param observer Умный указатель на Observer для регистрации.
     */
    void attach(std::shared_ptr<IObserver> observer) override {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.push_back(observer);
    }

    /**
     * @english
     * @brief Deregisters a specific Observer.
     * @details Compares the provided shared_ptr against all stored weak_ptr objects.
     * @param observer_to_remove The smart pointer to the Observer to be removed.
     *
     * @russian
     * @brief Отменяет регистрацию конкретного Observer.
     * @details Сравнивает предоставленный shared_ptr со всеми хранимыми объектами weak_ptr.
     * @param observer_to_remove Умный указатель на Observer для удаления.
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
     * @english
     * @brief Sends an event to all active Observers.
     * @details Iterates over the list, safely locking each weak pointer before calling update().
     * Invalid (expired) weak pointers are effectively ignored.
     * @param event The constant reference to the Event to be published.
     *
     * @russian
     * @brief Отправляет событие всем активным Observers.
     * @details Перебирает список, безопасно блокируя каждый слабый указатель перед вызовом update().
     * Невалидные (истекшие) слабые указатели эффективно игнорируются.
     * @param event Константная ссылка на событие для публикации.
     */
    void notify(const Event& event) const override {
        std::string color = getEventColor(event.type);
        std::string icon = getEventIcon(event.type);
        std::string sourceColor = ConsoleColor::DIM + ConsoleColor::BLACK;

        // Format message with colors
        std::string formattedMessage = color + icon + " " + event.details +
                                      sourceColor + " [" + event.source_name + "]" +
                                      ConsoleColor::RESET;

        // Create event copy with formatted message
        Event coloredEvent = event;
        coloredEvent.details = formattedMessage;

        for (auto& observer : observers_) {
            observer.lock()->update(coloredEvent);
        }
    }
};

}