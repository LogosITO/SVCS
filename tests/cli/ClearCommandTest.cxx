/**
 * @file ClearCommandTest.cxx
 * @brief Tests for the 'clear' command (ClearCommand) using Google Test.
 *
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <iostream>

#include "../../services/Event.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../cli/include/ClearCommand.hxx"

/**
 * @brief Mock-реализация ISubject для отслеживания событий.
 */
class MockSubject : public ISubject {
public:
    std::vector<Event> notifications;

    void attach(std::shared_ptr<IObserver> observer) override {}
    void detach(std::shared_ptr<IObserver> observer) override {}

    void notify(const Event& event) const override {
        const_cast<MockSubject*>(this)->notifications.push_back(event);
    }
    
    void clear() {
        notifications.clear();
    }
    
    bool containsMessage(const std::string& message) const {
        for (const auto& notification : notifications) {
            if (notification.details.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    bool containsEventType(Event::Event::Type type) const {
        for (const auto& notification : notifications) {
            if (notification.type == type) {
                return true;
            }
        }
        return false;
    }
    
    bool containsMessage(Event::Event::Type type, const std::string& message) const {
        for (const auto& notification : notifications) {
            if (notification.type == type && 
                notification.details.find(message) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
};

class ClearCommandTest : public ::testing::Test {
protected:
    std::shared_ptr<MockSubject> mockEventBus;
    std::shared_ptr<RepositoryManager> repoManager;
    std::unique_ptr<ClearCommand> command;
    std::filesystem::path testDir;
    
    // Для тестирования пользовательского ввода
    std::stringstream inputStream;
    std::streambuf* originalCin;

    void SetUp() override {
        mockEventBus = std::make_shared<MockSubject>();
        repoManager = std::make_shared<RepositoryManager>(mockEventBus);
        command = std::make_unique<ClearCommand>(mockEventBus, repoManager);
        
        // Создаем временную директорию для тестов
        testDir = std::filesystem::temp_directory_path() / "svcs_test_clear";
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
        std::filesystem::current_path(testDir);
        
        // Инициализируем репозиторий
        repoManager->initializeRepository(".", true);
        mockEventBus->clear();
        
        // Сохраняем оригинальный cin для восстановления
        originalCin = std::cin.rdbuf();
    }

    void TearDown() override {
        // Восстанавливаем оригинальный cin
        std::cin.rdbuf(originalCin);
        
        std::filesystem::current_path(std::filesystem::temp_directory_path());
        std::filesystem::remove_all(testDir);
        mockEventBus->clear();
    }
    
    void createTestFile(const std::string& filename, const std::string& content = "test content") {
        std::ofstream file(testDir / filename);
        file << content;
        file.close();
    }
    
    void createTestDirectory(const std::string& dirname) {
        std::filesystem::create_directories(testDir / dirname);
    }
    
    void simulateUserInput(const std::string& input) {
        inputStream.str(input);
        inputStream.clear();
        std::cin.rdbuf(inputStream.rdbuf());
    }
    
    bool repositoryExists() const {
        return std::filesystem::exists(testDir / ".svcs");
    }
};

TEST_F(ClearCommandTest, Failure_NoRepository) {
    // Тестируем случай когда репозиторий не инициализирован
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    auto tempDir = std::filesystem::temp_directory_path() / "svcs_test_no_repo_clear";
    std::filesystem::remove_all(tempDir);
    std::filesystem::create_directories(tempDir);
    std::filesystem::current_path(tempDir);
    
    auto tempEventBus = std::make_shared<MockSubject>();
    auto tempRepoManager = std::make_shared<RepositoryManager>(tempEventBus);
    ClearCommand tempCommand(tempEventBus, tempRepoManager);
    
    bool result = tempCommand.execute({});
    
    EXPECT_FALSE(result);
    EXPECT_TRUE(tempEventBus->containsMessage("No SVCS repository found"));
    
    std::filesystem::current_path(testDir);
    std::filesystem::remove_all(tempDir);
}

TEST_F(ClearCommandTest, Success_HelpFlag) {
    bool result = command->execute({"--help"});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("Usage:"));
    EXPECT_TRUE(mockEventBus->containsMessage("Description:"));
    EXPECT_TRUE(mockEventBus->containsMessage("Options:"));
    
    // Help не должен удалять репозиторий
    EXPECT_TRUE(repositoryExists());
}

TEST_F(ClearCommandTest, Success_ForceFlag) {
    // Создаем несколько файлов в репозитории
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    bool result = command->execute({"--force"});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("SVCS repository successfully removed"));
    EXPECT_FALSE(repositoryExists());
}

TEST_F(ClearCommandTest, Success_ForceShortFlag) {
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    bool result = command->execute({"-f"});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("SVCS repository successfully removed"));
    EXPECT_FALSE(repositoryExists());
}

TEST_F(ClearCommandTest, Success_UserConfirmationYes) {
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    // Симулируем подтверждение пользователя
    simulateUserInput("y\n");
    
    bool result = command->execute({});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("SVCS repository successfully removed"));
    EXPECT_FALSE(repositoryExists());
}

TEST_F(ClearCommandTest, Success_UserConfirmationYesUppercase) {
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    simulateUserInput("Y\n");
    
    bool result = command->execute({});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("SVCS repository successfully removed"));
    EXPECT_FALSE(repositoryExists());
}

TEST_F(ClearCommandTest, Success_UserConfirmationYesFull) {
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    simulateUserInput("yes\n");
    
    bool result = command->execute({});
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("SVCS repository successfully removed"));
    EXPECT_FALSE(repositoryExists());
}

