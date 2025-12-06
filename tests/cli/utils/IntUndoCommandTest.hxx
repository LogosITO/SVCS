/**
 * @file IntUndoCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Test fixture header for integration tests of the UndoCommand class.
 * @details Declares the UndoCommandTest fixture, which sets up a controlled
 * environment for testing the 'svcs undo' command. This includes initializing
 * mock and core components (MockSubject, RepositoryManager) and providing helper
 * functions for file manipulation, staging, committing, and simulating user
 * input for confirmation prompts.
 *
 * @russian
 * @brief Заголовок тестового фикстура для интеграционных тестов класса UndoCommand.
 * @details Объявляет фикстур UndoCommandTest, который настраивает контролируемое
 * окружение для тестирования команды 'svcs undo'. Это включает инициализацию
 * mock и основных компонентов (MockSubject, RepositoryManager) и предоставление
 * вспомогательных функций для манипуляции файлами, индексации, коммитов и имитации
 * пользовательского ввода для подтверждающих запросов.
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "../../../cli/include/UndoCommand.hxx"
#include "../mocks/MockSubject.hxx"

/**
 * @english
 * @namespace svcs::test::cli::utils
 * @brief Utility classes and test fixtures for CLI command testing.
 * @details Contains test fixtures, helper classes, and utility functions
 * that support the testing of CLI commands. These components provide
 * common setup, teardown, and testing patterns for command integration tests.
 *
 * @russian
 * @namespace svcs::test::cli::utils
 * @brief Служебные классы и тестовые фикстуры для тестирования CLI команд.
 * @details Содержит тестовые фикстуры, вспомогательные классы и служебные функции,
 * которые поддерживают тестирование CLI команд. Эти компоненты предоставляют
 * общие шаблоны настройки, очистки и тестирования для интеграционных тестов команд.
 */
namespace svcs::test::cli::utils {

using svcs::services::Event;
using svcs::core::RepositoryManager;
using svcs::cli::UndoCommand;
using svcs::test::cli::mocks::MockSubject;

/**
 * @english
 * @class UndoCommandTest
 * @brief Google Test fixture for running integration tests against the UndoCommand.
 * @details This fixture provides the necessary infrastructure for testing the undo command:
 * - **SetUp/TearDown:** Manage a temporary SVCS repository directory.
 * - **Core Components:** Instances of MockSubject (for event bus/output/input simulation) and RepositoryManager (for core logic).
 * - **Helper Methods:** Simplify file manipulation, staging, committing, and verification of notifications.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class UndoCommandTest
 * @brief Google Test фикстур для запуска интеграционных тестов UndoCommand.
 * @details Этот фикстур предоставляет необходимую инфраструктуру для тестирования команды undo:
 * - **SetUp/TearDown:** Управление временной директорией репозитория SVCS.
 * - **Основные компоненты:** Экземпляры MockSubject (для шины событий/вывода/имитации ввода) и RepositoryManager (для основной логики).
 * - **Вспомогательные методы:** Упрощают манипуляцию файлами, индексацию, коммиты и проверку уведомлений.
 * @ingroup IntegrationTests
 */
class UndoCommandTest : public ::testing::Test {
protected:
    /**
     * @english
     * @brief Sets up the testing environment before each test.
     * @details Initializes components, creates the temporary directory, and initializes the repository.
     *
     * @russian
     * @brief Настраивает тестовое окружение перед каждым тестом.
     * @details Инициализирует компоненты, создает временную директорию и инициализирует репозиторий.
     */
    void SetUp() override;

    /**
     * @english
     * @brief Tears down the testing environment after each test.
     * @details Cleans up the temporary directory and restores the working path.
     *
     * @russian
     * @brief Очищает тестовое окружение после каждого теста.
     * @details Очищает временную директорию и восстанавливает рабочий путь.
     */
    void TearDown() override;

    /**
     * @english
     * @brief Creates a test file with specified content in the temporary directory.
     * @param filename The relative path/name of the file.
     * @param content The content to write to the file.
     *
     * @russian
     * @brief Создает тестовый файл с указанным содержимым во временной директории.
     * @param filename Относительный путь/имя файла.
     * @param content Содержимое для записи в файл.
     */
    void createTestFile(const std::string& filename, const std::string& content) const;

