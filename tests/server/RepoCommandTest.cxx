/**
 * @file RepoCommandTest.cpp
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Integration tests for the RepoCommand class.
 * @details This file contains tests for the @ref RepoCommand class, which
 * manages remote repositories using @ref RemoteManager. The tests
 * use the Google Test (gtest) framework.
 *
 * @b Testing_Scope:
 * - **Repository Context**: Verifies that the command fails
 * if not inside an initialized SVCS repository.
 * - **Subcommands**: Tests the full lifecycle of remote repositories
 * (`add`, `remove`, `rename`, `list`).
 * - **Input/Output**: Verifies that console output (`std::cout`) matches
 * expected success messages and that errors are handled correctly.
 * - **Filesystem**: Indirectly tests persistence, since
 * @ref RemoteManager saves changes to the 'remotes' file.
 *
 * The test environment (`RepoCommandTest`) creates and cleans up a temporary
 * repository for each test. Tests temporarily change the current working
 * directory (`fs::current_path`) to simulate running the command from the repository root.
 *
 * @russian
 * @brief Интеграционные тесты для класса RepoCommand.
 * @details Этот файл содержит тесты для класса @ref RepoCommand, который
 * управляет удаленными репозиториями с помощью @ref RemoteManager. Тесты
 * используют фреймворк Google Test (gtest).
 *
 * @b Область_тестирования:
 * - **Контекст репозитория**: Проверяет, что команда завершается неудачей,
 * если не находится внутри инициализированного SVCS репозитория.
 * - **Субкоманды**: Тестирует полный жизненный цикл удаленных репозиториев
 * (`add`, `remove`, `rename`, `list`).
 * - **Ввод/вывод**: Проверяет, что вывод на консоль (`std::cout`) соответствует
 * ожидаемым сообщениям об успехе и что ошибки корректно обрабатываются.
 * - **Файловая система**: Косвенно проверяет сохранение состояния, так как
 * @ref RemoteManager сохраняет изменения в файл 'remotes'.
 *
 * Тестовое окружение (`RepoCommandTest`) создает и очищает временный
 * репозиторий для каждого теста. Тесты временно изменяют текущий рабочий
 * каталог (`fs::current_path`) для имитации запуска команды из корня репозитория.
 */

#include <gtest/gtest.h>
#include "../../server/include/RepoCommand.hxx"
#include "../../services/EventBus.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

class SimpleEventBus : public ISubject {
public:
    void attach(std::shared_ptr<IObserver>) override {}
    void detach(std::shared_ptr<IObserver>) override {}
    void notify(const Event&) const override {}
};

class RepoCommandTest : public ::testing::Test {
protected:
    std::shared_ptr<SimpleEventBus> event_bus;
    std::shared_ptr<RepositoryManager> repo_manager;
    std::unique_ptr<RepoCommand> command;
    fs::path temp_dir;

    void SetUp() override {
        event_bus = std::make_shared<SimpleEventBus>();
        repo_manager = std::make_shared<RepositoryManager>(event_bus);

        temp_dir = fs::temp_directory_path() / "repo_command_test";
        fs::remove_all(temp_dir);
        fs::create_directories(temp_dir);

        command = std::make_unique<RepoCommand>(event_bus, repo_manager);
    }

    void TearDown() override {
        command.reset();
        repo_manager.reset();
        event_bus.reset();
        fs::remove_all(temp_dir);
    }

    void createTestRepo() {
        // Create repository directly in temp dir
        repo_manager->initializeRepository(temp_dir.string());
    }
};

TEST_F(RepoCommandTest, FailsWhenNotInRepository) {
    // No repository created - should fail
    EXPECT_FALSE(command->execute({"list"}));
}

TEST_F(RepoCommandTest, ListsEmptyRepositories) {
    createTestRepo();

    // Mock being in the repository by changing directory
    auto original_cwd = fs::current_path();
    fs::current_path(temp_dir);

    testing::internal::CaptureStdout();
    bool result = command->execute({"list"});
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(result);
    EXPECT_TRUE(output.find("No remote repositories configured") != std::string::npos);

    fs::current_path(original_cwd);
}

TEST_F(RepoCommandTest, AddRemoteFailsWithInvalidArgs) {
    createTestRepo();
    auto original_cwd = fs::current_path();
    fs::current_path(temp_dir);

    EXPECT_FALSE(command->execute({"add"})); // Missing args
    EXPECT_FALSE(command->execute({"add", "origin"})); // Missing URL

    fs::current_path(original_cwd);
}

TEST_F(RepoCommandTest, RemoveRemoteSuccess) {
    createTestRepo();
    auto original_cwd = fs::current_path();
    fs::current_path(temp_dir);

    // Add then remove
    command->execute({"add", "origin", "user@server.com:/repo.git"});

    testing::internal::CaptureStdout();
    EXPECT_TRUE(command->execute({"remove", "origin"}));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Remote repository 'origin' removed") != std::string::npos);

    fs::current_path(original_cwd);
}

TEST_F(RepoCommandTest, BasicWorkflow) {
    createTestRepo();
    auto original_cwd = fs::current_path();
    fs::current_path(temp_dir);

    // Add remote
    EXPECT_TRUE(command->execute({"add", "origin", "user@server.com:/repo.git"}));

    // List should show it
    testing::internal::CaptureStdout();
    EXPECT_TRUE(command->execute({"list"}));
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("origin") != std::string::npos);

    // Rename it
    testing::internal::CaptureStdout();
    EXPECT_TRUE(command->execute({"rename", "origin", "primary"}));
    output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("renamed") != std::string::npos);

    // List should show new name
    testing::internal::CaptureStdout();
    EXPECT_TRUE(command->execute({"list"}));
    output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("primary") != std::string::npos);
    EXPECT_FALSE(output.find("origin") != std::string::npos);

    fs::current_path(original_cwd);
}

TEST_F(RepoCommandTest, EmptyCommandShowsList) {
    createTestRepo();
    auto original_cwd = fs::current_path();
    fs::current_path(temp_dir);

    testing::internal::CaptureStdout();
    EXPECT_TRUE(command->execute({}));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("No remote repositories configured") != std::string::npos);

    fs::current_path(original_cwd);
}