/**
 * @file IntSaveCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Test fixture header for integration tests of the SaveCommand (commit) class.
 * @details Declares the SaveCommandTest fixture, which sets up a controlled
 * environment for testing the 'svcs save' command. This includes initializing
 * mock and core components (MockSubject, RepositoryManager) and providing helper
 * functions for file manipulation and staging changes prior to a save operation.
 *
 * @russian
 * @brief Заголовок тестового фикстура для интеграционных тестов класса SaveCommand (коммит).
 * @details Объявляет фикстур SaveCommandTest, который настраивает контролируемое
 * окружение для тестирования команды 'svcs save'. Это включает инициализацию
 * mock и основных компонентов (MockSubject, RepositoryManager) и предоставление
 * вспомогательных функций для манипуляции файлами и индексации изменений перед операцией сохранения.
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "../../../cli/include/SaveCommand.hxx"
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
using svcs::cli::SaveCommand;
using svcs::test::cli::mocks::MockSubject;

/**
 * @english
 * @class SaveCommandTest
 * @brief Google Test fixture for running integration tests against the SaveCommand (commit).
 * @details This fixture provides the necessary infrastructure for testing the save command:
 * - **SetUp/TearDown:** Manage a temporary SVCS repository directory.
 * - **Core Components:** Instances of MockSubject (for event bus) and RepositoryManager (for core logic).
 * - **Helper Methods:** Simplify file creation, directory setup, staging, and verification of notifications.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class SaveCommandTest
 * @brief Google Test фикстур для запуска интеграционных тестов SaveCommand (коммит).
 * @details Этот фикстур предоставляет необходимую инфраструктуру для тестирования команды save:
 * - **SetUp/TearDown:** Управление временной директорией репозитория SVCS.
 * - **Основные компоненты:** Экземпляры MockSubject (для шины событий) и RepositoryManager (для основной логики).
 * - **Вспомогательные методы:** Упрощают создание файлов, настройку директорий, индексацию и проверку уведомлений.
 * @ingroup IntegrationTests
 */
class SaveCommandTest : public ::testing::Test {
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
    void createTestFile(const std::string& filename, const std::string& content);

    /**
     * @english
     * @brief Creates a test directory in the temporary location.
     * @param dirname The relative path/name of the directory.
     *
     * @russian
     * @brief Создает тестовую директорию во временном расположении.
     * @param dirname Относительный путь/имя директории.
     */
    void createTestDirectory(const std::string& dirname);

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
    bool containsMessage(const std::vector<Event>& notifications, const std::string& message);

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
    std::unique_ptr<SaveCommand> command;

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