    /**
     * @english
     * @brief Creates a test directory in the temporary location.
     * @param dirname The relative path/name of the directory.
     *
     * @russian
     * @brief Создает тестовую директорию во временном расположении.
     * @param dirname Относительный путь/имя директории.
     */
    void createTestDirectory(const std::string& dirname) const;

    /**
     * @english
     * @brief Stages a list of files using the RepositoryManager's staging logic.
     * @param files A vector of file names to stage.
     *
     * @russian
     * @brief Индексирует список файлов с использованием логики индексации RepositoryManager.
     * @param files Вектор имен файлов для индексации.
     */
    void stageFiles(const std::vector<std::string>& files);

    /**
     * @english
     * @brief Creates a new commit (save) with a specified message.
     * @details Internally stages files (if needed) and calls the repository manager's commit logic.
     * @param message The commit message.
     * @return true if the commit was successful, false otherwise.
     *
     * @russian
     * @brief Создает новый коммит (сохранение) с указанным сообщением.
     * @details Внутренне индексирует файлы (если необходимо) и вызывает логику коммита менеджера репозитория.
     * @param message Сообщение коммита.
     * @return true если коммит успешен, false в противном случае.
     */
    bool createTestCommit(const std::string& message);

    /**
     * @english
     * @brief Checks if a specific message (substring) exists in any event notification.
     * @param notifications The list of captured events.
     * @param message The substring to search for.
     * @return true if the message is found in any notification, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли конкретное сообщение (подстрока) в любом уведомлении о событии.
     * @param notifications Список захваченных событий.
     * @param message Подстрока для поиска.
     * @return true если сообщение найдено в любом уведомлении, false в противном случае.
     */
    static bool containsMessage(const std::vector<Event>& notifications, const std::string& message);

    /**
     * @english
     * @brief Counts the occurrences of a specific message (substring) across all event notifications.
     * @param notifications The list of captured events.
     * @param message The substring to count.
     * @return The number of events containing the substring.
     *
     * @russian
     * @brief Подсчитывает количество вхождений конкретного сообщения (подстроки) во всех уведомлениях о событиях.
     * @param notifications Список захваченных событий.
     * @param message Подстрока для подсчета.
     * @return Количество событий, содержащих подстроку.
     */
    static int countMessages(const std::vector<Event>& notifications, const std::string& message);

    /**
     * @english
     * @brief Simulates user input by pushing the string into the mock event bus's input queue.
     * @details This is crucial for testing the confirmation prompt in the UndoCommand.
     * @param input The string to simulate as user input (e.g., "y\n" or "n\n").
     *
     * @russian
     * @brief Имитирует пользовательский ввод, помещая строку в очередь ввода mock шины событий.
     * @details Это критически важно для тестирования подтверждающего запроса в UndoCommand.
     * @param input Строка для имитации пользовательского ввода (например, "y\n" или "n\n").
     */
    static void simulateUserInput(const std::string& input);

    /**
     * @english
     * @brief Shared pointer to the mock event bus (observer/input simulator).
     *
     * @russian
     * @brief Общий указатель на mock шину событий (наблюдатель/имитатор ввода).
     */
    std::shared_ptr<MockSubject> mockEventBus;

    /**
     * @english
     * @brief Shared pointer to the repository core logic component.
     *
     * @russian
     * @brief Общий указатель на компонент основной логики репозитория.
     */
    std::shared_ptr<RepositoryManager> repoManager;

    /**
     * @english
     * @brief Unique pointer to the command being tested.
     *
     * @russian
     * @brief Уникальный указатель на тестируемую команду.
     */
    std::unique_ptr<UndoCommand> command;

    /**
     * @english
     * @brief Path to the temporary test repository directory.
     *
     * @russian
     * @brief Путь к временной тестовой директории репозитория.
     */
    std::filesystem::path testDir;
};

}