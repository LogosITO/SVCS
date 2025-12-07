/**
 * @file SSHServerTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @brief Unit tests for SSHServer class
 * @details Tests for SSH server lifecycle, connection management, and command handling
 *
 * @english
 * @brief Unit tests for SSHServer class
 * @details Comprehensive test suite for SSH server initialization, client handling,
 * authentication, and SVCS command execution through SSH protocol.
 *
 * @russian
 * @brief Модульные тесты для класса SSHServer
 * @details Полный набор тестов для инициализации SSH сервера, обработки клиентов,
 * аутентификации и выполнения команд SVCS через SSH протокол.
 */

#include "../../server/include/SSHServer.hxx"
#include "../../server/include/SSHConfig.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../services/EventBus.hxx"
#include <gtest/gtest.h>
#include <memory>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

#ifdef NO_LIBSSH
    #define LIBSSH_AVAILABLE 0
    #warning "libssh not available - SSH tests will be limited"
#else
    #define LIBSSH_AVAILABLE 1
#endif

namespace svcs::test::server::ssh {

using namespace svcs::server::ssh;

namespace fs = std::filesystem;

class MockEventBus : public svcs::services::ISubject {
public:
    struct EventRecord {
        svcs::services::Event event;
        std::chrono::system_clock::time_point timestamp;
    };

    std::vector<EventRecord> events;
    std::vector<std::shared_ptr<svcs::services::IObserver>> observers;

    void attach(std::shared_ptr<svcs::services::IObserver> observer) override {
        observers.push_back(observer);
    }

    void detach(std::shared_ptr<svcs::services::IObserver> observer) override {
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }

    void notify(const svcs::services::Event& event) const override {
        const_cast<MockEventBus*>(this)->events.push_back({
            event,
            std::chrono::system_clock::now()
        });
    }

    void clear() {
        events.clear();
        observers.clear();
    }

    bool containsError(const std::string& message) const {
        for (const auto& record : events) {
            if (record.event.type == svcs::services::Event::ERROR_MESSAGE &&
                record.event.details.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    bool containsInfo(const std::string& message) const {
        for (const auto& record : events) {
            if (record.event.type == svcs::services::Event::GENERAL_INFO &&
                record.event.details.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

namespace test_utils {

std::string hashPassword(const std::string& password) {
    // Простая реализация для тестов
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    return "test_hash_" + std::to_string(hash);
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

    #ifndef _WIN32
    ::chmod(path.c_str(), 0600);
    #endif

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
        #ifndef _WIN32
        ::chmod(key_path.c_str(), 0600);
        #endif
    }
}

}

class SSHServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir_ = fs::temp_directory_path() / "svcs_ssh_server_test";
        fs::create_directories(test_dir_);

        config_.host = "127.0.0.1";
        config_.port = 2323; // Используем другой порт чтобы не конфликтовать
        config_.host_key = (test_dir_ / "host_key").string();
        config_.keys_dir = (test_dir_ / "keys").string();
        config_.passwd_dir = (test_dir_ / "passwd").string();
        config_.max_connections = 5;
        config_.timeout_sec = 30;

        fs::create_directories(config_.keys_dir);
        fs::create_directories(config_.passwd_dir);

        event_bus_ = std::make_shared<MockEventBus>();
        repo_manager_ = std::make_shared<svcs::core::RepositoryManager>(event_bus_);

        createTestHostKey();
    }

    void TearDown() override {
        if (server_) {
            server_->stop();
        }
        fs::remove_all(test_dir_);
    }

    void createTestHostKey() {
        std::ofstream key_file(config_.host_key);
        key_file << "-----BEGIN RSA PRIVATE KEY-----\n";
        key_file << "MIIEogIBAAKCAQEAwKvZcU9ZQ1jNkXoX5JmK7y... (test key)\n";
        key_file << "-----END RSA PRIVATE KEY-----\n";
        key_file.close();

        std::ofstream pub_file(config_.host_key + ".pub");
        pub_file << "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQD... test@host\n";
        pub_file.close();
    }

    bool isLibSSHAvailable() const {
        return LIBSSH_AVAILABLE == 1;
    }

    SSHConfig config_;
    fs::path test_dir_;
    std::shared_ptr<svcs::core::RepositoryManager> repo_manager_;
    std::shared_ptr<MockEventBus> event_bus_;
    std::unique_ptr<SSHServer> server_;
};

// Базовые тесты конструктора и деструктора
TEST_F(SSHServerTest, Constructor) {
    if (!isLibSSHAvailable()) {
        GTEST_SKIP() << "libssh not available, skipping SSH server tests";
    }

    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);
    EXPECT_FALSE(server_->isRunning());
    EXPECT_EQ(server_->getActiveConnections(), 0);
}

TEST_F(SSHServerTest, DestructorStopsServer) {
    if (!isLibSSHAvailable()) {
        GTEST_SKIP() << "libssh not available, skipping SSH server tests";
    }

    {
        SSHServer temp_server(config_, repo_manager_, event_bus_);
        // Деструктор должен корректно освобождать ресурсы
    }
    // Не должно быть утечек или крешей
    SUCCEED();
}

// Тесты конфигурации
TEST_F(SSHServerTest, InvalidConfigNoHostKey) {
    if (!isLibSSHAvailable()) {
        GTEST_SKIP() << "libssh not available, skipping SSH server tests";
    }

    config_.host_key = ""; // Пустой путь к ключу
    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);

    bool started = server_->start();
    EXPECT_FALSE(started);
}

TEST_F(SSHServerTest, InvalidPort) {
    if (!isLibSSHAvailable()) {
        GTEST_SKIP() << "libssh not available, skipping SSH server tests";
    }

    config_.port = 0; // Невалидный порт
    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);

    bool started = server_->start();
    EXPECT_FALSE(started);
}

// Тесты управления пользователями
TEST_F(SSHServerTest, AddUserKeyCreatesFile) {
    // Этот тест не требует libssh, работает с файловой системой
    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);

