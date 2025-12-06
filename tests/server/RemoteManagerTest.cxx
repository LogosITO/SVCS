/**
 * @file RemoteManagerTest.cpp
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Unit and integration tests for the RemoteManager class.
 * @details This file contains tests for the @ref RemoteManager class,
 * utilizing the Google Test (gtest) framework.
 *
 * The tests cover the entire public API:
 * - `addRemote` (successful addition, handling duplicates)
 * - `removeRemote` (successful removal, removing non-existent remote)
 * - `getRemoteUrl` (successful retrieval, retrieving non-existent remote)
 * - `listRemotes` (list verification)
 * - `save`/`load` (persistence testing)
 *
 * The `RemoteManagerTest` test fixture creates a temporary
 * directory (`fs::temp_directory_path() / "remote_manager_test"`)
 * to verify file I/O operations (saving and loading the 'remotes' file).
 *
 * @note These tests perform real I/O operations with the filesystem.
 *
 * @russian
 * @brief Модульные и интеграционные тесты для класса RemoteManager.
 * @details Этот файл содержит тесты для класса @ref RemoteManager,
 * использующие фреймворк Google Test (gtest).
 *
 * Тесты охватывают весь публичный API:
 * - `addRemote` (успешное добавление, обработка дубликатов)
 * - `removeRemote` (успешное удаление, удаление несуществующего удаленного репозитория)
 * - `getRemoteUrl` (успешное получение, получение несуществующего удаленного репозитория)
 * - `listRemotes` (проверка списка)
 * - `save`/`load` (тестирование сохранения состояния)
 *
 * Тестовый фикстур `RemoteManagerTest` создает временную
 * директорию (`fs::temp_directory_path() / "remote_manager_test"`)
 * для проверки операций ввода-вывода файлов (сохранение и загрузка файла 'remotes').
 *
 * @note Эти тесты выполняют реальные операции ввода-вывода с файловой системой.
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <memory>

namespace fs = std::filesystem;

#include "../../server/include/RemoteManager.hxx"

namespace svcs::test::server {

class RemoteManagerTest : public ::testing::Test {
protected:
    fs::path temp_dir;

    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "remote_manager_test";
        fs::remove_all(temp_dir);
        fs::create_directories(temp_dir);
    }

    void TearDown() override {
        fs::remove_all(temp_dir);
    }
};

TEST_F(RemoteManagerTest, ConstructorCreatesEmptyManager) {
    svcs::server::RemoteManager rm(temp_dir);
    EXPECT_EQ(rm.count(), 0);
}

TEST_F(RemoteManagerTest, AddRemoteSuccess) {
    svcs::server::RemoteManager rm(temp_dir);
    EXPECT_TRUE(rm.addRemote("origin", "user@server.com:/repo.git"));
    EXPECT_EQ(rm.count(), 1);
    EXPECT_TRUE(rm.hasRemote("origin"));
}

TEST_F(RemoteManagerTest, AddDuplicateRemoteFails) {
    svcs::server::RemoteManager rm(temp_dir);
    EXPECT_TRUE(rm.addRemote("origin", "user@server.com:/repo.git"));
    EXPECT_FALSE(rm.addRemote("origin", "user@server.com:/another.git"));
    EXPECT_EQ(rm.count(), 1);
}

TEST_F(RemoteManagerTest, RemoveRemoteSuccess) {
    svcs::server::RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");
    EXPECT_TRUE(rm.removeRemote("origin"));
    EXPECT_EQ(rm.count(), 0);
    EXPECT_FALSE(rm.hasRemote("origin"));
}

TEST_F(RemoteManagerTest, RemoveNonExistentRemoteFails) {
    svcs::server::RemoteManager rm(temp_dir);
    EXPECT_FALSE(rm.removeRemote("nonexistent"));
}

TEST_F(RemoteManagerTest, GetRemoteUrl) {
    svcs::server::RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");
    EXPECT_EQ(rm.getRemoteUrl("origin"), "user@server.com:/repo.git");
}

TEST_F(RemoteManagerTest, GetNonExistentRemoteUrl) {
    svcs::server::RemoteManager rm(temp_dir);
    EXPECT_TRUE(rm.getRemoteUrl("nonexistent").empty());
}

TEST_F(RemoteManagerTest, ListRemotes) {
    svcs::server::RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");
    rm.addRemote("upstream", "user@server.com:/upstream.git");

    auto remotes = rm.listRemotes();
    EXPECT_EQ(remotes.size(), 2);
    EXPECT_NE(std::find(remotes.begin(), remotes.end(), "origin"), remotes.end());
    EXPECT_NE(std::find(remotes.begin(), remotes.end(), "upstream"), remotes.end());
}

TEST_F(RemoteManagerTest, SaveAndLoadRemotes) {
    {
        svcs::server::RemoteManager rm(temp_dir);
        rm.addRemote("origin", "user@server.com:/repo.git");
        rm.addRemote("backup", "user@backup.com:/backup.git");
    }

    svcs::server::RemoteManager rm2(temp_dir);
    EXPECT_EQ(rm2.count(), 2);
    EXPECT_TRUE(rm2.hasRemote("origin"));
    EXPECT_TRUE(rm2.hasRemote("backup"));
    EXPECT_EQ(rm2.getRemoteUrl("origin"), "user@server.com:/repo.git");
    EXPECT_EQ(rm2.getRemoteUrl("backup"), "user@backup.com:/backup.git");
}

TEST_F(RemoteManagerTest, SaveCreatesConfigFile) {
    svcs::server::RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");

    EXPECT_TRUE(fs::exists(temp_dir / "remotes"));

    std::ifstream file(temp_dir / "remotes");
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    EXPECT_TRUE(content.find("[remote \"origin\"]") != std::string::npos);
    EXPECT_TRUE(content.find("url = user@server.com:/repo.git") != std::string::npos);
}

}