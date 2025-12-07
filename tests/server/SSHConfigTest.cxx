/**
 * @file SSHConfig_test.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @brief Unit tests for SSHConfig class
 */

#include "../../server/include/SSHConfig.hxx"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>


/**
 * @brief SSH-specific test functionality
 * @details Test classes and utilities specific to SSH protocol implementation.
 * Focuses on session management, secure communication, and protocol compliance.
 *
 * @english
 * SSH-specific test functionality
 * @details Provides test infrastructure for SSH protocol features including
 * session establishment, key exchange, authentication, and channel management.
 * Ensures compliance with SSH protocol specifications and security requirements.
 *
 * @russian
 * SSH-специфичная тестовая функциональность
 * @details Предоставляет тестовую инфраструктуру для функций SSH протокола,
 * включая установку сессий, обмен ключами, аутентификацию и управление каналами.
 * Гарантирует соответствие спецификациям SSH протокола и требованиям безопасности.
 */

namespace svcs::test::server::ssh {

using namespace svcs::server::ssh;

namespace fs = std::filesystem;

class SSHConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_dir_ = fs::temp_directory_path() / "svcs_ssh_test";
        fs::create_directories(temp_dir_);
    }

    void TearDown() override {
        fs::remove_all(temp_dir_);
    }

    std::string createConfigFile(const std::string& content) {
        std::string path = (temp_dir_ / "test_config.conf").string();
        std::ofstream file(path);
        file << content;
        return path;
    }

    fs::path temp_dir_;
};

TEST_F(SSHConfigTest, LoadEmptyConfig) {
    std::string config_path = createConfigFile("");
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "0.0.0.0");
    EXPECT_EQ(config.port, 2222);
    EXPECT_EQ(config.keys_dir, "/etc/svcs/ssh/keys");
    EXPECT_EQ(config.passwd_dir, "/etc/svcs/ssh");
    EXPECT_EQ(config.max_connections, 256);
    EXPECT_EQ(config.timeout_sec, 300);
}

TEST_F(SSHConfigTest, LoadFullConfig) {
    std::string content = R"(
        # Test configuration
        host = 192.168.1.100
        port = 2223
        host_key = "/etc/svcs/ssh/host_key"
        auth_file = "/etc/svcs/ssh/auth.conf"
        keys_dir = "/var/lib/svcs/keys"
        passwd_dir = "/var/lib/svcs/passwd"
        max_connections = 512
        timeout_sec = 600
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "192.168.1.100");
    EXPECT_EQ(config.port, 2223);
    EXPECT_EQ(config.host_key, "/etc/svcs/ssh/host_key");
    EXPECT_EQ(config.auth_file, "/etc/svcs/ssh/auth.conf");
    EXPECT_EQ(config.keys_dir, "/var/lib/svcs/keys");
    EXPECT_EQ(config.passwd_dir, "/var/lib/svcs/passwd");
    EXPECT_EQ(config.max_connections, 512);
    EXPECT_EQ(config.timeout_sec, 600);
}

TEST_F(SSHConfigTest, LoadConfigWithQuotes) {
    std::string content = R"(
        host = "127.0.0.1"
        host_key = "/path/with spaces/key"
        keys_dir = "/path with/spaces"
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "127.0.0.1");
    EXPECT_EQ(config.host_key, "/path/with spaces/key");
    EXPECT_EQ(config.keys_dir, "/path with/spaces");
}

TEST_F(SSHConfigTest, LoadConfigWithCommentsAndSpaces) {
    std::string content = R"(
        # This is a comment
        host = 192.168.1.1  # inline comment

        port   =   2224    # port comment

        # Another comment
        max_connections = 128

        timeout_sec = 450
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "192.168.1.1");
    EXPECT_EQ(config.port, 2224);
    EXPECT_EQ(config.max_connections, 128);
    EXPECT_EQ(config.timeout_sec, 450);
}

TEST_F(SSHConfigTest, LoadInvalidConfigFile) {
    SSHConfig config = SSHConfig::load("/nonexistent/path/config.conf");

    EXPECT_EQ(config.host, "0.0.0.0");
    EXPECT_EQ(config.port, 2222);
}

TEST_F(SSHConfigTest, SaveConfig) {
    SSHConfig config;
    config.host = "10.0.0.1";
    config.port = 2225;
    config.host_key = "/test/host_key";
    config.auth_file = "/test/auth.conf";
    config.keys_dir = "/test/keys";
    config.passwd_dir = "/test/passwd";
    config.max_connections = 100;
    config.timeout_sec = 200;

    std::string config_path = (temp_dir_ / "output_config.conf").string();
    bool saved = config.save(config_path);

    EXPECT_TRUE(saved);
    EXPECT_TRUE(fs::exists(config_path));

    std::ifstream file(config_path);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("host = 10.0.0.1"), std::string::npos);
    EXPECT_NE(content.find("port = 2225"), std::string::npos);
    EXPECT_NE(content.find("host_key = \"/test/host_key\""), std::string::npos);
    EXPECT_NE(content.find("keys_dir = \"/test/keys\""), std::string::npos);
    EXPECT_NE(content.find("passwd_dir = \"/test/passwd\""), std::string::npos);
    EXPECT_NE(content.find("max_connections = 100"), std::string::npos);
    EXPECT_NE(content.find("timeout_sec = 200"), std::string::npos);
}