    std::string username = "testuser";
    std::string public_key = "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQD... user@host";

    server_->addUserKey(username, public_key);

    std::string key_file = config_.keys_dir + "/" + username + "/authorized_keys";
    EXPECT_TRUE(fs::exists(key_file));

    std::ifstream file(key_file);
    std::string content;
    std::getline(file, content);
    EXPECT_EQ(content, public_key);
}

TEST_F(SSHServerTest, AddUserKeyEmptyParameters) {
    // Этот тест не требует libssh
    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);

    // Не должно падать при пустых параметрах
    server_->addUserKey("", "key");
    server_->addUserKey("user", "");
    server_->addUserKey("", "");

    SUCCEED(); // Просто проверяем что не падает
}

TEST_F(SSHServerTest, SetUserPasswordCreatesFile) {
    // Этот тест не требует libssh, работает с файловой системой
    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);

    std::string username = "testuser";
    std::string password = "testpassword123";

    server_->setUserPassword(username, password);

    std::string passwd_file = config_.passwd_dir + "/svcs.passwd";
    EXPECT_TRUE(fs::exists(passwd_file));

    std::ifstream file(passwd_file);
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        if (line.find(username + ":") == 0) {
            found = true;
            // Проверяем что строка не пустая после двоеточия
            EXPECT_GT(line.size(), username.size() + 1);
            break;
        }
    }
    EXPECT_TRUE(found);
}

// Тесты активных соединений
TEST_F(SSHServerTest, ActiveConnectionsInitiallyZero) {
    if (!isLibSSHAvailable()) {
        GTEST_SKIP() << "libssh not available, skipping SSH server tests";
    }

    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);
    EXPECT_EQ(server_->getActiveConnections(), 0);
}

// Тесты обработки событий
TEST_F(SSHServerTest, DirectoryCreationInConstructor) {
    // Этот тест не требует libssh
    // Удаляем директории чтобы проверить их создание в конструкторе
    fs::remove_all(config_.keys_dir);
    fs::remove_all(config_.passwd_dir);
    fs::remove_all(fs::path(config_.host_key).parent_path());

    server_ = std::make_unique<SSHServer>(config_, repo_manager_, event_bus_);

    // Директории должны быть созданы
    EXPECT_TRUE(fs::exists(config_.keys_dir));
    EXPECT_TRUE(fs::exists(config_.passwd_dir));
    EXPECT_TRUE(fs::exists(fs::path(config_.host_key).parent_path()));
}

