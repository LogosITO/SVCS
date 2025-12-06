/**
 * @file IObserver.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines the interface for the Observer pattern component.
 * @details This interface establishes the contract for any class that wishes to subscribe
 * to and receive notifications from a Subject (e.g., EventBus, Repository).
 *
 * @russian
 * @brief Определяет интерфейс для компонента шаблона Observer.
 * @details Этот интерфейс устанавливает контракт для любого класса, который желает подписаться
 * и получать уведомления от Subject (например, EventBus, Repository).
 */
#pragma once

#include "Event.hxx"


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
 * @class IObserver
 * @brief The Observer Interface (Abstract Base Class).
 * @details Any class that wants to receive notifications from the system core (e.g., Repository, Command)
 * must implement this interface to be registered with a Subject.
 *
 * @russian
 * @class IObserver
 * @brief Интерфейс Observer (Абстрактный Базовый Класс).
 * @details Любой класс, который хочет получать уведомления от ядра системы (например, Repository, Command),
 * должен реализовать этот интерфейс для регистрации в Subject.
 */
class IObserver {
public:
    /**
     * @english
     * @brief Default virtual destructor.
     * @details Ensures correct cleanup of derived observer classes via a base class pointer.
     *
     * @russian
     * @brief Виртуальный деструктор по умолчанию.
     * @details Обеспечивает корректную очистку производных классов наблюдателей через указатель на базовый класс.
     */
    virtual ~IObserver() = default;

    /**
     * @english
     * @brief Method called by the Subject to notify the observer.
     * @param event The Event structure containing information about the occurrence.
     * @deprecated Note: This method appears redundant with update(const Event& event).
     *
     * @russian
     * @brief Метод, вызываемый Subject для уведомления наблюдателя.
     * @param event Структура Event, содержащая информацию о произошедшем.
     * @deprecated Примечание: Этот метод кажется избыточным с update(const Event& event).
     */
    virtual void notify(const Event& event) = 0;

    /**
     * @english
     * @brief Method called by the Subject to update the observer.
     * @param event The Event structure containing information about the occurrence.
     *
     * @russian
     * @brief Метод, вызываемый Subject для обновления наблюдателя.
     * @param event Структура Event, содержащая информацию о произошедшем.
     */
    virtual void update(const Event& event) = 0;
};

}