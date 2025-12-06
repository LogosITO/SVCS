/**
 * @file SSHConfig.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief SSH server configuration
 * @details Configuration structure for SSH server
 *
 * @russian
 * @brief Конфигурация SSH сервера
 * @details Структура конфигурации для SSH сервера
 */
#pragma once

#include <string>
#include <cstdint>

namespace svcs::server::ssh {

/**
 * @english
 * @struct SSHConfig
 * @brief SSH server configuration
 *
 * @russian
 * @struct SSHConfig
 * @brief Конфигурация SSH сервера
 */
struct SSHConfig {
    std::string host = "0.0.0.0";               /**< @english Bind host @russian Хост для привязки */
    uint16_t port = 2222;                       /**< @english SSH port @russian SSH порт */
    std::string host_key;                       /**< @english Host key path @russian Путь к ключу хоста */
    std::string auth_file;                      /**< @english Auth file path @russian Путь к файлу аутентификации */
    std::string keys_dir = "/etc/svcs/ssh/keys"; /**< @english Keys directory @russian Директория ключей */
    std::string passwd_dir = "/etc/svcs/ssh";   /**< @english Password directory @russian Директория паролей */
    size_t max_connections = 256;               /**< @english Max connections @russian Максимум соединений */
    int timeout_sec = 300;                      /**< @english Connection timeout @russian Таймаут соединения */

    /**
     * @english
     * @brief Load from file
     * @param path File path
     * @return Configuration
     *
     * @russian
     * @brief Загружает из файла
     * @param path Путь к файлу
     * @return Конфигурация
     */
    static SSHConfig load(const std::string& path);

    /**
     * @english
     * @brief Save to file
     * @param path File path
     * @return true if saved successfully
     *
     * @russian
     * @brief Сохраняет в файл
     * @param path Путь к файлу
     * @return true если сохранено успешно
     */
    bool save(const std::string& path) const;

    /**
     * @english
     * @brief Validate configuration
     * @return true if configuration is valid
     *
     * @russian
     * @brief Проверяет конфигурацию
     * @return true если конфигурация валидна
     */
    bool validate() const;
};

} // namespace svcs::server::ssh