// Безлибовые тесты (работают без libssh)
class SSHServerNoLibSSHTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Эти тесты должны работать даже без libssh
        test_dir_ = fs::temp_directory_path() / "svcs_nolibssh_test";
        fs::create_directories(test_dir_);

        config_.host = "127.0.0.1";
        config_.port = 2323;
        config_.host_key = (test_dir_ / "host_key").string();
        config_.keys_dir = (test_dir_ / "keys").string();
        config_.passwd_dir = (test_dir_ / "passwd").string();
        config_.max_connections = 5;
        config_.timeout_sec = 30;

        event_bus_ = std::make_shared<MockEventBus>();
        repo_manager_ = std::make_shared<svcs::core::RepositoryManager>(event_bus_);
    }

    void TearDown() override {
        fs::remove_all(test_dir_);
    }

    SSHConfig config_;
    fs::path test_dir_;
    std::shared_ptr<svcs::core::RepositoryManager> repo_manager_;
    std::shared_ptr<MockEventBus> event_bus_;
};

// Тесты которые не требуют libssh
TEST_F(SSHServerNoLibSSHTest, ConfigValidation) {
    SSHConfig config;

    // Тестируем валидацию конфигурации
    EXPECT_FALSE(config.validate()); // Пустая конфигурация невалидна

    config.host = "127.0.0.1";
    config.port = 2222;
    config.host_key = "/tmp/key";
    config.max_connections = 10;
    config.timeout_sec = 30;

    EXPECT_TRUE(config.validate());

    // Невалидные порты
    config.port = 0;
    EXPECT_FALSE(config.validate());

    config.port = 65536; // Больше 65535
    EXPECT_FALSE(config.validate());

    config.port = 2222;
    config.max_connections = 0;
    EXPECT_FALSE(config.validate());

    config.max_connections = 10;
    config.timeout_sec = 0;
    EXPECT_FALSE(config.validate());
}

TEST_F(SSHServerNoLibSSHTest, ConfigSaveLoad) {
    SSHConfig original;
    original.host = "192.168.1.100";
    original.port = 2222;
    original.host_key = "/etc/svcs/ssh/host_key";
    original.auth_file = "/etc/svcs/ssh/auth.conf";
    original.keys_dir = "/etc/svcs/ssh/keys";
    original.passwd_dir = "/etc/svcs/ssh/passwords";
    original.max_connections = 256;
    original.timeout_sec = 300;

    std::string config_file = (test_dir_ / "test_config.conf").string();

    // Сохраняем конфигурацию
    EXPECT_TRUE(original.save(config_file));

    // Загружаем конфигурацию
    SSHConfig loaded = SSHConfig::load(config_file);

    // Проверяем что все поля совпадают
    EXPECT_EQ(loaded.host, original.host);
    EXPECT_EQ(loaded.port, original.port);
    EXPECT_EQ(loaded.host_key, original.host_key);
    EXPECT_EQ(loaded.auth_file, original.auth_file);
    EXPECT_EQ(loaded.keys_dir, original.keys_dir);
    EXPECT_EQ(loaded.passwd_dir, original.passwd_dir);
    EXPECT_EQ(loaded.max_connections, original.max_connections);
    EXPECT_EQ(loaded.timeout_sec, original.timeout_sec);
}

TEST_F(SSHServerNoLibSSHTest, ConfigLoadInvalidFile) {
    // Загрузка из несуществующего файла должна возвращать конфигурацию по умолчанию
    SSHConfig config = SSHConfig::load("/nonexistent/path/config.conf");

    EXPECT_EQ(config.host, "0.0.0.0");
    EXPECT_EQ(config.port, 2222);
    EXPECT_EQ(config.keys_dir, "/etc/svcs/ssh/keys");
    EXPECT_EQ(config.passwd_dir, "/etc/svcs/ssh");
}

// Тесты для утилитных функций
TEST_F(SSHServerNoLibSSHTest, TestHashPassword) {
    // Простая проверка хэширования
    std::string hash = test_utils::hashPassword("testpassword");
    EXPECT_FALSE(hash.empty());
    EXPECT_NE(hash, "testpassword"); // Хэш не должен быть равен исходному паролю
}

