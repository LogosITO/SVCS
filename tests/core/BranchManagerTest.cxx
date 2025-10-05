/**
 * @file BranchManagerTest.cxx
 * @brief Implementation of unit tests for the BranchManager class.
 *
 * @details This file implements the Google Test fixture and individual test cases 
 * to verify the functionality of the BranchManager, including branch creation, 
 * deletion, renaming, switching, persistence (loading/saving state to the file 
 * system), and input validation.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
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
        
        mockEventBus = std::make_shared<MockSubject>();
        branchManager = std::make_unique<BranchManager>(mockEventBus);
    }
    
    void TearDown() override {
        std::filesystem::current_path(std::filesystem::temp_directory_path());
        std::filesystem::remove_all(testDir);
    }
    
    void createTestHeadFile(const std::string& branch_name) {
        std::ofstream head_file(".svcs/HEAD");
        head_file << "ref: refs/heads/" << branch_name;
        head_file.close();
    }
    
    void createTestBranchesFile(const std::vector<std::pair<std::string, std::string>>& branches) {
        std::ofstream branches_file(".svcs/refs/heads/branches");
        for (const auto& [name, commit] : branches) {
            branches_file << name << ":" << commit << "\n";
        }
        branches_file.close();
    }
    
    std::string readHeadFile() {
        std::ifstream head_file(".svcs/HEAD");
        std::string content;
        std::getline(head_file, content);
        return content;
    }
    
    std::vector<std::string> readBranchesFile() {
        std::vector<std::string> lines;
        std::ifstream branches_file(".svcs/refs/heads/branches");
        std::string line;
        while (std::getline(branches_file, line)) {
            lines.push_back(line);
        }
        return lines;
    }
    
    std::filesystem::path testDir;
    std::shared_ptr<MockSubject> mockEventBus;
    std::unique_ptr<BranchManager> branchManager;
};

// Test basic branch creation
TEST_F(BranchManagerTest, CreateValidBranch) {
    bool result = branchManager->createBranchFromCommit("feature/test", "abc123");
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(branchManager->branchExists("feature/test"));
    EXPECT_EQ(branchManager->getBranchHead("feature/test"), "abc123");
}

// Test creating duplicate branch
TEST_F(BranchManagerTest, CreateDuplicateBranch) {
    branchManager->createBranchFromCommit("feature/test", "abc123");
    bool result = branchManager->createBranchFromCommit("feature/test", "def456");
    
    EXPECT_FALSE(result);
}

// Test creating branch with invalid name
TEST_F(BranchManagerTest, CreateBranchWithInvalidName) {
    EXPECT_FALSE(branchManager->createBranchFromCommit("", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("feature~test", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("feature/", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit(".", "abc123"));
    EXPECT_FALSE(branchManager->createBranchFromCommit("..", "abc123"));
}

// Test branch deletion
TEST_F(BranchManagerTest, DeleteExistingBranch) {
    branchManager->createBranchFromCommit("feature/test", "abc123");
    branchManager->createBranchFromCommit("develop", "def456");
    
    bool result = branchManager->deleteBranch("feature/test");
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(branchManager->branchExists("feature/test"));
    EXPECT_TRUE(branchManager->branchExists("develop"));
}

// Test deleting non-existent branch
TEST_F(BranchManagerTest, DeleteNonExistentBranch) {
    bool result = branchManager->deleteBranch("nonexistent");
    
    EXPECT_FALSE(result);
}

// Test deleting current branch
TEST_F(BranchManagerTest, DeleteCurrentBranch) {
    branchManager->createBranchFromCommit("feature/test", "abc123");
    branchManager->switchBranch("feature/test");
    
    bool result = branchManager->deleteBranch("feature/test");
    
    EXPECT_FALSE(result);
    EXPECT_TRUE(branchManager->branchExists("feature/test"));
}

// Test branch renaming
TEST_F(BranchManagerTest, RenameBranch) {
    branchManager->createBranchFromCommit("old-name", "abc123");
    
    bool result = branchManager->renameBranch("old-name", "new-name");
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(branchManager->branchExists("old-name"));
    EXPECT_TRUE(branchManager->branchExists("new-name"));
    EXPECT_EQ(branchManager->getBranchHead("new-name"), "abc123");
}

// Test renaming to existing branch name
TEST_F(BranchManagerTest, RenameToExistingBranch) {
    branchManager->createBranchFromCommit("branch1", "abc123");
    branchManager->createBranchFromCommit("branch2", "def456");
    
    bool result = branchManager->renameBranch("branch1", "branch2");
    
    EXPECT_FALSE(result);
    EXPECT_TRUE(branchManager->branchExists("branch1"));
    EXPECT_TRUE(branchManager->branchExists("branch2"));
}

// Test switching branches
TEST_F(BranchManagerTest, SwitchBetweenBranches) {
    branchManager->createBranchFromCommit("develop", "abc123");
    branchManager->createBranchFromCommit("feature/test", "def456");
    
    bool result1 = branchManager->switchBranch("develop");
    bool result2 = branchManager->switchBranch("feature/test");
    
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_EQ(branchManager->getCurrentBranch(), "feature/test");
}

// Test switching to non-existent branch
TEST_F(BranchManagerTest, SwitchToNonExistentBranch) {
    bool result = branchManager->switchBranch("nonexistent");
    
    EXPECT_FALSE(result);
    EXPECT_EQ(branchManager->getCurrentBranch(), "main"); // Should remain main
}

// Test getting all branches
TEST_F(BranchManagerTest, GetAllBranches) {
    branchManager->createBranchFromCommit("develop", "abc123");
    branchManager->createBranchFromCommit("feature/test", "def456");
    branchManager->switchBranch("develop");
    
    auto branches = branchManager->getAllBranches();
    
    EXPECT_EQ(branches.size(), 3); // main, develop, feature/test
    EXPECT_TRUE(std::any_of(branches.begin(), branches.end(),
                           [](const auto& b) { return b.name == "main"; }));
    EXPECT_TRUE(std::any_of(branches.begin(), branches.end(),
                           [](const auto& b) { return b.name == "develop"; }));
    EXPECT_TRUE(std::any_of(branches.begin(), branches.end(),
                           [](const auto& b) { return b.name == "feature/test"; }));
    
    // Check that develop is marked as current
    auto develop_it = std::find_if(branches.begin(), branches.end(),
                                  [](const auto& b) { return b.name == "develop"; });
    EXPECT_TRUE(develop_it != branches.end());
    EXPECT_TRUE(develop_it->is_current);
}

// Test branch persistence
TEST_F(BranchManagerTest, BranchPersistence) {
    // Create branches with first instance
    branchManager->createBranchFromCommit("develop", "abc123");
    branchManager->createBranchFromCommit("feature/test", "def456");
    branchManager->switchBranch("develop");
    
    // Create new BranchManager instance (simulating restart)
    auto newBranchManager = std::make_unique<BranchManager>(mockEventBus);
    
    // Verify branches are loaded correctly
    EXPECT_TRUE(newBranchManager->branchExists("main"));
    EXPECT_TRUE(newBranchManager->branchExists("develop"));
    EXPECT_TRUE(newBranchManager->branchExists("feature/test"));
    EXPECT_EQ(newBranchManager->getCurrentBranch(), "develop");
    EXPECT_EQ(newBranchManager->getBranchHead("feature/test"), "def456");
}

// Test loading from existing branches file
TEST_F(BranchManagerTest, LoadFromExistingBranchesFile) {
    // Create branches file manually
    createTestBranchesFile({
        {"main", "abc123"},
        {"develop", "def456"},
        {"feature/test", "ghi789"}
    });
    createTestHeadFile("develop");
    
    // Create BranchManager - should load existing branches
    auto testBranchManager = std::make_unique<BranchManager>(mockEventBus);
    
    EXPECT_TRUE(testBranchManager->branchExists("main"));
    EXPECT_TRUE(testBranchManager->branchExists("develop"));
    EXPECT_TRUE(testBranchManager->branchExists("feature/test"));
    EXPECT_EQ(testBranchManager->getCurrentBranch(), "develop");
    EXPECT_EQ(testBranchManager->getBranchHead("feature/test"), "ghi789");
}

// Test branch name validation
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

// Test multiple operations sequence
TEST_F(BranchManagerTest, MultipleOperationsSequence) {
    // Create multiple branches
    EXPECT_TRUE(branchManager->createBranchFromCommit("develop", "commit1"));
    EXPECT_TRUE(branchManager->createBranchFromCommit("feature/auth", "commit2"));
    EXPECT_TRUE(branchManager->createBranchFromCommit("feature/ui", "commit3"));
    
    // Switch between them
    EXPECT_TRUE(branchManager->switchBranch("develop"));
    EXPECT_EQ(branchManager->getCurrentBranch(), "develop");
    
    EXPECT_TRUE(branchManager->switchBranch("feature/auth"));
    EXPECT_EQ(branchManager->getCurrentBranch(), "feature/auth");
    
    // Rename a branch
    EXPECT_TRUE(branchManager->renameBranch("feature/ui", "feature/new-ui"));
    
    // Delete a branch (но НЕ текущую!)
    EXPECT_TRUE(branchManager->switchBranch("develop")); // Переключаемся с feature/auth
    EXPECT_TRUE(branchManager->deleteBranch("feature/auth")); // Теперь можно удалить
    
    // Verify final state
    EXPECT_TRUE(branchManager->branchExists("main"));
    EXPECT_TRUE(branchManager->branchExists("develop"));
    EXPECT_TRUE(branchManager->branchExists("feature/new-ui"));
    EXPECT_FALSE(branchManager->branchExists("feature/ui"));
    EXPECT_FALSE(branchManager->branchExists("feature/auth"));
    EXPECT_EQ(branchManager->getCurrentBranch(), "develop");
}

// Test getBranchHead for non-existent branch
TEST_F(BranchManagerTest, GetBranchHeadForNonExistentBranch) {
    std::string head = branchManager->getBranchHead("nonexistent");
    
    EXPECT_TRUE(head.empty());
}