TEST_F(SSHConfigTest, ValidateValidConfig) {
    SSHConfig config;
    config.host = "192.168.1.1";
    config.port = 2222;
    config.host_key = "/etc/svcs/key";
    config.max_connections = 100;
    config.timeout_sec = 300;

    EXPECT_TRUE(config.validate());
}

TEST_F(SSHConfigTest, ValidateInvalidHost) {
    SSHConfig config;
    config.host = "";
    config.port = 2222;
    config.host_key = "/etc/svcs/key";
    config.max_connections = 100;
    config.timeout_sec = 300;

    EXPECT_FALSE(config.validate());
}

TEST_F(SSHConfigTest, ValidateInvalidPort) {
    SSHConfig config;
    config.host = "192.168.1.1";
    config.port = 0;
    config.host_key = "/etc/svcs/key";
    config.max_connections = 100;
    config.timeout_sec = 300;

    EXPECT_FALSE(config.validate());

    config.port = 65536;
    EXPECT_FALSE(config.validate());
}

TEST_F(SSHConfigTest, ValidateInvalidHostKey) {
    SSHConfig config;
    config.host = "192.168.1.1";
    config.port = 2222;
    config.host_key = "";
    config.max_connections = 100;
    config.timeout_sec = 300;

    EXPECT_FALSE(config.validate());
}

TEST_F(SSHConfigTest, ValidateInvalidMaxConnections) {
    SSHConfig config;
    config.host = "192.168.1.1";
    config.port = 2222;
    config.host_key = "/etc/svcs/key";
    config.max_connections = 0;
    config.timeout_sec = 300;

    EXPECT_FALSE(config.validate());
}

TEST_F(SSHConfigTest, ValidateInvalidTimeout) {
    SSHConfig config;
    config.host = "192.168.1.1";
    config.port = 2222;
    config.host_key = "/etc/svcs/key";
    config.max_connections = 100;
    config.timeout_sec = 0;

    EXPECT_FALSE(config.validate());

    config.timeout_sec = -1;
    EXPECT_FALSE(config.validate());
}

TEST_F(SSHConfigTest, LoadAndSaveRoundtrip) {
    SSHConfig original;
    original.host = "192.168.1.100";
    original.port = 2223;
    original.host_key = "/test/key";
    original.auth_file = "/test/auth";
    original.keys_dir = "/test/keys";
    original.passwd_dir = "/test/passwd";
    original.max_connections = 512;
    original.timeout_sec = 600;

    std::string config_path = (temp_dir_ / "roundtrip.conf").string();
    ASSERT_TRUE(original.save(config_path));

    SSHConfig loaded = SSHConfig::load(config_path);

    EXPECT_EQ(loaded.host, original.host);
    EXPECT_EQ(loaded.port, original.port);
    EXPECT_EQ(loaded.host_key, original.host_key);
    EXPECT_EQ(loaded.auth_file, original.auth_file);
    EXPECT_EQ(loaded.keys_dir, original.keys_dir);
    EXPECT_EQ(loaded.passwd_dir, original.passwd_dir);
    EXPECT_EQ(loaded.max_connections, original.max_connections);
    EXPECT_EQ(loaded.timeout_sec, original.timeout_sec);
}

TEST_F(SSHConfigTest, PartialConfig) {
    std::string content = R"(
        host = 10.0.0.1
        port = 2226
        # Остальные значения - по умолчанию
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "10.0.0.1");
    EXPECT_EQ(config.port, 2226);
    EXPECT_EQ(config.keys_dir, "/etc/svcs/ssh/keys");
    EXPECT_EQ(config.passwd_dir, "/etc/svcs/ssh");
    EXPECT_EQ(config.max_connections, 256);
    EXPECT_EQ(config.timeout_sec, 300);
}

TEST_F(SSHConfigTest, MalformedLines) {
    std::string content = R"(
        = missing_key
        key_without_value =
        line_without_equals
        valid = value
        = = =
        # Комментарий
        another_valid = another_value
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "0.0.0.0");
    SUCCEED();
}

TEST_F(SSHConfigTest, VeryLargePortNumber) {
    std::string content = R"(
        host = 192.168.1.1
        port = 99999  # Слишком большое число
    )";

    std::string config_path = createConfigFile(content);

    // Не должно падать, но порт будет неправильным
    EXPECT_NO_THROW({
        SSHConfig config = SSHConfig::load(config_path);
        // stoi преобразует, но validate() должна отловить
        EXPECT_FALSE(config.validate());
    });
}

TEST_F(SSHConfigTest, NegativeValues) {
    std::string content = R"(
        host = 192.168.1.1
        port = -1
        max_connections = -100
        timeout_sec = -50
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_FALSE(config.validate());
}

TEST_F(SSHConfigTest, ConfigWithSpecialCharacters) {
    std::string content = R"(
        host = 192.168.1.1
        host_key = "/path/with/underscores_and-dashes/key_rsa-2048"
        keys_dir = "/path/with/multiple/directories"
    )";

    std::string config_path = createConfigFile(content);
    SSHConfig config = SSHConfig::load(config_path);

    EXPECT_EQ(config.host, "192.168.1.1");
    EXPECT_EQ(config.host_key, "/path/with/underscores_and-dashes/key_rsa-2048");
    EXPECT_EQ(config.keys_dir, "/path/with/multiple/directories");
}

}