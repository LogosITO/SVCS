/**
 * @file HistoryCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Integration tests for the HistoryCommand class, covering history display,
 * options (--oneline, --last N, --full), and argument validation.
 *
 * @russian
 * @brief Интеграционные тесты для класса HistoryCommand, охватывающие отображение истории,
 * опции (--oneline, --last N, --full) и валидацию аргументов.
 */

#include "utils/IntRemoveCommandTest.hxx"

#include <memory>

// Test removing single file from staging
TEST_F(RemoveCommandTest, RemoveSingleFile) {
    // Stage files first
    stageFiles({"file1.txt", "file2.txt"});
    
    // Remove one file
    std::vector<std::string> args = {"file1.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Check that file1 was removed but file2 remains
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_EQ(stagedFiles.size(), 1);
    EXPECT_EQ(stagedFiles[0], "file2.txt");
    
    // Verify notifications
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Removed from staging: file1.txt"));
}

// Test removing multiple files from staging
TEST_F(RemoveCommandTest, RemoveMultipleFiles) {
    // Stage all files
    stageFiles({"file1.txt", "file2.txt", "file3.txt"});
    
    // Remove two files
    std::vector<std::string> args = {"file1.txt", "file3.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Check that only file2 remains
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_EQ(stagedFiles.size(), 1);
    EXPECT_EQ(stagedFiles[0], "file2.txt");
    
    // Verify notifications
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Removed from staging: file1.txt"));
    EXPECT_TRUE(containsMessage(notifications, "Removed from staging: file3.txt"));
}

// Test removing all files with --all flag (with simulated confirmation)
TEST_F(RemoveCommandTest, RemoveAllFilesWithConfirmation) {
    // Stage multiple files
    stageFiles({"file1.txt", "file2.txt", "file3.txt"});
    
    // Simulate user confirmation "yes"
    std::istringstream inputStream("y\n");
    std::cin.rdbuf(inputStream.rdbuf());
    
    std::vector<std::string> args = {"--all"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Check that all files were removed
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_TRUE(stagedFiles.empty());
    
    // Verify notifications
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Removed all files from staging area"));
}

// Test removing all files with --all flag (with simulated rejection)
TEST_F(RemoveCommandTest, RemoveAllFilesWithRejection) {
    // Stage multiple files
    stageFiles({"file1.txt", "file2.txt", "file3.txt"});
    
    // Simulate user rejection "no"
    std::istringstream inputStream("n\n");
    std::cin.rdbuf(inputStream.rdbuf());
    
    std::vector<std::string> args = {"--all"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result); // Should return false when cancelled
    
    // Check that files remain staged
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_EQ(stagedFiles.size(), 3);
    
    // Verify cancellation message
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Remove operation cancelled"));
}

// Test removing all files with legacy dot notation
TEST_F(RemoveCommandTest, RemoveAllFilesWithDot) {
    // Stage multiple files
    stageFiles({"file1.txt", "file2.txt", "file3.txt"});
    
    std::vector<std::string> args = {"."};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Check that all files were removed
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_TRUE(stagedFiles.empty());
    
    // Verify notifications
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Removed all files from staging area"));
}

// Test removing all files with --all --force flags
TEST_F(RemoveCommandTest, RemoveAllFilesWithForce) {
    // Stage multiple files
    stageFiles({"file1.txt", "file2.txt", "file3.txt"});
    
    std::vector<std::string> args = {"--all", "--force"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Check that all files were removed
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_TRUE(stagedFiles.empty());
    
    // Verify notifications
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Removed all files from staging area"));
}

// Test behavior when no files are staged
TEST_F(RemoveCommandTest, RemoveWhenNoFilesStaged) {
    // Don't stage any files
    
    std::vector<std::string> args = {"file1.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Verify info message
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "No files in staging area"));
}

// Test help command
TEST_F(RemoveCommandTest, ShowHelp) {
    std::vector<std::string> args = {"--help"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Verify help messages were shown
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Usage:"));
    EXPECT_TRUE(containsMessage(notifications, "Description:"));
    EXPECT_TRUE(containsMessage(notifications, "Options:"));
}

// Test error when no repository initialized
TEST_F(RemoveCommandTest, ErrorWhenNoRepository) {
    // Change to directory without repository
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    
    std::vector<std::string> args = {"file1.txt"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    // Verify error message
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Not a SVCS repository"));
}

// Test error when no arguments provided
TEST_F(RemoveCommandTest, ErrorWhenNoArguments) {
    std::vector<std::string> args;
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    // Verify error message
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "No files specified"));
}

// Test removing files from subdirectory
TEST_F(RemoveCommandTest, RemoveFileFromSubdirectory) {
    // Stage files including from subdirectory
    stageFiles({"file1.txt", "subdir/file4.txt"});
    
    // Remove file from subdirectory
    std::vector<std::string> args = {"subdir/file4.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Check that only file1 remains
    auto stagedFiles = repoManager->getStagedFiles();
    EXPECT_EQ(stagedFiles.size(), 1);
    EXPECT_EQ(stagedFiles[0], "file1.txt");
    
    // Verify notifications
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Removed from staging: subdir/file4.txt"));
}