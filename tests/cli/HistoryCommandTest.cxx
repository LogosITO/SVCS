/**
 * @file HistoryCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Integration tests for the HistoryCommand class, covering core functionality,
 * format options, limit handling, and argument validation.
 * @details This file implements the test cases for the HistoryCommandTest fixture.
 * It verifies the command's behavior in various repository states and rigorously
 * checks argument parsing and error reporting for all supported flags.
 * Two tests include verbose debugging output to `std::cout` to assist in tracing
 * the internal state of the repository manager and the event notifications.
 *
 * @russian
 * @brief Интеграционные тесты для класса HistoryCommand, охватывающие основную функциональность,
 * параметры формата, обработку ограничений и валидацию аргументов.
 * @details Этот файл реализует тестовые случаи для фикстура HistoryCommandTest.
 * Он проверяет поведение команды в различных состояниях репозитория и тщательно
 * проверяет разбор аргументов и отчетность об ошибках для всех поддерживаемых флагов.
 * Два теста включают подробный отладочный вывод в `std::cout` для помощи в отслеживании
 * внутреннего состояния менеджера репозитория и уведомлений о событиях.
 */

#include "utils/IntHistoryCommandTest.hxx"

TEST_F(HistoryCommandTest, DebugCommitHistory) {
    // Create test commits
    createTestCommit("First commit");
    createTestCommit("Second commit");
    createTestCommit("Third commit");
    
    // Debug: check what getCommitHistory returns
    auto commits = repoManager->getCommitHistory();
    std::cout << "=== DEBUG: Commit history size: " << commits.size() << " ===" << std::endl;
    for (size_t i = 0; i < commits.size(); i++) {
        std::cout << "Commit " << i << ": " << commits[i].message << std::endl;
    }
    std::cout << "=== END DEBUG ===" << std::endl;
    
    mockEventBus->clear();
    
    std::vector<std::string> args = {"--last", "2"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    std::cout << "=== DEBUG: Notifications ===" << std::endl;
    for (const auto& event : notifications) {
        std::cout << "Type: " << static_cast<int>(event.type) 
                  << ", Details: " << event.details << std::endl;
    }
    std::cout << "=== END DEBUG ===" << std::endl;
}

TEST_F(HistoryCommandTest, HistoryNoCommits) {
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "No commits yet."));
}

// Test history with multiple commits
TEST_F(HistoryCommandTest, HistoryWithCommits) {
    // Create test commits
    createTestCommit("First commit");
    createTestCommit("Second commit");
    createTestCommit("Third commit");
    
    mockEventBus->clear();
    
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Commit history"));
    EXPECT_TRUE(containsMessage(notifications, "3 commits"));
    EXPECT_TRUE(containsMessage(notifications, "First commit"));
    EXPECT_TRUE(containsMessage(notifications, "Second commit"));
    EXPECT_TRUE(containsMessage(notifications, "Third commit"));
}

