/**
 * @file IntRemoveCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Test fixture header for integration tests of the RemoveCommand class.
 * @details Declares the RemoveCommandTest fixture, which sets up a controlled
 * environment for testing the 'svcs remove' (or 'svcs rm') command. This fixture
 * manages a temporary SVCS repository and provides helper functions for file
 * manipulation, staging, and output verification.
 *
 * @russian
 * @brief Заголовок тестового фикстура для интеграционных тестов класса RemoveCommand.
 * @details Объявляет фикстур RemoveCommandTest, который настраивает контролируемое
 * окружение для тестирования команды 'svcs remove' (или 'svcs rm'). Этот фикстур
 * управляет временным репозиторием SVCS и предоставляет вспомогательные функции
 * для манипуляции файлами, индексации и проверки вывода.
 */

#pragma once

#include "../../../cli/include/RemoveCommand.hxx"
#include "../mocks/MockSubject.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include <string>

/**
 * @english
 * @class RemoveCommandTest
 * @brief Google Test fixture for running integration tests against the RemoveCommand.
 * @details This fixture provides the necessary infrastructure for testing the
 * file removal command, ensuring it correctly stages files for removal from the
 * repository, handles errors, and respects command line options.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class RemoveCommandTest
 * @brief Google Test фикстур для запуска интеграционных тестов RemoveCommand.
 * @details Этот фикстур предоставляет необходимую инфраструктуру для тестирования
 * команды удаления файлов, обеспечивая корректную индексацию файлов для удаления
 * из репозитория, обработку ошибок и соблюдение параметров командной строки.
 * @ingroup IntegrationTests
 */
class RemoveCommandTest : public ::testing::Test {
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
     * @brief Stages a list of files (using the add command logic for convenience)
     * in the RepositoryManager's staging area.
     * @param files A vector of file names to stage.
     *
     * @russian
     * @brief Индексирует список файлов (используя логику команды add для удобства)
     * в области индексации RepositoryManager.
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
    std::unique_ptr<RemoveCommand> command;

    /**
     * @english
     * @brief Path to the temporary test repository directory.
     *
     * @russian
     * @brief Путь к временной тестовой директории репозитория.
     */
    std::filesystem::path testDir;
};