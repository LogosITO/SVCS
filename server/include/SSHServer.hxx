/**
 * @file SSHServer.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief SSH server for SVCS remote operations
 * @details Main SSH server class header
 *
 * @russian
 * @brief SSH сервер для удаленных операций SVCS
 * @details Заголовок основного класса SSH сервера
 */

#pragma once

#include "SSHConfig.hxx"
#include "SSHSession.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <cstring>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>

namespace svcs::server::ssh {

/**
 * @english
 * @class SSHServer
 * @brief Main SSH server class
 * @details Manages SSH connections and executes repository operations
 *
 * @russian
 * @class SSHServer
 * @brief Основной класс SSH сервера
 * @details Управляет SSH соединениями и выполняет операции с репозиториями
 */
class SSHServer {
public:
    /**
     * @english
     * @brief Constructor
     * @param config Server configuration
     * @param repo_manager Repository manager
     * @param event_bus Event bus
     *
     * @russian
     * @brief Конструктор
     * @param config Конфигурация сервера
     * @param repo_manager Менеджер репозиториев
     * @param event_bus Шина событий
     */
    SSHServer(const SSHConfig& config,
              std::shared_ptr<svcs::core::RepositoryManager> repo_manager,
              std::shared_ptr<svcs::services::ISubject> event_bus);

    /**
     * @english
     * @brief Destructor
     *
     * @russian
     * @brief Деструктор
     */
    ~SSHServer();

    SSHServer(const SSHServer&) = delete;
    SSHServer& operator=(const SSHServer&) = delete;

    /**
     * @english
     * @brief Start server
     * @return true if started
     *
     * @russian
     * @brief Запускает сервер
     * @return true если запущен
     */
    bool start();

    /**
     * @english
     * @brief Stop server
     *
     * @russian
     * @brief Останавливает сервер
     */
    void stop();

    /**
     * @english
     * @brief Check if running
     * @return true if running
     *
     * @russian
     * @brief Проверяет работу
     * @return true если работает
     */
    bool isRunning() const { return running_; }

    /**
     * @english
     * @brief Get active connections
     * @return Connection count
     *
     * @russian
     * @brief Получает активные соединения
     * @return Количество соединений
     */
    size_t getActiveConnections() const;

    /**
     * @english
     * @brief Add public key for user
     * @param username Username
     * @param public_key Public key in OpenSSH format
     *
     * @russian
     * @brief Добавляет публичный ключ для пользователя
     * @param username Имя пользователя
     * @param public_key Публичный ключ в формате OpenSSH
     */
    void addUserKey(const std::string& username,
                   const std::string& public_key);

    /**
     * @english
     * @brief Set password for user
     * @param username Username
     * @param password Plain text password
     *
     * @russian
     * @brief Устанавливает пароль для пользователя
     * @param username Имя пользователя
     * @param password Пароль в открытом виде
     */
    void setUserPassword(const std::string& username,
                        const std::string& password);

private:
    void run();
    void handleClient(int client_fd);
    bool initializeSSH();
    void cleanup();
    void logEvent(const std::string& message, bool is_error = false) const;
    bool handleSVCSCommand(const std::string& command, ssh_channel channel);

    SSHConfig config_;
    std::shared_ptr<svcs::core::RepositoryManager> repo_manager_;
    std::shared_ptr<svcs::services::ISubject> event_bus_;

    std::atomic<bool> running_{false};
    std::thread server_thread_;

    int server_socket_ = -1;
    ssh_bind ssh_bind_ = nullptr;

    mutable std::mutex connections_mutex_;
    std::vector<std::thread> client_threads_;
    std::atomic<size_t> active_connections_{0};
};

}