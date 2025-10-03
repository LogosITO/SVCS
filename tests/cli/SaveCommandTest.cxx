/**
 * @file SaveCommandTest.cxx
 * @brief Unit tests for SaveCommand class.
 */

#include "utils/IntSaveCommandTest.hxx"

 TEST_F(SaveCommandTest, SaveWithStagedChanges) {
    // Stage files first
    stageFiles({"file1.txt", "file2.txt"});
    
    // Execute save command with message
    std::vector<std::string> args = {"-m", "Test commit message"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    // Verify success messages
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Saving changes with message: Test commit message"));
    EXPECT_TRUE(containsMessage(notifications, "Changes saved successfully!"));
}

// Test save with --message flag
TEST_F(SaveCommandTest, SaveWithMessageFlag) {
    stageFiles({"file1.txt"});
    
    std::vector<std::string> args = {"--message", "Another test message"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Saving changes with message: Another test message"));
}

// Test error when no repository initialized
TEST_F(SaveCommandTest, ErrorWhenNoRepository) {
    // Change to directory without repository
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    
    std::vector<std::string> args = {"-m", "Test message"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Not a SVCS repository"));
}

// Test error when no message provided
TEST_F(SaveCommandTest, ErrorWhenNoMessage) {
    stageFiles({"file1.txt"});
    
    std::vector<std::string> args = {}; // No message argument
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Save message is required"));
}

// Test error when empty message provided
TEST_F(SaveCommandTest, ErrorWhenEmptyMessage) {
    stageFiles({"file1.txt"});
    
    std::vector<std::string> args = {"-m", ""};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Save message is required"));
}

// Test error when message is too short
TEST_F(SaveCommandTest, ErrorWhenMessageTooShort) {
    stageFiles({"file1.txt"});
    
    std::vector<std::string> args = {"-m", "a"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Save message is too short"));
}

// Test error when no staged changes
TEST_F(SaveCommandTest, ErrorWhenNoStagedChanges) {
    // Don't stage any files
    
    std::vector<std::string> args = {"-m", "Test message"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "No changes staged for save"));
}

// Test help command
TEST_F(SaveCommandTest, ShowHelp) {
    std::vector<std::string> args = {"--help"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    // Проверяем конкретные сообщения которые реально отправляются
    EXPECT_TRUE(containsMessage(notifications, "svcs save -m \"message\""));
    EXPECT_TRUE(containsMessage(notifications, "Save staged changes to the repository"));
    EXPECT_TRUE(containsMessage(notifications, "Creates a permanent snapshot"));
    EXPECT_TRUE(containsMessage(notifications, "-m, --message <msg>"));
}

// Test message parsing with different argument orders
TEST_F(SaveCommandTest, MessageParsingDifferentOrders) {
    stageFiles({"file1.txt"});
    
    // Test with -m flag
    std::vector<std::string> args1 = {"-m", "Message with -m"};
    bool result1 = command->execute(args1);
    EXPECT_TRUE(result1);
    
    // Проверяем уведомления от первой команды
    auto notifications1 = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications1, "Message with -m"));
}

// Separate test for --message flag
TEST_F(SaveCommandTest, SaveWithLongMessageFlag) {
    stageFiles({"file1.txt"});
    
    // Test with --message flag
    std::vector<std::string> args = {"--message", "Message with --message"};
    bool result = command->execute(args);
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Message with --message"));
}

// Test that staging area is cleared after successful save
TEST_F(SaveCommandTest, StagingAreaClearedAfterSave) {
    // Stage files
    stageFiles({"file1.txt", "file2.txt"});
    
    // Verify files are staged
    auto stagedBefore = repoManager->getStagedFiles();
    EXPECT_EQ(stagedBefore.size(), 2);
    
    // Execute save
    std::vector<std::string> args = {"-m", "Test clear staging"};
    bool result = command->execute(args);
    EXPECT_TRUE(result);
    
    // Verify staging area is cleared
    auto stagedAfter = repoManager->getStagedFiles();
    EXPECT_TRUE(stagedAfter.empty());
}

// Test save with special characters in message
TEST_F(SaveCommandTest, SaveWithSpecialCharactersInMessage) {
    stageFiles({"file1.txt"});
    
    std::vector<std::string> args = {"-m", "Message with spéciål chàräctêrs & symbols!"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Message with spéciål chàräctêrs & symbols!"));
}