/**
 * @file SSHServer.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */

#include "SSHServer.hxx"
#include "RemoteProtocol.hxx"
#include "../../services/Event.hxx"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <map>
#include <cstring>

namespace svcs::server::ssh {

using namespace svcs::core;
using namespace svcs::services;

namespace fs = std::filesystem;

namespace {

int createServerSocket(const std::string& host, uint16_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(sock);
        return -1;
    }

    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(sock);
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (host == "0.0.0.0") {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    if (listen(sock, 128) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

bool generateHostKey(const std::string& key_path) {
    if (fs::exists(key_path)) {
        return true;
    }

    fs::path key_dir = fs::path(key_path).parent_path();
    if (!key_dir.empty() && !fs::exists(key_dir)) {
        fs::create_directories(key_dir);
    }

    ssh_key key = nullptr;
    if (ssh_pki_generate(SSH_KEYTYPE_RSA, 2048, &key) != SSH_OK) {
        return false;
    }

    bool success = true;

    if (ssh_pki_export_privkey_file(key, nullptr, nullptr, nullptr, key_path.c_str()) != SSH_OK) {
        success = false;
    }

    std::string pubkey_path = key_path + ".pub";
    if (ssh_pki_export_pubkey_file(key, pubkey_path.c_str()) != SSH_OK) {
        success = false;
    }

    ssh_key_free(key);

    if (success) {
        ::chmod(key_path.c_str(), 0600);
        ::chmod(pubkey_path.c_str(), 0644);
    }

    return success;
}

std::string hashPassword(const std::string& password) {
    // Простой хэш для теста - в реальном проекте используйте bcrypt/scrypt
    unsigned char salt[16];
    if (RAND_bytes(salt, sizeof(salt)) != 1) {
        return "";
    }

    std::stringstream salt_ss;
    for (int i = 0; i < 16; i++) {
        salt_ss << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(salt[i]);
    }
    std::string salt_str = salt_ss.str();

    // Используем EVP для совместимости с OpenSSL 3.0
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        return "";
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    std::string salted = salt_str + password;
    if (EVP_DigestUpdate(mdctx, salted.c_str(), salted.size()) != 1) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

    std::stringstream hash_ss;
    for (unsigned int i = 0; i < hash_len; i++) {
        hash_ss << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(hash[i]);
    }

    return "sha256$" + salt_str + "$" + hash_ss.str();
}

bool writePasswordFile(const std::string& path,
                      const std::map<std::string, std::string>& users) {
    std::ofstream file(path);
    if (!file) {
        return false;
    }

    for (const auto& pair : users) {
        file << pair.first << ":" << pair.second << "\n";
    }

    return file.good();
}

std::map<std::string, std::string> readPasswordFile(const std::string& path) {
    std::map<std::string, std::string> users;

    std::ifstream file(path);
    if (!file) {
        return users;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string username = line.substr(0, colon);
            std::string hash = line.substr(colon + 1);
            users[username] = hash;
        }
    }

    return users;
}

void writeAuthorizedKey(const std::string& username,
                       const std::string& public_key,
                       const std::string& keys_dir) {
    std::string user_dir = keys_dir + "/" + username;
    fs::create_directories(user_dir);

    std::string key_path = user_dir + "/authorized_keys";
    std::ofstream file(key_path, std::ios::app);
    if (file) {
        file << public_key << "\n";
        ::chmod(key_path.c_str(), 0600);
    }
}

// Альтернативная функция для получения IP клиента
std::string getClientIpFromSocket(int sock) {
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

} // anonymous namespace

SSHServer::SSHServer(const SSHConfig& config,
                     std::shared_ptr<RepositoryManager> repo_manager,
                     std::shared_ptr<ISubject> event_bus)
    : config_(config)
    , repo_manager_(std::move(repo_manager))
    , event_bus_(std::move(event_bus)) {

    if (!config_.host_key.empty()) {
        fs::path key_path(config_.host_key);
        fs::create_directories(key_path.parent_path());
    }

    if (!config_.keys_dir.empty()) {
        fs::create_directories(config_.keys_dir);
    }

    if (!config_.passwd_dir.empty()) {
        fs::create_directories(config_.passwd_dir);
    }
}

SSHServer::~SSHServer() {
    stop();
}

bool SSHServer::start() {
    if (running_) {
        return true;
    }

    if (!initializeSSH()) {
        logEvent("Failed to initialize SSH", true);
        return false;
    }

    server_socket_ = createServerSocket(config_.host, config_.port);
    if (server_socket_ < 0) {
        logEvent("Failed to create server socket", true);
        cleanup();
        return false;
    }

    running_ = true;
    server_thread_ = std::thread(&SSHServer::run, this);

    logEvent("SSH server started on " + config_.host + ":" +
             std::to_string(config_.port));

    return true;
}

void SSHServer::stop() {
    if (!running_) {
        return;
    }

    running_ = false;

    if (server_thread_.joinable()) {
        server_thread_.join();
    }

    cleanup();

    logEvent("SSH server stopped");
}

size_t SSHServer::getActiveConnections() const {
    return active_connections_.load();
}

void SSHServer::addUserKey(const std::string& username,
                          const std::string& public_key) {
    if (username.empty() || public_key.empty()) {
        return;
    }

    writeAuthorizedKey(username, public_key, config_.keys_dir);
    logEvent("Added public key for user: " + username);
}

void SSHServer::setUserPassword(const std::string& username,
                               const std::string& password) {
    if (username.empty() || password.empty()) {
        return;
    }

    std::string passwd_path = config_.passwd_dir + "/svcs.passwd";

    auto users = readPasswordFile(passwd_path);
    std::string hash = hashPassword(password);

    if (!hash.empty()) {
        users[username] = hash;
        if (writePasswordFile(passwd_path, users)) {
            ::chmod(passwd_path.c_str(), 0600);
            logEvent("Set password for user: " + username);
        }
    }
}

void SSHServer::run() {
    fd_set read_fds;
    timeval timeout;

    while (running_) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket_, &read_fds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(server_socket_ + 1, &read_fds, nullptr, nullptr, &timeout);

        if (activity < 0 && errno != EINTR) {
            logEvent("select() error", true);
            break;
        }

        if (activity > 0 && FD_ISSET(server_socket_, &read_fds)) {
            sockaddr_in client_addr{};
            socklen_t addr_len = sizeof(client_addr);

            int client_fd = accept(server_socket_,
                                  (sockaddr*)&client_addr,
                                  &addr_len);

            if (client_fd < 0) {
                if (errno != EWOULDBLOCK && errno != EAGAIN) {
                    logEvent("accept() error", true);
                }
                continue;
            }

            if (active_connections_ >= config_.max_connections) {
                logEvent("Max connections reached, rejecting client", true);
                close(client_fd);
                continue;
            }

            active_connections_++;

            std::thread client_thread(&SSHServer::handleClient, this, client_fd);

            std::lock_guard<std::mutex> lock(connections_mutex_);
            client_threads_.push_back(std::move(client_thread));

            client_threads_.erase(
                std::remove_if(client_threads_.begin(), client_threads_.end(),
                    [](std::thread& t) {
                        return !t.joinable();
                    }),
                client_threads_.end()
            );
        }
    }
}

void SSHServer::handleClient(int client_fd) {
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags & ~O_NONBLOCK);

