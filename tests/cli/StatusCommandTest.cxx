/**
 * @file StatusCommandTest.cxx
 * @brief Integration tests for the StatusCommand class.
 *
 * @details This file implements the test cases for the StatusCommandTest fixture. 
 * It verifies the command's ability to display the working tree status, covering 
 * the full repository status display and status checks for specific files. 
 * Tests include verifying correct reporting for staged, unstaged, and missing 
 * files, as well as handling the 'no repository' error condition.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "utils/IntStatusCommandTest.hxx"

// Test full status with no staged files
TEST_F(StatusCommandTest, FullStatusNoStagedFiles) {
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    // Should show repository status
    EXPECT_TRUE(containsMessage(notifications, "Repository status:"));
    EXPECT_TRUE(containsMessage(notifications, "On branch:"));
    EXPECT_TRUE(containsMessage(notifications, "No staged changes"));
    EXPECT_TRUE(containsMessage(notifications, "No unstaged changes"));
    EXPECT_TRUE(containsMessage(notifications, "Untracked files:"));
}

// Test full status with staged files
TEST_F(StatusCommandTest, FullStatusWithStagedFiles) {
    // Stage some files
    stageFiles({"file1.txt", "file2.txt"});
    
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    EXPECT_TRUE(containsMessage(notifications, "Repository status:"));
    EXPECT_TRUE(containsMessage(notifications, "Staged changes"));
    EXPECT_TRUE(containsMessage(notifications, "file1.txt"));
    EXPECT_TRUE(containsMessage(notifications, "file2.txt"));
}

// Test file status for specific files
TEST_F(StatusCommandTest, FileStatusSpecificFiles) {
    // Stage some files
    stageFiles({"file1.txt"});
    
    std::vector<std::string> args = {"file1.txt", "file2.txt", "file3.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    EXPECT_TRUE(containsMessage(notifications, "File status:"));
    EXPECT_TRUE(containsMessage(notifications, "file1.txt"));
    EXPECT_TRUE(containsMessage(notifications, "file2.txt")); 
    EXPECT_TRUE(containsMessage(notifications, "file3.txt"));
    EXPECT_TRUE(containsMessage(notifications, "Shown 3 file(s)"));
}

// Test file status with missing file
TEST_F(StatusCommandTest, FileStatusWithMissingFile) {
    std::vector<std::string> args = {"file1.txt", "nonexistent.txt", "file2.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    EXPECT_TRUE(containsMessage(notifications, "File status:"));
    EXPECT_TRUE(containsMessage(notifications, "file1.txt"));
    EXPECT_TRUE(containsMessage(notifications, "nonexistent.txt"));
    EXPECT_TRUE(containsMessage(notifications, "file2.txt"));
    EXPECT_TRUE(containsMessage(notifications, "missing"));
    EXPECT_TRUE(containsMessage(notifications, "Shown 2 file(s), 1 missing"));
}

// Test file status with only missing files
TEST_F(StatusCommandTest, FileStatusOnlyMissingFiles) {
    std::vector<std::string> args = {"ghost1.txt", "ghost2.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    EXPECT_TRUE(containsMessage(notifications, "File status:"));
    EXPECT_TRUE(containsMessage(notifications, "missing"));
    EXPECT_TRUE(containsMessage(notifications, "Shown 0 file(s), 2 missing"));
}

// Test error when no repository initialized
TEST_F(StatusCommandTest, ErrorWhenNoRepository) {
    // Change to directory without repository
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Not a SVCS repository"));
}

// Test help command
TEST_F(StatusCommandTest, ShowHelp) {
    std::vector<std::string> args = {"--help"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Usage: svcs status"));
    EXPECT_TRUE(containsMessage(notifications, "Show the working tree status"));
    EXPECT_TRUE(containsMessage(notifications, "Options:"));
}

// Test status with subdirectory files
TEST_F(StatusCommandTest, StatusWithSubdirectoryFiles) {
    std::vector<std::string> args = {"subdir/file4.txt"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "subdir/file4.txt"));
    EXPECT_TRUE(containsMessage(notifications, "Shown 1 file(s)"));
}

// Test empty status (no files specified for file status)
TEST_F(StatusCommandTest, EmptyFileStatus) {
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Should show full status, not file status
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Repository status:"));
    EXPECT_FALSE(containsMessage(notifications, "No files to display"));
}

// Test file status with empty file list (shouldn't happen but test robustness)
TEST_F(StatusCommandTest, FileStatusEmptyList) {
    // This tests the internal showFileStatus method with empty vector
    // In practice, execute() with empty args shows full status
    
    auto notifications = mockEventBus->getNotifications();
    // Just verify command executes without crashing
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    EXPECT_TRUE(result);
}

// Test status shows correct file states
TEST_F(StatusCommandTest, StatusShowsCorrectFileStates) {
    // Stage file1, modify file2, leave file3 untracked
    stageFiles({"file1.txt"});
    
    // Modify file2 by writing new content
    createTestFile("file2.txt", "modified content");
    
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    // Should show staged file
    EXPECT_TRUE(containsMessage(notifications, "[staged]"));
    EXPECT_TRUE(containsMessage(notifications, "file1.txt"));
    
    // Should show untracked files
    EXPECT_TRUE(containsMessage(notifications, "[untracked]"));
    
    // Should show usage instructions
    EXPECT_TRUE(containsMessage(notifications, "Use 'svcs add <file>' to stage changes"));
}