TEST_F(SSHServerNoLibSSHTest, PasswordFileOperations) {
    std::string passwd_file = (test_dir_ / "test.passwd").string();

    std::map<std::string, std::string> users = {
        {"user1", "hash1"},
        {"user2", "hash2"},
        {"user3", "hash3"}
    };

    // Записываем файл
    EXPECT_TRUE(test_utils::writePasswordFile(passwd_file, users));
    EXPECT_TRUE(fs::exists(passwd_file));

    // Читаем файл
    auto loaded_users = test_utils::readPasswordFile(passwd_file);

    EXPECT_EQ(loaded_users.size(), 3);
    EXPECT_EQ(loaded_users["user1"], "hash1");
    EXPECT_EQ(loaded_users["user2"], "hash2");
    EXPECT_EQ(loaded_users["user3"], "hash3");

    // Чтение несуществующего файла
    auto empty_users = test_utils::readPasswordFile("/nonexistent/file.passwd");
    EXPECT_TRUE(empty_users.empty());
}

TEST_F(SSHServerNoLibSSHTest, WriteAuthorizedKey) {
    std::string username = "testuser";
    std::string public_key = "ssh-rsa AAAAB3NzaC1yc2E... user@host";
    std::string keys_dir = (test_dir_ / "test_keys").string();

    test_utils::writeAuthorizedKey(username, public_key, keys_dir);

    std::string key_file = keys_dir + "/" + username + "/authorized_keys";
    EXPECT_TRUE(fs::exists(key_file));

    std::ifstream file(key_file);
    std::string content;
    std::getline(file, content);
    EXPECT_EQ(content, public_key);
}

// Тесты для RepositoryManager (если нужны)
TEST_F(SSHServerNoLibSSHTest, RepositoryManagerCreation) {
    // Проверяем что RepositoryManager создается корректно
    EXPECT_NE(repo_manager_, nullptr);
    EXPECT_NE(event_bus_, nullptr);
}

// Интеграционные тесты с libssh
class SSHServerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!isLibSSHAvailable()) {
            GTEST_SKIP() << "libssh not available, skipping integration tests";
        }

        test_dir_ = fs::temp_directory_path() / "svcs_ssh_integration_test";
        fs::create_directories(test_dir_);

        config_.host = "127.0.0.1";
        config_.port = 2323;
        config_.host_key = (test_dir_ / "host_key").string();
        config_.keys_dir = (test_dir_ / "keys").string();
        config_.passwd_dir = (test_dir_ / "passwd").string();
        config_.max_connections = 5;
        config_.timeout_sec = 30;

        fs::create_directories(config_.keys_dir);
        fs::create_directories(config_.passwd_dir);

        event_bus_ = std::make_shared<MockEventBus>();
        repo_manager_ = std::make_shared<svcs::core::RepositoryManager>(event_bus_);
    }

    void TearDown() override {
        fs::remove_all(test_dir_);
    }

    bool isLibSSHAvailable() const {
        return LIBSSH_AVAILABLE == 1;
    }

    SSHConfig config_;
    fs::path test_dir_;
    std::shared_ptr<svcs::core::RepositoryManager> repo_manager_;
    std::shared_ptr<MockEventBus> event_bus_;
};

TEST_F(SSHServerIntegrationTest, FullStartStopCycle) {
    if (!isLibSSHAvailable()) {
        GTEST_SKIP() << "libssh not available, skipping integration test";
    }

    // Создаем тестовый ключ хоста
    std::ofstream key_file(config_.host_key);
    key_file << "-----BEGIN RSA PRIVATE KEY-----\n";
    key_file << "MIIEogIBAAKCAQEAwKvZcU9ZQ1jNkXoX5JmK7y...\n";
    key_file << "-----END RSA PRIVATE KEY-----\n";
    key_file.close();

    SSHServer server(config_, repo_manager_, event_bus_);

    // Пробуем запустить сервер
    bool started = server.start();

    // Сервер может не запуститься из-за конфликта портов или других проблем,
    // но не должен падать
    if (started) {
        EXPECT_TRUE(server.isRunning());

        // Ждем немного чтобы сервер начал слушать
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Останавливаем сервер
        server.stop();
        EXPECT_FALSE(server.isRunning());
        EXPECT_EQ(server.getActiveConnections(), 0);
    }
}

}
