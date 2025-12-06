/**
 * @file IntVersionCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Test fixture header for integration tests of the VersionCommand class.
 * @details Declares the VersionCommandTest fixture, which sets up the mock event
 * bus and the command instance necessary to verify the output and behavior of
 * the 'svcs version' command. Since the command is stateless, the setup is minimal.
 *
 * @russian
 * @brief Заголовок тестового фикстура для интеграционных тестов класса VersionCommand.
 * @details Объявляет фикстур VersionCommandTest, который настраивает mock шину событий
 * и экземпляр команды, необходимые для проверки вывода и поведения команды 'svcs version'.
 * Поскольку команда не имеет состояния, настройка минимальна.
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "../../../cli/include/VersionCommand.hxx"
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
using svcs::cli::VersionCommand;
using svcs::test::cli::mocks::MockSubject;

/**
 * @english
 * @class VersionCommandTest
 * @brief Google Test fixture for running unit/integration tests against the VersionCommand.
 * @details This fixture provides the necessary infrastructure for testing the VersionCommand:
 * - **SetUp/TearDown:** Initialize and clean up the mock event bus and the command object.
 * - **Core Components:** Instances of MockSubject (for capturing output) and the VersionCommand itself.
 * - **Helper Methods:** Simplify checking the captured event notifications for specific content.
 * @ingroup IntegrationTests
 *
 * @russian
 * @class VersionCommandTest
 * @brief Google Test фикстур для запуска модульных/интеграционных тестов VersionCommand.
 * @details Этот фикстур предоставляет необходимую инфраструктуру для тестирования VersionCommand:
 * - **SetUp/TearDown:** Инициализация и очистка mock шины событий и объекта команды.
 * - **Основные компоненты:** Экземпляры MockSubject (для захвата вывода) и самого VersionCommand.
 * - **Вспомогательные методы:** Упрощают проверку захваченных уведомлений о событиях на наличие конкретного содержимого.
 * @ingroup IntegrationTests
 */
class VersionCommandTest : public ::testing::Test {
protected:
    /**
     * @english
     * @brief Sets up the testing environment before each test.
     *
     * @russian
     * @brief Настраивает тестовое окружение перед каждым тестом.
     */
    void SetUp() override;

    /**
     * @english
     * @brief Tears down the testing environment after each test.
     *
     * @russian
     * @brief Очищает тестовое окружение после каждого теста.
     */
    void TearDown() override;

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
     * @brief Checks if any event notification contains the exact specified message.
     * @param notifications The list of captured events.
     * @param message The exact message to search for.
     * @return true if an exact match is found, false otherwise.
     *
     * @russian
     * @brief Проверяет, содержит ли какое-либо уведомление о событии точно указанное сообщение.
     * @param notifications Список захваченных событий.
     * @param message Точное сообщение для поиска.
     * @return true если найдено точное совпадение, false в противном случае.
     */
    bool containsExactMessage(const std::vector<Event>& notifications, const std::string& message);

    /**
     * @english
     * @brief Shared pointer to the mock event bus for capturing command output.
     *
     * @russian
     * @brief Общий указатель на mock шину событий для захвата вывода команды.
     */
    std::shared_ptr<MockSubject> mockEventBus;

    /**
     * @english
     * @brief Unique pointer to the command being tested.
     *
     * @russian
     * @brief Уникальный указатель на тестируемую команду.
     */
    std::unique_ptr<VersionCommand> command;
};

}