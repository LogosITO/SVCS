/**
 * @file RemoteManagerTest.cpp
 * @brief Unit and integration tests for the RemoteManager class.
 *
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
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "../../server/include/RemoteManager.hxx"

namespace fs = std::filesystem;

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
    RemoteManager rm(temp_dir);
    EXPECT_EQ(rm.count(), 0);
}

TEST_F(RemoteManagerTest, AddRemoteSuccess) {
    RemoteManager rm(temp_dir);
    EXPECT_TRUE(rm.addRemote("origin", "user@server.com:/repo.git"));
    EXPECT_EQ(rm.count(), 1);
    EXPECT_TRUE(rm.hasRemote("origin"));
}

TEST_F(RemoteManagerTest, AddDuplicateRemoteFails) {
    RemoteManager rm(temp_dir);
    EXPECT_TRUE(rm.addRemote("origin", "user@server.com:/repo.git"));
    EXPECT_FALSE(rm.addRemote("origin", "user@server.com:/another.git"));
    EXPECT_EQ(rm.count(), 1);
}

TEST_F(RemoteManagerTest, RemoveRemoteSuccess) {
    RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");
    EXPECT_TRUE(rm.removeRemote("origin"));
    EXPECT_EQ(rm.count(), 0);
    EXPECT_FALSE(rm.hasRemote("origin"));
}

TEST_F(RemoteManagerTest, RemoveNonExistentRemoteFails) {
    RemoteManager rm(temp_dir);
    EXPECT_FALSE(rm.removeRemote("nonexistent"));
}

TEST_F(RemoteManagerTest, GetRemoteUrl) {
    RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");
    EXPECT_EQ(rm.getRemoteUrl("origin"), "user@server.com:/repo.git");
}

TEST_F(RemoteManagerTest, GetNonExistentRemoteUrl) {
    RemoteManager rm(temp_dir);
    EXPECT_TRUE(rm.getRemoteUrl("nonexistent").empty());
}

TEST_F(RemoteManagerTest, ListRemotes) {
    RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");
    rm.addRemote("upstream", "user@server.com:/upstream.git");

    auto remotes = rm.listRemotes();
    EXPECT_EQ(remotes.size(), 2);
    EXPECT_NE(std::find(remotes.begin(), remotes.end(), "origin"), remotes.end());
    EXPECT_NE(std::find(remotes.begin(), remotes.end(), "upstream"), remotes.end());
}

TEST_F(RemoteManagerTest, SaveAndLoadRemotes) {
    {
        RemoteManager rm(temp_dir);
        rm.addRemote("origin", "user@server.com:/repo.git");
        rm.addRemote("backup", "user@backup.com:/backup.git");
    }

    RemoteManager rm2(temp_dir);
    EXPECT_EQ(rm2.count(), 2);
    EXPECT_TRUE(rm2.hasRemote("origin"));
    EXPECT_TRUE(rm2.hasRemote("backup"));
    EXPECT_EQ(rm2.getRemoteUrl("origin"), "user@server.com:/repo.git");
    EXPECT_EQ(rm2.getRemoteUrl("backup"), "user@backup.com:/backup.git");
}

TEST_F(RemoteManagerTest, SaveCreatesConfigFile) {
    RemoteManager rm(temp_dir);
    rm.addRemote("origin", "user@server.com:/repo.git");

    EXPECT_TRUE(fs::exists(temp_dir / "remotes"));

    std::ifstream file(temp_dir / "remotes");
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    EXPECT_TRUE(content.find("[remote \"origin\"]") != std::string::npos);
    EXPECT_TRUE(content.find("url = user@server.com:/repo.git") != std::string::npos);
}