// Test oneline format
TEST_F(HistoryCommandTest, HistoryOnelineFormat) {
    createTestCommit("First test commit with longer message");
    createTestCommit("Second commit");
    
    mockEventBus->clear();
    
    std::vector<std::string> args = {"--oneline"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    // Should show truncated messages in single line format
    EXPECT_TRUE(containsMessage(notifications, "First test commit with longer"));
    EXPECT_TRUE(containsMessage(notifications, "Second commit"));
}

// Test last N commits
TEST_F(HistoryCommandTest, DebugLastNCommitsDetailed) {
    // Create 5 commits
    for (int i = 1; i <= 5; i++) {
        createTestCommit("Commit " + std::to_string(i));
    }
    
    // Debug: check what getCommitHistory returns
    auto commits = repoManager->getCommitHistory();
    std::cout << "=== DEBUG: All commits before limit ===" << std::endl;
    std::cout << "Total commits: " << commits.size() << std::endl;
    for (size_t i = 0; i < commits.size(); i++) {
        std::cout << "Commit " << i << ": " << commits[i].message << std::endl;
    }
    
    mockEventBus->clear();
    
    std::vector<std::string> args = {"--last", "3"};
    std::cout << "=== DEBUG: Executing command with args: --last 3 ===" << std::endl;
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    
    std::cout << "=== DEBUG: All notifications ===" << std::endl;
    for (const auto& event : notifications) {
        std::cout << "Type: " << static_cast<int>(event.type) 
                  << ", Details: " << event.details << std::endl;
    }
    
    // Specific checks
    std::cout << "=== DEBUG: Specific checks ===" << std::endl;
    std::cout << "Contains 'Commit 3': " << containsMessage(notifications, "Commit 3") << std::endl;
    std::cout << "Contains 'Commit 4': " << containsMessage(notifications, "Commit 4") << std::endl;
    std::cout << "Contains 'Commit 5': " << containsMessage(notifications, "Commit 5") << std::endl;
    std::cout << "Contains 'Commit 1': " << containsMessage(notifications, "Commit 1") << std::endl;
    std::cout << "Contains 'Commit 2': " << containsMessage(notifications, "Commit 2") << std::endl;
    
    std::cout << "=== END DEBUG ===" << std::endl;
}

// Test last N with oneline format
TEST_F(HistoryCommandTest, HistoryLastNWithOneline) {
    for (int i = 1; i <= 4; i++) {
        createTestCommit("Commit " + std::to_string(i));
    }
    
    mockEventBus->clear();
    
    std::vector<std::string> args = {"-n", "2", "-o"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    // Should show last 2 commits in oneline format
    EXPECT_TRUE(containsMessage(notifications, "Commit 3"));
    EXPECT_TRUE(containsMessage(notifications, "Commit 4"));
}

// Test error when no repository initialized
TEST_F(HistoryCommandTest, ErrorWhenNoRepository) {
    // Change to directory without repository
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    
    std::vector<std::string> args = {};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Not a SVCS repository"));
}

// Test help command
TEST_F(HistoryCommandTest, ShowHelp) {
    std::vector<std::string> args = {"--help"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Usage: svcs history"));
    EXPECT_TRUE(containsMessage(notifications, "Show history of saves"));
    EXPECT_TRUE(containsMessage(notifications, "Options:"));
    EXPECT_TRUE(containsMessage(notifications, "--oneline"));
    EXPECT_TRUE(containsMessage(notifications, "--last N"));
}

// Test error for missing limit value
TEST_F(HistoryCommandTest, ErrorMissingLimitValue) {
    std::vector<std::string> args = {"--last"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Missing value for --last option"));
}

// Test error for invalid limit value
TEST_F(HistoryCommandTest, ErrorInvalidLimitValue) {
    std::vector<std::string> args = {"--last", "invalid"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Invalid number for --last option"));
}

// Test error for negative limit
TEST_F(HistoryCommandTest, ErrorNegativeLimit) {
    std::vector<std::string> args = {"--last", "-5"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Limit must be positive number"));
}

// Test error for zero limit
TEST_F(HistoryCommandTest, ErrorZeroLimit) {
    std::vector<std::string> args = {"--last", "0"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Limit must be positive number"));
}

// Test error for unknown option
TEST_F(HistoryCommandTest, ErrorUnknownOption) {
    std::vector<std::string> args = {"--unknown"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Unknown option: --unknown"));
}

// Test error for mutually exclusive options
TEST_F(HistoryCommandTest, ErrorMutuallyExclusiveOptions) {
    createTestCommit("Test commit");
    
    std::vector<std::string> args = {"--oneline", "--full"};
    bool result = command->execute(args);
    
    EXPECT_FALSE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Cannot use --oneline and --full together"));
}

// Test full details format
TEST_F(HistoryCommandTest, HistoryFullDetails) {
    createTestCommit("Detailed commit message");
    
    mockEventBus->clear();
    
    std::vector<std::string> args = {"--full"};
    bool result = command->execute(args);
    
    EXPECT_TRUE(result);
    
    auto notifications = mockEventBus->getNotifications();
    EXPECT_TRUE(containsMessage(notifications, "Commit 1:"));
    EXPECT_TRUE(containsMessage(notifications, "Hash:"));
    EXPECT_TRUE(containsMessage(notifications, "Message: Detailed commit message"));
    EXPECT_TRUE(containsMessage(notifications, "Files:"));
}