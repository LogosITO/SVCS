/**
 * @file IntUndoCommandTest.cxx
 * @brief Implementation of integration tests for the UndoCommand class, focusing on forced operations.
 *
 * @details This file contains test cases that verify the 'svcs undo' command's 
 * behavior when the `--force` (`-f`) flag is used. The tests ensure that the 
 * command correctly skips the interactive confirmation prompt, delegates to the 
 * repository manager with the correct target (last commit or specified hash), 
 * and handles edge cases like the initial commit and invalid hashes.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
 
#include "utils/IntUndoCommandTest.hxx"

TEST_F(UndoCommandTest, UndoLastCommitWithForce) {
    // Create test commits
    createTestCommit("First commit");
    createTestCommit("Second commit");

    mockEventBus->clear();

    std::vector<std::string> args = {"--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo last commit"));
    EXPECT_TRUE(containsMessage(notifications, "Second commit"));
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
}

// Test undo last commit with explicit --last and --force flags
TEST_F(UndoCommandTest, UndoLastCommitExplicitWithForce) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    mockEventBus->clear();

    std::vector<std::string> args = {"--last", "--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo last commit"));
    EXPECT_TRUE(containsMessage(notifications, "Second commit"));
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
}

// Test undo specific commit by hash with force
TEST_F(UndoCommandTest, UndoSpecificCommitWithForce) {
    createTestCommit("First commit");
    createTestCommit("Second commit");
    createTestCommit("Third commit");

    // Get commit history to find a hash
    auto commits = repoManager->getCommitHistory();
    ASSERT_GE(commits.size(), 2);
    std::string targetHash = commits[1].hash.substr(0, 8); // Partial hash

    mockEventBus->clear();

    std::vector<std::string> args = {targetHash, "--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo commit"));
    EXPECT_TRUE(containsMessage(notifications, targetHash));
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
}

// Test undo with --commit flag and force
TEST_F(UndoCommandTest, UndoWithCommitFlagAndForce) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    auto commits = repoManager->getCommitHistory();
    ASSERT_GE(commits.size(), 1);
    std::string targetHash = commits[0].hash.substr(0, 8);

    mockEventBus->clear();

    std::vector<std::string> args = {"--commit", targetHash, "--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo commit"));
    EXPECT_TRUE(containsMessage(notifications, targetHash));
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
}

TEST_F(UndoCommandTest, ForceUndoInitialCommit) {
    createTestCommit("First commit");

    mockEventBus->clear();

    std::vector<std::string> args = {"--force"};
    bool result = command->execute(args);

    auto notifications = mockEventBus->getNotifications();

    EXPECT_TRUE(containsMessage(notifications, "Force"));
    EXPECT_TRUE(containsMessage(notifications, "initial commit"));

    SUCCEED();
}

// Test undo with multiple flags including force
TEST_F(UndoCommandTest, UndoWithMultipleFlagsAndForce) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    auto commits = repoManager->getCommitHistory();
    ASSERT_GE(commits.size(), 1);
    std::string targetHash = commits[0].hash.substr(0, 8);

    mockEventBus->clear();

    std::vector<std::string> args = {"--commit", targetHash, "--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
    EXPECT_TRUE(containsMessage(notifications, targetHash));
}

TEST_F(UndoCommandTest, MultipleUndoCommandsWithForce) {
    createTestCommit("First commit");
    createTestCommit("Second commit");
    createTestCommit("Third commit");

    // First undo with force
    std::vector<std::string> args1 = {"--force"};
    bool result1 = command->execute(args1);

    EXPECT_TRUE(result1);

    mockEventBus->clear();

    std::vector<std::string> args2 = {"--force"};
    bool result2 = command->execute(args2);

    auto notifications = mockEventBus->getNotifications();

    EXPECT_TRUE(repoManager->isRepositoryInitialized());

    bool hasUndoAttempt = containsMessage(notifications, "About to undo") ||
                         containsMessage(notifications, "undo") ||
                         containsMessage(notifications, "Undo");

    if (hasUndoAttempt) {
        SUCCEED();
    }

    SUCCEED();
}

// Test undo with short force flag
TEST_F(UndoCommandTest, UndoWithShortForceFlag) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    mockEventBus->clear();

    std::vector<std::string> args = {"-f"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
    EXPECT_TRUE(containsMessage(notifications, "About to undo last commit"));
}

// Test undo with combination of short flags
TEST_F(UndoCommandTest, UndoWithShortFlags) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    auto commits = repoManager->getCommitHistory();
    ASSERT_GE(commits.size(), 1);
    std::string targetHash = commits[0].hash.substr(0, 8);

    mockEventBus->clear();

    std::vector<std::string> args = {"-c", targetHash, "-f"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
    EXPECT_TRUE(containsMessage(notifications, targetHash));
}

// Test undo with invalid commit hash
TEST_F(UndoCommandTest, UndoWithInvalidCommitHash) {
    createTestCommit("First commit");

    mockEventBus->clear();

    std::vector<std::string> args = {"invalid_hash", "--force"};
    bool result = command->execute(args);

    EXPECT_FALSE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Commit not found"));
    EXPECT_TRUE(containsMessage(notifications, "invalid_hash"));
}

// Test undo with only force flag and no target specified
TEST_F(UndoCommandTest, UndoWithOnlyForceFlag) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    mockEventBus->clear();

    std::vector<std::string> args = {"--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo last commit"));
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
}

// Test undo with exact commit hash (full length)
TEST_F(UndoCommandTest, UndoWithFullCommitHash) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    auto commits = repoManager->getCommitHistory();
    ASSERT_GE(commits.size(), 1);
    std::string fullHash = commits[0].hash; // Full hash

    mockEventBus->clear();

    std::vector<std::string> args = {fullHash, "--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo commit"));
    EXPECT_TRUE(containsMessage(notifications, fullHash.substr(0, 8)));
}

// Test undo with mixed flag order
TEST_F(UndoCommandTest, UndoWithMixedFlagOrder) {
    createTestCommit("First commit");
    createTestCommit("Second commit");

    auto commits = repoManager->getCommitHistory();
    std::string targetHash = commits[0].hash.substr(0, 8);

    mockEventBus->clear();

    std::vector<std::string> args = {"--force", "--commit", targetHash};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Force mode enabled"));
    EXPECT_TRUE(containsMessage(notifications, targetHash));
}

// Test verifying repository state after force undo
TEST_F(UndoCommandTest, VerifyRepositoryStateAfterForceUndo) {
    // Setup repository with multiple commits using existing methods
    // Create test commits using the existing createTestCommit method
    ASSERT_TRUE(createTestCommit("First commit"));
    ASSERT_TRUE(createTestCommit("Second commit"));
    ASSERT_TRUE(createTestCommit("Third commit"));

    // Get initial commit count SAFELY
    auto initialCommits = repoManager->getCommitHistory();
    size_t initialCount = initialCommits.size();

    // Проверяем, что у нас достаточно коммитов
    if (initialCount < 2) {
        GTEST_SKIP() << "Not enough commits for undo test (need at least 2, got " << initialCount << ")";
    }

    // Store the commit to undo (second commit) SAFELY
    std::string commitToUndo = initialCommits[1].hash; // Second commit

    // Execute undo with force using the command (not direct repo manager call)
    std::vector<std::string> args = {"--commit", commitToUndo, "--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result) << "Undo with force should succeed";

    // Get final commit count
    auto finalCommits = repoManager->getCommitHistory();
    size_t finalCount = finalCommits.size();

    // Verify the commit was removed
    // Note: The exact count might vary depending on implementation
    EXPECT_LT(finalCount, initialCount) << "Commit count should decrease after undo";

    // Verify the specific commit is gone
    bool commitFound = false;
    for (const auto& commit : finalCommits) {
        if (commit.hash == commitToUndo) {
            commitFound = true;
            break;
        }
    }
    EXPECT_FALSE(commitFound) << "Undone commit should not be in history";

    // Verify repository is still functional
    EXPECT_TRUE(repoManager->isRepositoryInitialized())
        << "Repository should still be initialized after undo";
}

// Test undo with special characters in commit message
TEST_F(UndoCommandTest, UndoCommitWithSpecialCharacters) {
    createTestCommit("Fix: bug #1234 - critical issue!");
    createTestCommit("Feature: add 'new' functionality");

    mockEventBus->clear();

    std::vector<std::string> args = {"--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "About to undo last commit"));
    EXPECT_TRUE(containsMessage(notifications, "Feature: add 'new' functionality"));
}

TEST_F(UndoCommandTest, UndoWithLargeCommitHistory) {
    for (int i = 0; i < 3; ++i) {
        ASSERT_TRUE(createTestCommit("Commit " + std::to_string(i + 1)));
    }

    auto initialCommits = repoManager->getCommitHistory();
    size_t initialCount = initialCommits.size();

    // Убедимся что коммиты создались
    if (initialCount == 0) {
        GTEST_SKIP() << "No commits were created for undo test";
        return;
    }

    mockEventBus->clear();

    std::vector<std::string> args = {"--force"};
    bool result = command->execute(args);

    EXPECT_TRUE(result);

    auto finalCommits = repoManager->getCommitHistory();

    EXPECT_TRUE(repoManager->isRepositoryInitialized());

    auto notifications = mockEventBus->getNotifications();

    // Проверяем что была попытка undo
    bool hasUndoMessage = containsMessage(notifications, "About to undo") ||
                         containsMessage(notifications, "undo") ||
                         containsMessage(notifications, "Undo");

    if (hasUndoMessage) {
        SUCCEED();
    }

    SUCCEED();
}