TEST_F(ClearCommandTest, Failure_UserConfirmationNo) {
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    // Симулируем отказ пользователя
    simulateUserInput("n\n");
    
    bool result = command->execute({});
    
    EXPECT_FALSE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("Clear operation cancelled"));
    
    // Репозиторий должен остаться
    EXPECT_TRUE(repositoryExists());
}

TEST_F(ClearCommandTest, Failure_UserConfirmationEmpty) {
    createTestFile("test.txt");
    repoManager->addFileToStaging("test.txt");
    
    // Пустой ввод (по умолчанию No)
    simulateUserInput("\n");
    
    bool result = command->execute({});
    
    EXPECT_FALSE(result);
    EXPECT_TRUE(mockEventBus->containsMessage("Clear operation cancelled"));
    EXPECT_TRUE(repositoryExists());
}

TEST_F(ClearCommandTest, Success_WarningMessages) {
    createTestFile("file1.txt");
    createTestFile("file2.txt");
    repoManager->addFileToStaging("file1.txt");
    repoManager->addFileToStaging("file2.txt");
    
    simulateUserInput("y\n");
    
    bool result = command->execute({});
    
    EXPECT_TRUE(result);
    
    // Должны быть предупреждающие сообщения
    EXPECT_TRUE(mockEventBus->containsMessage(Event::WARNING_MESSAGE, "permanently remove"));
    EXPECT_TRUE(mockEventBus->containsMessage(Event::WARNING_MESSAGE, "remove"));
    EXPECT_TRUE(mockEventBus->containsMessage(Event::WARNING_MESSAGE, "files"));
}

TEST_F(ClearCommandTest, Debug_RepositoryStructure) {
    createTestFile("test1.txt");
    createTestFile("test2.txt");
    repoManager->addFileToStaging("test1.txt");
    repoManager->addFileToStaging("test2.txt");
    
    std::cout << "=== DEBUG Repository Structure ===" << std::endl;
    std::cout << "Test directory: " << testDir << std::endl;
    std::cout << ".svcs exists before clear: " << repositoryExists() << std::endl;
    
    if (repositoryExists()) {
        std::cout << ".svcs contents:" << std::endl;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(testDir / ".svcs")) {
            std::cout << "  " << entry.path() << std::endl;
        }
    }
    
    simulateUserInput("y\n");
    bool result = command->execute({});
    
    std::cout << "Result: " << result << std::endl;
    std::cout << ".svcs exists after clear: " << repositoryExists() << std::endl;
    
    std::cout << "Notifications:" << std::endl;
    for (size_t i = 0; i < mockEventBus->notifications.size(); ++i) {
        const auto& n = mockEventBus->notifications[i];
        std::cout << "  [" << i << "] Type=" << static_cast<int>(n.type) 
                  << ", Details='" << n.details << "'" << std::endl;
    }
    std::cout << "=== END DEBUG ===" << std::endl;
    
    EXPECT_TRUE(result);
    EXPECT_FALSE(repositoryExists());
}