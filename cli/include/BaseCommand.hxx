/**
 * @file BaseCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the BaseCommand class for all commands in the system.
 * @details BaseCommand implements the logic for interacting with the Event Bus system,
 * allowing all derived commands to easily generate informational, debug,
 * and other messages that will be automatically processed by registered
 * observers (e.g., Logger).
 *
 * @russian
 * @brief Объявление класса BaseCommand для всех команд в системе.
 * @details BaseCommand реализует логику взаимодействия с системой шины событий,
 * позволяя всем производным командам легко генерировать информационные, отладочные
 * и другие сообщения, которые будут автоматически обработаны зарегистрированными
 * наблюдателями (например, Logger).
 */
#pragma once

#include "ICommand.hxx"
#include "../../core/include/Repository.hxx"
// Note: Assumes ISubject, IObserver, and Event are available via other includes.

#include <memory>
#include <string>
#include <iostream>

// Forward declaration of ISubject is necessary if not already included
class ISubject;

/**
 * @english
 * @class BaseCommand
 * @brief Base class for all CLI commands.
 * @details Provides a common infrastructure for interacting with the Event Bus system (ISubject)
 * and utility methods for working with the repository. It is responsible for logging
 * via the Event Bus.
 *
 * @russian
 * @class BaseCommand
 * @brief Базовый класс для всех CLI команд.
 * @details Предоставляет общую инфраструктуру для взаимодействия с системой шины событий (ISubject)
 * и служебные методы для работы с репозиторием. Отвечает за логирование
 * через шину событий.
 */
class BaseCommand : public ICommand {
protected:
    /**
     * @english
     * @brief The Event Subject (Event Bus) instance.
     * @details Used to generate events (logs) that will be received by observers, such as the Logger.
     *
     * @russian
     * @brief Экземпляр субъекта событий (шины событий).
     * @details Используется для генерации событий (логов), которые будут получены наблюдателями, такими как Logger.
     */
    std::shared_ptr<ISubject> eventSubject_;

    /**
     * @english
     * @brief Finds an existing repository in the current or parent directories.
     * @return A smart pointer to the found Repository or nullptr if no repository is found.
     *
     * @russian
     * @brief Находит существующий репозиторий в текущей или родительских директориях.
     * @return Умный указатель на найденный Repository или nullptr если репозиторий не найден.
     */
    static std::shared_ptr<Repository> findRepository() {
        return Repository::findRepository();
    }

    /**
     * @english
     * @brief Creates a new repository instance.
     * @param path The path where the repository should be created (defaults to '.').
     * @return A smart pointer to the new Repository.
     *
     * @russian
     * @brief Создает новый экземпляр репозитория.
     * @param path Путь, где должен быть создан репозиторий (по умолчанию '.').
     * @return Умный указатель на новый Repository.
     */
    static std::shared_ptr<Repository> createRepository(const std::string& path = ".") {
        return std::make_shared<Repository>(path);
    }

public:
    /**
     * @english
     * @brief Constructor.
     * @param subject The ISubject (Event Bus) instance that will be used for generating events and logs.
     *
     * @russian
     * @brief Конструктор.
     * @param subject Экземпляр ISubject (шины событий), который будет использоваться для генерации событий и логов.
     */
    BaseCommand(std::shared_ptr<ISubject> subject) : eventSubject_(std::move(subject)) {}

    /**
     * @english
     * @brief Virtual destructor.
     * @details Ensures correct cleanup of derived classes.
     *
     * @russian
     * @brief Виртуальный деструктор.
     * @details Обеспечивает корректную очистку производных классов.
     */
    ~BaseCommand() override = default;

    /**
     * @english
     * @brief Gets the command name (e.g., "init", "commit").
     * @return The command name as a string.
     *
     * @russian
     * @brief Получает имя команды (например, "init", "commit").
     * @return Имя команды в виде строки.
     */
    virtual std::string getName() const override = 0;

    /**
     * @english
     * @brief Gets a brief description of the command.
     * @return The command's description.
     *
     * @russian
     * @brief Получает краткое описание команды.
     * @return Описание команды.
     */
    virtual std::string getDescription() const override = 0;

    /**
     * @english
     * @brief Gets the command's usage syntax.
     * @return A string with the syntax (e.g., "vcs init <path>").
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка с синтаксисом (например, "vcs init <path>").
     */
    virtual std::string getUsage() const override = 0;

    /**
     * @english
     * @brief Displays the command's help (Usage and Description).
     * @details This method outputs information directly to the standard output stream (stdout).
     *
     * @russian
     * @brief Отображает справку по команде (использование и описание).
     * @details Этот метод выводит информацию непосредственно в стандартный поток вывода (stdout).
     */
    void showHelp() const override {
        std::cout << "Usage: " << getUsage() << std::endl;
        std::cout << "Description: " << getDescription() << std::endl;
    }

    /**
     * @english
     * @brief Generates a debug information event (DEBUG_MESSAGE).
     * @param message The text of the debug message.
     *
     * @russian
     * @brief Генерирует событие отладочной информации (DEBUG_MESSAGE).
     * @param message Текст отладочного сообщения.
     */
    void logDebug(const std::string& message) const {
        if (eventSubject_) {
            // Note: Assuming Event takes (Type, Message, Source/Command Name)
            eventSubject_->notify({Event::DEBUG_MESSAGE, message, getName()});
        }
    }

    /**
     * @english
     * @brief Generates a general informational event (GENERAL_INFO).
     * @param message The text of the informational message.
     *
     * @russian
     * @brief Генерирует общее информационное событие (GENERAL_INFO).
     * @param message Текст информационного сообщения.
     */
    void logInfo(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::GENERAL_INFO, message, getName()});
        }
    }

    /**
     * @english
     * @brief Generates a runtime error event (RUNTIME_ERROR).
     * @param message The text of the error message.
     *
     * @russian
     * @brief Генерирует событие ошибки выполнения (RUNTIME_ERROR).
     * @param message Текст сообщения об ошибке.
     */
    void logError(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::RUNTIME_ERROR, message, getName()});
        }
    }

    /**
     * @english
     * @brief Generates a successful execution event (GENERAL_INFO with SUCCESS prefix).
     * @param message The text of the successful execution message.
     *
     * @russian
     * @brief Генерирует событие успешного выполнения (GENERAL_INFO с префиксом SUCCESS).
     * @param message Текст сообщения об успешном выполнении.
     */
    void logSuccess(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::GENERAL_INFO, "SUCCESS: " + message, getName()});
        }
    }

    /**
     * @english
     * @brief Generates a warning event (RUNTIME_ERROR with WARNING prefix).
     * @param message The text of the warning message.
     *
     * @russian
     * @brief Генерирует событие предупреждения (RUNTIME_ERROR с префиксом WARNING).
     * @param message Текст сообщения-предупреждения.
     */
    void logWarning(const std::string& message) const {
        if (eventSubject_) {
            eventSubject_->notify({Event::RUNTIME_ERROR, "WARNING: " + message, getName()});
        }
    }
};