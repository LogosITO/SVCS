/**
 * @file BranchManagerTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of unit tests for the BranchManager class.
 *
 * @russian
 * @brief Реализация модульных тестов для класса BranchManager.
 */

#include "../../core/include/BranchManager.hxx"
#include "utils/MockSubject.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace svcs::test::core {

class BranchManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = std::filesystem::temp_directory_path() / "svcs_branch_test";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        std::filesystem::current_path(testDir);

        std::filesystem::create_directories(".svcs/refs/heads");
        createTestHeadFile("main");

        mockEventBus = std::make_shared<mocks::MockSubject>();
        branchManager = std::make_unique<svcs::core::BranchManager>(mockEventBus);
    }

    void TearDown() override {
        std::filesystem::current_path(std::filesystem::temp_directory_path());
        std::filesystem::remove_all(testDir);
    }

    static void createTestHeadFile(const std::string& branch_name) {
        std::ofstream head_file(".svcs/HEAD");
        head_file << "ref: refs/heads/" << branch_name;
        head_file.close();
    }

    static void createTestBranchFile(const std::string& branch_name, const std::string& commit_hash) {
        std::string branch_file = ".svcs/refs/heads/" + branch_name;
        std::ofstream file(branch_file);
        file << commit_hash;
        file.close();
    }

    static std::string readHeadFile() {
        std::ifstream head_file(".svcs/HEAD");
        std::string content;
        std::getline(head_file, content);
        return content;
    }

    static std::string readBranchFile(const std::string& branch_name) {
        std::string branch_file = ".svcs/refs/heads/" + branch_name;
        if (!std::filesystem::exists(branch_file)) {
            return "";
        }
        std::ifstream file(branch_file);
        std::string content;
        std::getline(file, content);
        return content;
    }

    std::filesystem::path testDir;
    std::shared_ptr<mocks::MockSubject> mockEventBus;
    std::unique_ptr<svcs::core::BranchManager> branchManager;
};

TEST_F(BranchManagerTest, CreateDuplicateBranch) {
    createTestBranchFile("main", "main_commit");
    branchManager->createBranchFromCommit("feature/test", "abc123");
    bool result = branchManager->createBranchFromCommit("feature/test", "def456");

    EXPECT_FALSE(result);
}

TEST_F(BranchManagerTest, CreateBranchWithInvalidName) {
    createTestBranchFile("main", "main_commit");
    EXPECT_FALSE(branchManager->createBranchFromCommit("", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("feature~test", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("feature/", "abc123"));
}

TEST_F(BranchManagerTest, DeleteNonExistentBranch) {
    bool result = branchManager->deleteBranch("nonexistent");
    EXPECT_FALSE(result);
}

TEST_F(BranchManagerTest, RenameBranch) {
    createTestBranchFile("main", "main_commit");
    branchManager->createBranchFromCommit("old-name", "abc123");

    bool result = branchManager->renameBranch("old-name", "new-name");

    EXPECT_TRUE(result);
    EXPECT_FALSE(branchManager->branchExists("old-name"));
    EXPECT_TRUE(branchManager->branchExists("new-name"));
    EXPECT_EQ(branchManager->getBranchHead("new-name"), "abc123");
}

TEST_F(BranchManagerTest, RenameToExistingBranch) {
    createTestBranchFile("main", "main_commit");
    branchManager->createBranchFromCommit("branch1", "abc123");
    branchManager->createBranchFromCommit("branch2", "def456");

    bool result = branchManager->renameBranch("branch1", "branch2");
    EXPECT_FALSE(result);
    EXPECT_TRUE(branchManager->branchExists("branch1"));
    EXPECT_TRUE(branchManager->branchExists("branch2"));
}

TEST_F(BranchManagerTest, SwitchToNonExistentBranch) {
    bool result = branchManager->switchBranch("nonexistent");
    EXPECT_FALSE(result);
    EXPECT_EQ(branchManager->getCurrentBranch(), "main");
}

TEST_F(BranchManagerTest, BranchNameValidation) {
    EXPECT_TRUE(svcs::core::BranchManager::isValidBranchName("main"));
    EXPECT_TRUE(svcs::core::BranchManager::isValidBranchName("develop"));
    EXPECT_TRUE(svcs::core::BranchManager::isValidBranchName("feature/new-feature"));
    EXPECT_TRUE(svcs::core::BranchManager::isValidBranchName("bugfix/issue-123"));

    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName(""));
    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName("feature~test"));
    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName("feature^test"));
    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName("feature:test"));
    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName("feature/"));
    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName("."));
    EXPECT_FALSE(svcs::core::BranchManager::isValidBranchName(".."));
}

TEST_F(BranchManagerTest, GetBranchHeadForNonExistentBranch) {
    std::string head = branchManager->getBranchHead("nonexistent");
    EXPECT_TRUE(head.empty());
}

}