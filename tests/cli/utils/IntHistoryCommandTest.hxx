/**
 * @file IntHistoryCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Test fixture header for integration tests of the HistoryCommand class.
 * @details Declares the HistoryCommandTest fixture, which sets up a controlled
 * environment for testing the 'svcs history' command. This includes initializing
 * mock and core components (MockSubject, RepositoryManager) and providing helper
 * functions for creating files, staging changes, and creating mock commits.
 *
 * @russian
 * @brief Заголовок тестового фикстура для интеграционных тестов класса HistoryCommand.
 * @details Объявляет фикстур HistoryCommandTest, который настраивает контролируемое
 * окружение для тестирования команды 'svcs history'. Это включает инициализацию
 * mock и основных компонентов (MockSubject, RepositoryManager) и предоставление
 * вспомогательных функций для создания файлов, индексации изменений и создания mock коммитов.
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "../../../cli/include/HistoryCommand.hxx"
#include "../mocks/MockSubject.hxx"

// Forward declarations for core components used in fixture
class RepositoryManager;
struct Event;

/**
 * @english
 * @class HistoryCommandTest
 * @brief Google Test fixture for running integration tests against the HistoryCommand.
 * @details This fixture provides the necessary infrastructure for integration tests of
 * the history command:
 * - **SetUp/TearDown:** Manage a temporary SVCS repository directory.
 * - **Core Components:** Instances of MockSubject (for event bus) and RepositoryManager (for core logic).
 * - **Helper Methods:** Simplify file creation, staging, and crucially, creating mock commits
 * to populate the history for testing various output formats and limits.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class HistoryCommandTest
 * @brief Google Test фикстур для запуска интеграционных тестов HistoryCommand.
 * @details Этот фикстур предоставляет необходимую инфраструктуру для интеграционных тестов
 * команды history:
 * - **SetUp/TearDown:** Управление временной директорией репозитория SVCS.
 * - **Основные компоненты:** Экземпляры MockSubject (для шины событий) и RepositoryManager (для основной логики).
 * - **Вспомогательные методы:** Упрощают создание файлов, индексацию и, что важно, создание mock коммитов
 * для заполнения истории для тестирования различных форматов вывода и ограничений.
 * @ingroup IntegrationTests
 */
class HistoryCommandTest : public ::testing::Test {
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
    void stageFiles(const std::vector<std::string>& files) const;

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
     * @return true if the message is found, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли конкретное сообщение (подстрока) в любом уведомлении о событии.
     * @param notifications Список захваченных событий.
     * @param message Подстрока для поиска.
     * @return true если сообщение найдено, false в противном случае.
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
    int countMessages(const std::vector<Event>& notifications, const std::string& message);

    /**
     * @english
     * @brief Shared pointer to the mock event bus (observer).
     *
     * @russian
     * @brief Общий указатель на mock шину событий (наблюдатель).
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
    std::unique_ptr<HistoryCommand> command;

    /**
     * @english
     * @brief Path to the temporary test repository directory.
     *
     * @russian
     * @brief Путь к временной тестовой директории репозитория.
     */
    std::filesystem::path testDir;
};