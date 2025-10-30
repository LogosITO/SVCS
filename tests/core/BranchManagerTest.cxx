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
#include "../cli/mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

class BranchManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary test directory
        testDir = std::filesystem::temp_directory_path() / "svcs_branch_test";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        std::filesystem::current_path(testDir);
        
        // Initialize .svcs directory structure
        std::filesystem::create_directories(".svcs/refs/heads");
        
        // Create initial HEAD file pointing to main
        createTestHeadFile("main");

        mockEventBus = std::make_shared<MockSubject>();
        branchManager = std::make_unique<BranchManager>(mockEventBus);
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

    static void createTestBranchesFile(const std::vector<std::pair<std::string, std::string>>& branches) {
        // Create individual branch files (new format)
        for (const auto& [name, commit] : branches) {
            createTestBranchFile(name, commit);
        }
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
    std::shared_ptr<MockSubject> mockEventBus;
    std::unique_ptr<BranchManager> branchManager;
};

// Test creating duplicate branch
TEST_F(BranchManagerTest, CreateDuplicateBranch) {
    createTestBranchFile("main", "main_commit");
    branchManager->createBranchFromCommit("feature/test", "abc123");
    bool result = branchManager->createBranchFromCommit("feature/test", "def456");

    EXPECT_FALSE(result);
}

// Test creating branch with invalid name
TEST_F(BranchManagerTest, CreateBranchWithInvalidName) {
    createTestBranchFile("main", "main_commit");
    EXPECT_FALSE(branchManager->createBranchFromCommit("", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("feature~test", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("feature/", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit(".", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("..", "abc123"));
}

// Test deleting non-existent branch
TEST_F(BranchManagerTest, DeleteNonExistentBranch) {
    bool result = branchManager->deleteBranch("nonexistent");

    EXPECT_FALSE(result);
}

// Test branch renaming
TEST_F(BranchManagerTest, RenameBranch) {
    createTestBranchFile("main", "main_commit");
    branchManager->createBranchFromCommit("old-name", "abc123");

    bool result = branchManager->renameBranch("old-name", "new-name");

    EXPECT_TRUE(result);
    EXPECT_FALSE(branchManager->branchExists("old-name"));
    EXPECT_TRUE(branchManager->branchExists("new-name"));
    EXPECT_EQ(branchManager->getBranchHead("new-name"), "abc123");

    // Verify files were renamed
    EXPECT_FALSE(std::filesystem::exists(".svcs/refs/heads/old-name"));
    EXPECT_TRUE(std::filesystem::exists(".svcs/refs/heads/new-name"));
    EXPECT_EQ(readBranchFile("new-name"), "abc123");
}

// Test renaming to existing branch name
TEST_F(BranchManagerTest, RenameToExistingBranch) {
    createTestBranchFile("main", "main_commit");
    branchManager->createBranchFromCommit("branch1", "abc123");
    branchManager->createBranchFromCommit("branch2", "def456");

    bool result = branchManager->renameBranch("branch1", "branch2");

    EXPECT_FALSE(result);
    EXPECT_TRUE(branchManager->branchExists("branch1"));
    EXPECT_TRUE(branchManager->branchExists("branch2"));
}

// Test switching to non-existent branch
TEST_F(BranchManagerTest, SwitchToNonExistentBranch) {
    bool result = branchManager->switchBranch("nonexistent");

    EXPECT_FALSE(result);
    EXPECT_EQ(branchManager->getCurrentBranch(), "main"); // Should remain main
}

TEST_F(BranchManagerTest, BranchNameValidation) {
    EXPECT_TRUE(BranchManager::isValidBranchName("main"));
    EXPECT_TRUE(BranchManager::isValidBranchName("develop"));
    EXPECT_TRUE(BranchManager::isValidBranchName("feature/new-feature"));
    EXPECT_TRUE(BranchManager::isValidBranchName("bugfix/issue-123"));
    EXPECT_TRUE(BranchManager::isValidBranchName("release/v1.0.0"));

    EXPECT_FALSE(BranchManager::isValidBranchName(""));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature~test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature^test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature:test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature?test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature*test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature[test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature]test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature\\test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature/"));
    EXPECT_FALSE(BranchManager::isValidBranchName("feature//test"));
    EXPECT_FALSE(BranchManager::isValidBranchName("."));
    EXPECT_FALSE(BranchManager::isValidBranchName(".."));
}

// Test getBranchHead for non-existent branch
TEST_F(BranchManagerTest, GetBranchHeadForNonExistentBranch) {
    std::string head = branchManager->getBranchHead("nonexistent");
    
    EXPECT_TRUE(head.empty());
}