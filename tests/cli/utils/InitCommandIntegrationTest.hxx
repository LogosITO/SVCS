/**
 * @file InitCommandIntegrationTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the test fixture for InitCommand integration tests.
 * @details This fixture sets up an isolated environment with a temporary directory
 * and uses the real RepositoryManager to verify end-to-end repository initialization.
 *
 * @russian
 * @brief Объявление тестового фикстура для интеграционных тестов InitCommand.
 * @details Этот фикстур настраивает изолированное окружение с временной директорией
 * и использует реальный RepositoryManager для проверки сквозной инициализации репозитория.
 */

#pragma once

#include "../../../core/include/RepositoryManager.hxx"
#include "../../../cli/include/InitCommand.hxx"
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

using svcs::services::Event;
using svcs::core::RepositoryManager;
using svcs::cli::InitCommand;
using svcs::test::cli::mocks::MockSubject;

/**
 * @english
 * @class InitCommandIntegrationTest
 * @brief Test fixture for InitCommand integration tests.
 * @details Manages the setup and teardown of a temporary, isolated directory on the filesystem
 * for each test case. It uses the real RepositoryManager to test actual repository
 * creation logic and a MockSubject to verify event publications.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class InitCommandIntegrationTest
 * @brief Тестовый фикстур для интеграционных тестов InitCommand.
 * @details Управляет настройкой и очисткой временной изолированной директории в файловой системе
 * для каждого тестового случая. Использует реальный RepositoryManager для тестирования фактической
 * логики создания репозитория и MockSubject для проверки публикации событий.
 * @ingroup IntegrationTests
 */
class InitCommandIntegrationTest : public ::testing::Test {
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
     * @brief InitCommand instance under test.
     *
     * @russian
     * @brief Экземпляр InitCommand под тестом.
     */
    std::unique_ptr<InitCommand> command;

    /**
     * @english
     * @brief Path to the temporary directory created for the test.
     *
     * @russian
     * @brief Путь к временной директории, созданной для теста.
     */
    std::filesystem::path testDir;

    /**
     * @english
     * @brief Sets up the test environment.
     * @details Creates a unique temporary directory, changes the current path to it,
     * and initializes the RepositoryManager and InitCommand.
     *
     * @russian
     * @brief Настраивает тестовое окружение.
     * @details Создает уникальную временную директорию, изменяет текущий путь на нее
     * и инициализирует RepositoryManager и InitCommand.
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
};

}