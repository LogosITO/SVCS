/**
 * @file ISubject.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines the interface for the Subject (Publisher) component of the Observer pattern.
 * @details This interface establishes the contract for any class that wishes to act as an
 * event publisher, allowing Observers to register for notifications. It mandates
 * the use of smart pointers for managing observer lifetimes.
 *
 * @russian
 * @brief Определяет интерфейс для компонента Subject (Издатель) шаблона Observer.
 * @details Этот интерфейс устанавливает контракт для любого класса, который желает действовать как
 * издатель событий, позволяя Observers регистрироваться для уведомлений. Он требует
 * использования умных указателей для управления временем жизни наблюдателей.
 */
#pragma once

#include "IObserver.hxx"
#include <memory>
#include <vector>


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
 * @class ISubject
 * @brief The Subject (Publisher) Interface (Abstract Base Class).
 * @details Core classes (e.g., Repository, EventBus) implement this interface to allow
 * observers to subscribe to events. The contract requires all management functions
 * (`attach`, `detach`) to handle `std::shared_ptr<IObserver>`.
 *
 * @russian
 * @class ISubject
 * @brief Интерфейс Subject (Издатель) (Абстрактный Базовый Класс).
 * @details Основные классы (например, Repository, EventBus) реализуют этот интерфейс, чтобы позволить
 * наблюдателям подписываться на события. Контракт требует, чтобы все функции управления
 * (`attach`, `detach`) обрабатывали `std::shared_ptr<IObserver>`.
 */
class ISubject {
protected:
    /**
     * @english
     * @brief Collection of registered observers.
     *
     * @russian
     * @brief Коллекция зарегистрированных наблюдателей.
     */
    std::vector<IObserver*> observers;

public:
    /**
     * @english
     * @brief Virtual destructor.
     * @details Ensures correct cleanup of derived subject classes.
     *
     * @russian
     * @brief Виртуальный деструктор.
     * @details Обеспечивает корректную очистку производных классов субъектов.
     */
    virtual ~ISubject() = default;

    /**
     * @english
     * @brief Attaches an observer to the subject.
     * @param observer A smart pointer to the observer object to attach.
     *
     * @russian
     * @brief Прикрепляет наблюдателя к субъекту.
     * @param observer Умный указатель на объект наблюдателя для прикрепления.
     */
    virtual void attach(std::shared_ptr<IObserver> observer) = 0;

    /**
     * @english
     * @brief Detaches an observer from the subject.
     * @param observer A smart pointer to the observer object to detach.
     *
     * @russian
     * @brief Открепляет наблюдателя от субъекта.
     * @param observer Умный указатель на объект наблюдателя для открепления.
     */
    virtual void detach(std::shared_ptr<IObserver> observer) = 0;

    /**
     * @english
     * @brief Notifies all attached observers about an event.
     * @details This method is made public so that component classes (like ObjectStorage)
     * can publish events through the injected ISubject pointer. The method is declared
     * as `const` because event notification is a logically non-mutating action on the Subject's state.
     * @param event The constant reference to the event structure to be sent.
     *
     * @russian
     * @brief Уведомляет всех прикрепленных наблюдателей о событии.
     * @details Этот метод сделан публичным, чтобы классы компонентов (как ObjectStorage)
     * могли публиковать события через внедренный указатель ISubject. Метод объявлен
     * как `const`, потому что уведомление о событии является логически неизменяющим действием над состоянием Subject.
     * @param event Константная ссылка на структуру события для отправки.
     */
    virtual void notify(const Event& event) const = 0;
};

}