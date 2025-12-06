/**
 * @file SSHSession.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief SSH session handler
 * @details Header for SSH client session management
 *
 * @russian
 * @brief Обработчик SSH сессии
 * @details Заголовок для управления SSH сессиями клиентов
 */
#pragma once

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "SSHConfig.hxx"
#include <libssh/libssh.h>
#include <libssh/server.h>
#include <memory>
#include <string>
#include <functional>
#include <chrono>

namespace svcs::server::ssh {

/**
 * @english
 * @class SSHSession
 * @brief Single SSH client session
 * @details Handles authentication and command execution for one client
 *
 * @russian
 * @class SSHSession
 * @brief Одна SSH сессия клиента
 * @details Обрабатывает аутентификацию и выполнение команд для одного клиента
 */
class SSHSession {
public:
    /**
     * @english
     * @brief Command execution callback
     * @param command Command to execute
     * @param channel SSH channel for I/O
     * @return true if command executed successfully
     *
     * @russian
     * @brief Коллбек выполнения команд
     * @param command Команда для выполнения
     * @param channel SSH канал для ввода/вывода
     * @return true если команда выполнена успешно
     */
    using CommandHandler = std::function<bool(const std::string&, ssh_channel)>;

    /**
     * @english
     * @brief Constructor
     * @param session libssh session (takes ownership)
     * @param config SSH server configuration
     * @param handler Command handler callback
     *
     * @russian
     * @brief Конструктор
     * @param session libssh сессия (забирает владение)
     * @param config Конфигурация SSH сервера
     * @param handler Коллбек обработчика команд
     */
    SSHSession(ssh_session session,
               const SSHConfig& config,
               CommandHandler handler);

    /**
     * @english
     * @brief Destructor
     *
     * @russian
     * @brief Деструктор
     */
    ~SSHSession();

    // Запрещаем копирование
    SSHSession(const SSHSession&) = delete;
    SSHSession& operator=(const SSHSession&) = delete;

    // Разрешаем перемещение
    SSHSession(SSHSession&& other) noexcept;
    SSHSession& operator=(SSHSession&& other) noexcept;

    /**
     * @english
     * @brief Process session events
     * @return true if session should continue
     *
     * @russian
     * @brief Обрабатывает события сессии
     * @return true если сессия должна продолжиться
     */
    bool process();

    /**
     * @english
     * @brief Check if session is alive
     * @return true if session active
     *
     * @russian
     * @brief Проверяет, активна ли сессия
     * @return true если сессия активна
     */
    bool isAlive() const;

    /**
     * @english
     * @brief Get authenticated username
     * @return Username or empty string
     *
     * @russian
     * @brief Получает аутентифицированное имя пользователя
     * @return Имя пользователя или пустая строка
     */
    std::string getUsername() const { return username_; }

    /**
     * @english
     * @brief Get client IP address
     * @return IP address
     *
     * @russian
     * @brief Получает IP адрес клиента
     * @return IP адрес
     */
    std::string getClientIp() const;

    /**
     * @english
     * @brief Get session start time
     * @return Time point when session started
     *
     * @russian
     * @brief Получает время начала сессии
     * @return Момент времени когда сессия началась
     */
    std::chrono::system_clock::time_point getStartTime() const { return start_time_; }

    /**
     * @english
     * @brief Force close session
     *
     * @russian
     * @brief Принудительно закрывает сессию
     */
    void close();

private:
    bool authenticate();
    bool handleCommand();
    bool validatePublicKey(ssh_key key);
    bool validatePassword(const std::string& username, const std::string& password);
    std::vector<std::string> loadAuthorizedKeys(const std::string& username) const;
    std::string getPasswordHash(const std::string& username) const;

private:
    ssh_session session_;                 /**< @english libssh session @russian libssh сессия */
    SSHConfig config_;                    /**< @english SSH configuration @russian SSH конфигурация */
    CommandHandler handler_;              /**< @english Command handler @russian Обработчик команд */
    std::string username_;                /**< @english Authenticated username @russian Аутентифицированное имя пользователя */
    bool authenticated_ = false;          /**< @english Authentication status @russian Статус аутентификации */
    std::chrono::system_clock::time_point start_time_; /**< @english Session start time @russian Время начала сессии */
    bool owns_session_ = true;            /**< @english Ownership flag @russian Флаг владения */
};

}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif