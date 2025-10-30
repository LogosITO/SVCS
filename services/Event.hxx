/**
 * @file Event.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines the event structure used for the Observer pattern notifications.
 *
 * @russian
 * @brief Определяет структуру события, используемую для уведомлений по шаблону Observer.
 */
#pragma once

#include <string>

/**
 * @english
 * @brief Structure describing an event published by the VCS core.
 * @details This is the "message" that the Subject (Publisher, e.g., Repository)
 * sends to all its Observers.
 *
 * @russian
 * @brief Структура, описывающая событие, опубликованное ядром VCS.
 * @details Это "сообщение", которое Subject (Издатель, например, Repository)
 * отправляет всем своим Observers.
 */
struct Event {
    /**
     * @english
     * @brief Types of events that can be published.
     *
     * @russian
     * @brief Типы событий, которые могут быть опубликованы.
     */
    enum Type {
        /**
         * @english
         * @brief Fatal error that causes application termination.
         *
         * @russian
         * @brief Фатальная ошибка, вызывающая завершение приложения.
         */
        FATAL_ERROR,

        /**
         * @english
         * @brief Runtime error that may allow continued execution.
         *
         * @russian
         * @brief Ошибка выполнения, которая может позволить продолжить выполнение.
         */
        RUNTIME_ERROR,

        /**
         * @english
         * @brief Repository initialization completed successfully.
         *
         * @russian
         * @brief Инициализация репозитория успешно завершена.
         */
        REPOSITORY_INIT_SUCCESS,

        /**
         * @english
         * @brief File staging operation completed successfully.
         *
         * @russian
         * @brief Операция подготовки файлов успешно завершена.
         */
        STAGE_SUCCESS,

        /**
         * @english
         * @brief Save (commit) operation completed successfully.
         *
         * @russian
         * @brief Операция сохранения (коммита) успешно завершена.
         */
        SAVE_SUCCESS,

        /**
         * @english
         * @brief Object write operation completed successfully.
         *
         * @russian
         * @brief Операция записи объекта успешно завершена.
         */
        OBJECT_WRITE_SUCCESS,

        /**
         * @english
         * @brief Object read operation completed successfully.
         *
         * @russian
         * @brief Операция чтения объекта успешно завершена.
         */
        OBJECT_READ_SUCCESS,

        /**
         * @english
         * @brief Index has been updated.
         *
         * @russian
         * @brief Индекс был обновлен.
         */
        INDEX_UPDATE,

        /**
         * @english
         * @brief New commit has been created.
         *
         * @russian
         * @brief Создан новый коммит.
         */
        COMMIT_CREATED,

        /**
         * @english
         * @brief General informational message.
         *
         * @russian
         * @brief Общее информационное сообщение.
         */
        GENERAL_INFO,

        /**
         * @english
         * @brief Help message for user assistance.
         *
         * @russian
         * @brief Справочное сообщение для помощи пользователю.
         */
        HELP_MESSAGE,

        /**
         * @english
         * @brief Debug message for development purposes.
         *
         * @russian
         * @brief Отладочное сообщение для целей разработки.
         */
        DEBUG_MESSAGE,

        /**
         * @english
         * @brief Error message for user notification.
         *
         * @russian
         * @brief Сообщение об ошибке для уведомления пользователя.
         */
        ERROR_MESSAGE,

        /**
         * @english
         * @brief Warning message for potential issues.
         *
         * @russian
         * @brief Предупреждающее сообщение о потенциальных проблемах.
         */
        WARNING_MESSAGE,

        /**
         * @english
         * @brief Protocol operation has started.
         *
         * @russian
         * @brief Операция протокола началась.
         */
        PROTOCOL_START,

        /**
         * @english
         * @brief Protocol operation completed successfully.
         *
         * @russian
         * @brief Операция протокола успешно завершена.
         */
        PROTOCOL_SUCCESS,

        /**
         * @english
         * @brief Protocol operation encountered an error.
         *
         * @russian
         * @brief В операции протокола произошла ошибка.
         */
        PROTOCOL_ERROR,

        /**
         * @english
         * @brief Data has been sent over network.
         *
         * @russian
         * @brief Данные были отправлены по сети.
         */
        NETWORK_SEND,

        /**
         * @english
         * @brief Data has been received over network.
         *
         * @russian
         * @brief Данные были получены по сети.
         */
        NETWORK_RECEIVE,

        /**
         * @english
         * @brief Object transfer in progress.
         *
         * @russian
         * @brief Выполняется передача объекта.
         */
        OBJECT_TRANSFER,

        /**
         * @english
         * @brief Reference (branch/tag) has been updated.
         *
         * @russian
         * @brief Ссылка (ветка/тег) была обновлена.
         */
        REFERENCE_UPDATE,

        /**
         * @english
         * @brief Protocol negotiation phase in progress.
         *
         * @russian
         * @brief Выполняется фаза согласования протокола.
         */
        NEGOTIATION_PHASE
    };

    /**
     * @english
     * @brief The type of event that occurred.
     *
     * @russian
     * @brief Тип произошедшего события.
     */
    Type type;

    /**
     * @english
     * @brief Detailed description or payload (e.g., object hash).
     *
     * @russian
     * @brief Подробное описание или полезная нагрузка (например, хеш объекта).
     */
    std::string details;

    /**
     * @english
     * @brief Source of the event (e.g., "main", "InitCommand", "Repository").
     * @details Added to support three-argument aggregate initialization in notify() calls.
     *
     * @russian
     * @brief Источник события (например, "main", "InitCommand", "Repository").
     * @details Добавлено для поддержки агрегатной инициализации с тремя аргументами в вызовах notify().
     */
    std::string source_name;
};