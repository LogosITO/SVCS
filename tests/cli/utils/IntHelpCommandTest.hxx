/**
 * @file IntHelpCommandTest.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Test fixture header for integration tests of the HelpCommand class.
 * @details Declares the HelpCommandTest fixture, which sets up the mock event
 * bus and the mock help service necessary to verify the command's delegation
 * logic and output for both general and command-specific help requests.
 *
 * @russian
 * @brief Заголовок тестового фикстура для интеграционных тестов класса HelpCommand.
 * @details Объявляет фикстур HelpCommandTest, который настраивает mock шину событий
 * и mock сервис справки, необходимые для проверки логики делегирования команды
 * и вывода для общих и командных запросов справки.
 */

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "../../../cli/include/HelpCommand.hxx"
#include "../mocks/MockSubject.hxx"
#include "../mocks/MockHelpService.hxx"

// Forward declaration for the Event structure used in helpers
struct Event;

/**
 * @english
 * @class HelpCommandTest
 * @brief Google Test fixture for running unit/integration tests against the HelpCommand.
 * @details This fixture is crucial for isolating the HelpCommand's logic, ensuring
 * it correctly delegates help requests to the **MockHelpService** and reports
 * the resulting output via the **MockSubject** (event bus).
 * @ingroup UnitTests
 *
 * @russian
 * @class HelpCommandTest
 * @brief Google Test фикстур для запуска модульных/интеграционных тестов HelpCommand.
 * @details Этот фикстур критически важен для изоляции логики HelpCommand, обеспечивая
 * правильное делегирование запросов справки в **MockHelpService** и отчетность
 * о результатах через **MockSubject** (шину событий).
 * @ingroup UnitTests
 */
class HelpCommandTest : public ::testing::Test {
protected:
    /**
     * @english
     * @brief Sets up the testing environment before each test.
     * @details Initializes the MockSubject, MockHelpService, and the HelpCommand instance.
     *
     * @russian
     * @brief Настраивает тестовое окружение перед каждым тестом.
     * @details Инициализирует MockSubject, MockHelpService и экземпляр HelpCommand.
     */
    void SetUp() override;

    /**
     * @english
     * @brief Tears down the testing environment after each test.
     * @details Clears the event bus and the mock help service's internal state.
     *
     * @russian
     * @brief Очищает тестовое окружение после каждого теста.
     * @details Очищает шину событий и внутреннее состояние mock сервиса справки.
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
     * @brief Shared pointer to the mock event bus (observer) used to capture command output.
     *
     * @russian
     * @brief Общий указатель на mock шину событий (наблюдатель), используемую для захвата вывода команды.
     */
    std::shared_ptr<MockSubject> mockEventBus;

    /**
     * @english
     * @brief Shared pointer to the mock help service, used to control help data and verify calls.
     *
     * @russian
     * @brief Общий указатель на mock сервис справки, используемый для управления данными справки и проверки вызовов.
     */
    std::shared_ptr<MockHelpService> mockHelpService;

    /**
     * @english
     * @brief Unique pointer to the command being tested.
     *
     * @russian
     * @brief Уникальный указатель на тестируемую команду.
     */
    std::unique_ptr<HelpCommand> command;
};