    ssh_session session = ssh_new();
    if (!session) {
        close(client_fd);
        active_connections_--;
        return;
    }

    if (ssh_bind_accept(ssh_bind_, session) != SSH_OK) {
        ssh_free(session);
        close(client_fd);
        active_connections_--;
        return;
    }

    ssh_set_blocking(session, 1);

    // Получаем IP через сокет
    std::string ip_str = getClientIpFromSocket(client_fd);
    if (ip_str.empty()) {
        ip_str = "unknown";
    }

    logEvent("Client connected: " + ip_str);

    auto handler = [this](const std::string& cmd, ssh_channel chan) {
        return handleSVCSCommand(cmd, chan);
    };

    SSHSession ssh_session(session, config_, handler);

    while (running_ && ssh_session.isAlive()) {
        if (!ssh_session.process()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    close(client_fd);
    active_connections_--;

    logEvent("Client disconnected: " + ip_str);
}

bool SSHServer::initializeSSH() {
    ssh_bind_ = ssh_bind_new();
    if (!ssh_bind_) {
        return false;
    }

    ssh_bind_options_set(ssh_bind_, SSH_BIND_OPTIONS_BINDADDR, config_.host.c_str());
    ssh_bind_options_set(ssh_bind_, SSH_BIND_OPTIONS_BINDPORT, &config_.port);
    ssh_bind_options_set(ssh_bind_, SSH_BIND_OPTIONS_HOSTKEY, "ssh-rsa");

    if (!config_.host_key.empty() && !generateHostKey(config_.host_key)) {
        logEvent("Failed to generate host key: " + config_.host_key, true);
        ssh_bind_free(ssh_bind_);
        ssh_bind_ = nullptr;
        return false;
    }

    if (!config_.host_key.empty()) {
        if (ssh_bind_options_set(ssh_bind_,
                                SSH_BIND_OPTIONS_RSAKEY,
                                config_.host_key.c_str()) < 0) {
            logEvent("Failed to set host key: " + config_.host_key, true);
            ssh_bind_free(ssh_bind_);
            ssh_bind_ = nullptr;
            return false;
        }
    }

    // Упрощенная настройка - без опции SSH_BIND_OPTIONS_AUTH_METHODS
    if (ssh_bind_listen(ssh_bind_) < 0) {
        logEvent("Failed to start SSH listener: " +
                std::string(ssh_get_error(ssh_bind_)), true);
        ssh_bind_free(ssh_bind_);
        ssh_bind_ = nullptr;
        return false;
    }

    return true;
}

void SSHServer::cleanup() {
    if (ssh_bind_) {
        ssh_bind_free(ssh_bind_);
        ssh_bind_ = nullptr;
    }

    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }

    std::lock_guard<std::mutex> lock(connections_mutex_);
    for (auto& thread : client_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    client_threads_.clear();

    active_connections_ = 0;
}

void SSHServer::logEvent(const std::string& message, bool is_error) const {
    if (event_bus_) {
        Event event;

        if (is_error) {
            event.type = Event::ERROR_MESSAGE;
        } else {
            event.type = Event::GENERAL_INFO;
        }

        event.source_name = "SSHServer";
        event.details = message;

        event_bus_->notify(event);
    }
}

bool SSHServer::handleSVCSCommand(const std::string& command, ssh_channel channel) {
    if (command.find("svcs-upload-pack") == 0) {
        size_t start = command.find('\'');
        size_t end = command.rfind('\'');
        if (start == std::string::npos || end == std::string::npos || start >= end) {
            const char* msg = "SVCS: Invalid command format\n";
            ssh_channel_write(channel, msg, std::strlen(msg));
            return false;
        }

        std::string repo_path = command.substr(start + 1, end - start - 1);

        auto protocol = std::make_unique<RemoteProtocol>(event_bus_, repo_manager_);

        const char* start_msg = "SVCS: Starting upload-pack\n";
        ssh_channel_write(channel, start_msg, std::strlen(start_msg));

        bool success = protocol->handleUploadPack();

        if (success) {
            const char* msg = "SVCS: Upload-pack completed\n";
            ssh_channel_write(channel, msg, std::strlen(msg));
        } else {
            const char* msg = "SVCS: Upload-pack failed\n";
            ssh_channel_write(channel, msg, std::strlen(msg));
        }

        return success;
    } else if (command.find("svcs-receive-pack") == 0) {
        size_t start = command.find('\'');
        size_t end = command.rfind('\'');
        if (start == std::string::npos || end == std::string::npos || start >= end) {
            const char* msg = "SVCS: Invalid command format\n";
            ssh_channel_write(channel, msg, std::strlen(msg));
            return false;
        }

        std::string repo_path = command.substr(start + 1, end - start - 1);

        auto protocol = std::make_unique<RemoteProtocol>(event_bus_, repo_manager_);

        const char* start_msg = "SVCS: Starting receive-pack\n";
        ssh_channel_write(channel, start_msg, std::strlen(start_msg));

        bool success = protocol->handleReceivePack();

        if (success) {
            const char* msg = "SVCS: Receive-pack completed\n";
            ssh_channel_write(channel, msg, std::strlen(msg));
        } else {
            const char* msg = "SVCS: Receive-pack failed\n";
            ssh_channel_write(channel, msg, std::strlen(msg));
        }

        return success;
    }

    const char* msg = "SVCS: Unknown command\n";
    ssh_channel_write(channel, msg, std::strlen(msg));
    return false;
}

}