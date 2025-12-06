/**
 * @file AddCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the test fixture for AddCommand integration tests.
 * @details This fixture sets up an isolated environment with an initialized SVCS repository
 * and provides helper functions for creating test files and directories.
 *
 * @russian
 * @brief Объявление тестового фикстура для интеграционных тестов AddCommand.
 * @details Этот фикстур настраивает изолированное окружение с инициализированным репозиторием SVCS
 * и предоставляет вспомогательные функции для создания тестовых файлов и директорий.
 */

#pragma once

#include "../../../core/include/RepositoryManager.hxx"
#include "../../../cli/include/AddCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>

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

using namespace svcs::test::cli::mocks;

using namespace svcs::core;
using namespace svcs::cli;
using namespace svcs::services;

/**
 * @english
 * @class AddCommandTest
 * @brief Test fixture for AddCommand integration tests.
 * @details Manages the setup and teardown of a temporary, initialized SVCS repository.
 * Provides utility methods to create various file system structures for testing the
 * staging logic of the AddCommand.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class AddCommandTest
 * @brief Тестовый фикстур для интеграционных тестов AddCommand.
 * @details Управляет настройкой и очисткой временного инициализированного репозитория SVCS.
 * Предоставляет служебные методы для создания различных структур файловой системы для тестирования
 * логики добавления в индекс команды AddCommand.
 * @ingroup IntegrationTests
 */
class AddCommandTest : public ::testing::Test {
protected:
    /**
     * @english
     * @brief Mock event bus for capturing system notifications.
     *
     * @russian
     * @brief Mock шина событий для захвата системных уведомлений.
     */
    std::shared_ptr<MockSubject> mockEventBus;

    /**
     * @english
     * @brief Real RepositoryManager instance under test.
     *
     * @russian
     * @brief Реальный экземпляр RepositoryManager под тестом.
     */
    std::shared_ptr<RepositoryManager> repoManager;

    /**
     * @english
     * @brief AddCommand instance under test.
     *
     * @russian
     * @brief Экземпляр AddCommand под тестом.
     */
    std::unique_ptr<AddCommand> command;

    /**
     * @english
     * @brief Path to the temporary directory used as the repository root.
     *
     * @russian
     * @brief Путь к временной директории, используемой как корень репозитория.
     */
    std::filesystem::path testDir;

    /**
     * @english
     * @brief Sets up the test environment.
     * @details Creates a unique temporary directory, changes the current path to it,
     * initializes a repository, and clears initial events.
     *
     * @russian
     * @brief Настраивает тестовое окружение.
     * @details Создает уникальную временную директорию, изменяет текущий путь на нее,
     * инициализирует репозиторий и очищает начальные события.
     */
    void SetUp() override;

    /**
     * @english
     * @brief Tears down the test environment.
     * @details Restores the original current path and recursively removes the temporary directory.
     *
     * @russian
     * @brief Очищает тестовое окружение.
     * @details Восстанавливает оригинальный текущий путь и рекурсивно удаляет временную директорию.
     */
    void TearDown() override;

    /**
     * @english
     * @brief Creates a test file inside the test directory.
     * @param filename The name of the file to create.
     * @param content The content of the file (defaults to "test content").
     *
     * @russian
     * @brief Создает тестовый файл внутри тестовой директории.
     * @param filename Имя создаваемого файла.
     * @param content Содержимое файла (по умолчанию "test content").
     */
    void createTestFile(const std::string& filename, const std::string& content = "test content") const;

    /**
     * @english
     * @brief Creates a test directory inside the test directory.
     * @param dirname The name of the directory to create.
     *
     * @russian
     * @brief Создает тестовую директорию внутри тестовой директории.
     * @param dirname Имя создаваемой директории.
     */
    void createTestDirectory(const std::string& dirname) const;
};

}