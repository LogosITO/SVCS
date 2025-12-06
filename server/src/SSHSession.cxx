/**
 * @file SSHSession.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */

#include "SSHSession.hxx"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace svcs::server::ssh {

namespace fs = std::filesystem;

SSHSession::SSHSession(ssh_session session,
                       const SSHConfig& config,
                       CommandHandler handler)
    : session_(session)
    , config_(config)
    , handler_(std::move(handler))
    , start_time_(std::chrono::system_clock::now()) {
}

SSHSession::~SSHSession() {
    close();
}

SSHSession::SSHSession(SSHSession&& other) noexcept
    : session_(other.session_)
    , config_(other.config_)
    , handler_(std::move(other.handler_))
    , username_(std::move(other.username_))
    , authenticated_(other.authenticated_)
    , start_time_(other.start_time_)
    , owns_session_(other.owns_session_) {
    other.session_ = nullptr;
    other.owns_session_ = false;
}

SSHSession& SSHSession::operator=(SSHSession&& other) noexcept {
    if (this != &other) {
        close();

        session_ = other.session_;
        config_ = other.config_;
        handler_ = std::move(other.handler_);
        username_ = std::move(other.username_);
        authenticated_ = other.authenticated_;
        start_time_ = other.start_time_;
        owns_session_ = other.owns_session_;

        other.session_ = nullptr;
        other.owns_session_ = false;
    }
    return *this;
}

bool SSHSession::process() {
    if (!session_ || !owns_session_) {
        return false;
    }

    if (!authenticated_) {
        // Простая аутентификация - принимаем всех
        username_ = "testuser";
        authenticated_ = true;
        return true;
    }

    return handleCommand();
}

bool SSHSession::isAlive() const {
    return session_ && owns_session_ && ssh_is_connected(session_);
}

std::string SSHSession::getClientIp() const {
    if (!session_ || !owns_session_) {
        return "";
    }

    // Альтернативный способ получения IP через сокетный дескриптор
    int sock = ssh_get_fd(session_);
    if (sock < 0) {
        return "";
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (getpeername(sock, (struct sockaddr*)&addr, &addr_len) < 0) {
        return "";
    }

    char ip_str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str)) == nullptr) {
        return "";
    }

    return std::string(ip_str);
}

void SSHSession::close() {
    if (session_ && owns_session_) {
        ssh_disconnect(session_);
        ssh_free(session_);
        session_ = nullptr;
        owns_session_ = false;
    }
}

bool SSHSession::authenticate() {
    if (!session_ || !owns_session_) {
        return false;
    }

    // Упрощенная аутентификация для теста
    // В реальном проекте используйте ssh_userauth_none() и ssh_userauth_password()

    // Просто ждем и устанавливаем флаг аутентификации
    username_ = "anonymous";
    authenticated_ = true;

    return true;
}

bool SSHSession::handleCommand() {
    if (!session_ || !owns_session_ || !authenticated_) {
        return false;
    }

    ssh_channel channel = ssh_channel_new(session_);
    if (!channel) {
        return false;
    }

    if (ssh_channel_open_session(channel) != SSH_OK) {
        ssh_channel_free(channel);
        return false;
    }

    // Упрощенная обработка команд
    // Читаем из канала и выполняем команду
    char buffer[1024];
    int nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);

    if (nbytes > 0) {
        std::string command(buffer, nbytes);

        // Убираем перевод строки в конце
        if (!command.empty() && command.back() == '\n') {
            command.pop_back();
        }

        if (handler_) {
            bool success = handler_(command, channel);

            ssh_channel_send_eof(channel);
            ssh_channel_close(channel);
            ssh_channel_free(channel);

            return success;
        }
    }

    ssh_channel_free(channel);
    return false;
}

bool SSHSession::validatePublicKey(ssh_key key) {
    // Упрощенная проверка для теста
    return true;
}

bool SSHSession::validatePassword(const std::string& username,
                                 const std::string& password) {
    // Упрощенная проверка для теста
    return true;
}

std::vector<std::string> SSHSession::loadAuthorizedKeys(const std::string& username) const {
    std::vector<std::string> keys;

    if (config_.keys_dir.empty()) {
        return keys;
    }

    std::string key_path = config_.keys_dir + "/" + username + "/authorized_keys";
    if (!fs::exists(key_path)) {
        key_path = config_.keys_dir + "/authorized_keys";
    }

    if (!fs::exists(key_path)) {
        return keys;
    }

    std::ifstream file(key_path);
    if (!file) {
        return keys;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '#') {
            keys.push_back(line);
        }
    }

    return keys;
}

std::string SSHSession::getPasswordHash(const std::string& username) const {
    if (config_.passwd_dir.empty()) {
        return "";
    }

    std::string passwd_path = config_.passwd_dir + "/svcs.passwd";
    if (!fs::exists(passwd_path)) {
        return "";
    }

    std::ifstream file(passwd_path);
    if (!file) {
        return "";
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string file_username = line.substr(0, colon);
            if (file_username == username) {
                return line.substr(colon + 1);
            }
        }
    }

    return "";
}

}