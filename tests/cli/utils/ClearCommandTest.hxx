/**
 * @file ClearCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the test fixture for ClearCommand integration tests.
 * @details This fixture sets up an isolated environment with a temporary SVCS repository
 * and handles the redirection of standard input (`std::cin`) to simulate user
 * confirmation during the repository clearing process.
 *
 * @russian
 * @brief Объявление тестового фикстура для интеграционных тестов ClearCommand.
 * @details Этот фикстур настраивает изолированное окружение с временным репозиторием SVCS
 * и обрабатывает перенаправление стандартного ввода (`std::cin`) для имитации
 * подтверждения пользователем во время процесса очистки репозитория.
 */
#pragma once

#include "../../../core/include/RepositoryManager.hxx"
#include "../../../cli/include/ClearCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <sstream>

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

/**
 * @english
 * @class ClearCommandTest
 * @brief Test fixture for ClearCommand integration tests.
 * @details Manages the setup and teardown of a temporary, initialized SVCS repository.
 * Provides utility methods to create test files/directories and to simulate
 * user input required for the 'clear' confirmation prompt.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class ClearCommandTest
 * @brief Тестовый фикстур для интеграционных тестов ClearCommand.
 * @details Управляет настройкой и очисткой временного инициализированного репозитория SVCS.
 * Предоставляет служебные методы для создания тестовых файлов/директорий и для имитации
 * пользовательского ввода, необходимого для подтверждения очистки репозитория.
 * @ingroup IntegrationTests
 */
class ClearCommandTest : public ::testing::Test {
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
     * @brief ClearCommand instance under test.
     *
     * @russian
     * @brief Экземпляр ClearCommand под тестом.
     */
    std::unique_ptr<ClearCommand> command;

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
     * @brief String stream used to feed simulated input to std::cin.
     *
     * @russian
     * @brief Строковый поток, используемый для подачи имитированного ввода в std::cin.
     */
    std::stringstream inputStream;

    /**
     * @english
     * @brief Pointer to the original buffer of std::cin, saved for restoration.
     *
     * @russian
     * @brief Указатель на оригинальный буфер std::cin, сохраненный для восстановления.
     */
    std::streambuf* originalCin;

    /**
     * @english
     * @brief Sets up the test environment.
     * @details Creates and initializes a new temporary SVCS repository and saves the original std::cin buffer.
     *
     * @russian
     * @brief Настраивает тестовое окружение.
     * @details Создает и инициализирует новый временный репозиторий SVCS и сохраняет оригинальный буфер std::cin.
     */
    void SetUp() override;

    /**
     * @english
     * @brief Tears down the test environment.
     * @details Restores the original std::cin buffer and recursively removes the temporary directory.
     *
     * @russian
     * @brief Очищает тестовое окружение.
     * @details Восстанавливает оригинальный буфер std::cin и рекурсивно удаляет временную директорию.
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

    /**
     * @english
     * @brief Simulates user input by redirecting std::cin to a string stream.
     * @param input The string that will be read as user input.
     *
     * @russian
     * @brief Имитирует пользовательский ввод путем перенаправления std::cin в строковый поток.
     * @param input Строка, которая будет прочитана как пользовательский ввод.
     */
    void simulateUserInput(const std::string& input);

    /**
     * @english
     * @brief Checks if the .svcs repository directory exists.
     * @return true if the repository exists, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли директория репозитория .svcs.
     * @return true если репозиторий существует, false в противном случае.
     */
    bool repositoryExists() const;
};

}