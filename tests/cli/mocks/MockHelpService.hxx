/**
 * @file MockHelpService.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Mock implementation of the HelpService interface for unit testing purposes.
 * @details This class provides a mock implementation with the same interface as HelpService
 * but with controlled behavior for testing. It includes utility methods (`setAvailableCommands`,
 * `wasCommandHelpCalled`, etc.) to allow tests to configure the mock's behavior and inspect its interactions.
 * This mock is essential for unit testing the **HelpCommand** without depending
 * on the actual implementation of the CLI command registration system.
 *
 * @russian
 * @brief Mock реализация интерфейса HelpService для целей модульного тестирования.
 * @details Этот класс предоставляет mock реализацию с тем же интерфейсом, что и HelpService,
 * но с контролируемым поведением для тестирования. Он включает служебные методы (`setAvailableCommands`,
 * `wasCommandHelpCalled` и т.д.) чтобы позволить тестам настраивать поведение mock'а и проверять его взаимодействия.
 * Этот mock необходим для модульного тестирования **HelpCommand** без зависимости
 * от фактической реализации системы регистрации CLI команд.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include "MockSubject.hxx"
#include "../../../services/Event.hxx"

/**
 * @english
 * @namespace svcs::test::cli::mocks
 * @brief Mock objects and test doubles for CLI command testing.
 * @details Contains mock implementations of CLI-related interfaces used
 * for testing command behavior, help systems, and user interactions.
 * These mocks provide controlled command responses and verification
 * capabilities for CLI command unit tests.
 *
 * @russian
 * @namespace svcs::test::cli::mocks
 * @brief Mock объекты и тестовые дубли для тестирования CLI команд.
 * @details Содержит mock реализации CLI-интерфейсов, используемых
 * для тестирования поведения команд, систем справки и взаимодействий с пользователем.
 * Эти моки предоставляют контролируемые ответы команд и возможности
 * верификации для модульных тестов CLI команд.
 */
namespace svcs::test::cli::mocks {

using svcs::services::Event;

/**
 * @english
 * @class MockHelpService
 * @brief A mock implementation of the HelpService for testing the HelpCommand.
 * @details This mock allows test cases to:
 * 1. **Configure** the list of available commands.
 * 2. **Configure** the descriptions for specific commands.
 * 3. **Verify** which command's help was requested by the `HelpCommand`.
 * 4. Use the provided MockSubject for emitting debug information during testing.
 *
 * @russian
 * @class MockHelpService
 * @brief Mock реализация HelpService для тестирования HelpCommand.
 * @details Этот mock позволяет тестовым случаям:
 * 1. **Настраивать** список доступных команд.
 * 2. **Настраивать** описания для конкретных команд.
 * 3. **Проверять**, справка какой команды была запрошена `HelpCommand`.
 * 4. Использовать предоставленный MockSubject для вывода отладочной информации во время тестирования.
 */
class MockHelpService {
private:
    /**
     * @english
     * @brief Storage for the list of available commands to be returned by the mock.
     *
     * @russian
     * @brief Хранилище для списка доступных команд, возвращаемых mock'ом.
     */
    std::vector<std::string> availableCommands_;

    /**
     * @english
     * @brief Storage for pre-configured command descriptions.
     *
     * @russian
     * @brief Хранилище для предварительно настроенных описаний команд.
     */
    std::map<std::string, std::string> commandDescriptions_;

    /**
     * @english
     * @brief Records the names of commands whose help was requested via showCommandHelp.
     *
     * @russian
     * @brief Записывает имена команд, для которых была запрошена справка через showCommandHelp.
     */
    std::set<std::string> calledCommandHelp_;

    /**
     * @english
     * @brief The mock event bus used for optional debug output.
     *
     * @russian
     * @brief Mock шина событий, используемая для опционального отладочного вывода.
     */
    std::shared_ptr<MockSubject> mockEventBus_;

    /**
     * @english
     * @brief Tracks if general help was shown.
     *
     * @russian
     * @brief Отслеживает, была ли показана общая справка.
     */
    bool wasGeneralHelpCalled_ = false;

public:
    /**
     * @english
     * @brief Constructor for MockHelpService.
     * @param mockEventBus The shared pointer to the MockSubject used for notifications.
     *
     * @russian
     * @brief Конструктор для MockHelpService.
     * @param mockEventBus Общий указатель на MockSubject, используемый для уведомлений.
     */
    explicit MockHelpService(const std::shared_ptr<MockSubject>& mockEventBus)
        : mockEventBus_(mockEventBus) {
    }

    // --- Mock Methods Matching HelpService Interface ---

    /**
     * @english
     * @brief Returns the pre-configured list of available commands.
     * @return A vector of command names.
     *
     * @russian
     * @brief Возвращает предварительно настроенный список доступных команд.
     * @return Вектор имен команд.
     */
    [[nodiscard]] std::vector<std::string> getAvailableCommands() const {
        return availableCommands_;
    }

    /**
     * @english
     * @brief Returns the pre-configured description for a given command.
     * @param commandName The name of the command.
     * @return The configured description, or "No description available" if not set.
     *
     * @russian
     * @brief Возвращает предварительно настроенное описание для заданной команды.
     * @param commandName Имя команды.
     * @return Настроенное описание или "No description available", если не установлено.
     */
    [[nodiscard]] std::string getCommandDescription(const std::string& commandName) const {
        auto it = commandDescriptions_.find(commandName);
        if (it != commandDescriptions_.end()) {
            return it->second;
        }
        return "No description available";
    }

    /**
     * @english
     * @brief Records that help was requested for the given command.
     * @details Also emits a debug message to the mock event bus.
     * @param commandName The name of the command whose help is requested.
     *
     * @russian
     * @brief Записывает, что справка была запрошена для заданной команды.
     * @details Также отправляет отладочное сообщение в mock шину событий.
     * @param commandName Имя команды, для которой запрашивается справка.
     */
    void showCommandHelp(const std::string& commandName) {
        calledCommandHelp_.insert(commandName);
        if (mockEventBus_) {
            // Создаем объект Event правильно - используем поле details
            Event event;
            event.type = Event::DEBUG_MESSAGE;
            event.details = "MockHelpService: Showing help for " + commandName;
            event.source_name = "help";
            mockEventBus_->notify(event);
        }
    }

    /**
     * @english
     * @brief Records that general help was requested.
     * @details Also emits a debug message to the mock event bus.
     *
     * @russian
     * @brief Записывает, что была запрошена общая справка.
     * @details Также отправляет отладочное сообщение в mock шину событий.
     */
    void showGeneralHelp() {
        wasGeneralHelpCalled_ = true;
        if (mockEventBus_) {
            // Создаем объект Event правильно - используем поле details
            Event event;
            event.type = Event::DEBUG_MESSAGE;
            event.details = "MockHelpService: Showing general help";
            event.source_name = "help";
            mockEventBus_->notify(event);
        }
    }

    // --- Test Control Methods ---

    /**
     * @english
     * @brief Sets the list of command names to be returned by getAvailableCommands().
     * @param commands The vector of command names.
     *
     * @russian
     * @brief Устанавливает список имен команд, возвращаемых getAvailableCommands().
     * @param commands Вектор имен команд.
     */
    void setAvailableCommands(const std::vector<std::string>& commands) {
        availableCommands_ = commands;
    }

    /**
     * @english
     * @brief Configures a description for a specific command name.
     * @param commandName The name of the command.
     * @param description The description to return for that command.
     *
     * @russian
     * @brief Настраивает описание для конкретного имени команды.
     * @param commandName Имя команды.
     * @param description Описание для возврата для этой команды.
     */
    void setCommandDescription(const std::string& commandName, const std::string& description) {
        commandDescriptions_[commandName] = description;
    }

    /**
     * @english
     * @brief Checks if showCommandHelp() was called for a specific command.
     * @param commandName The name of the command to check.
     * @return true if showCommandHelp() was called with this name, false otherwise.
     *
     * @russian
     * @brief Проверяет, был ли вызван showCommandHelp() для конкретной команды.
     * @param commandName Имя команды для проверки.
     * @return true если showCommandHelp() был вызван с этим именем, false в противном случае.
     */
    [[nodiscard]] bool wasCommandHelpCalled(const std::string& commandName) const {
        return calledCommandHelp_.find(commandName) != calledCommandHelp_.end();
    }

    /**
     * @english
     * @brief Checks if showGeneralHelp() was called.
     * @return true if showGeneralHelp() was called, false otherwise.
     *
     * @russian
     * @brief Проверяет, был ли вызван showGeneralHelp().
     * @return true если showGeneralHelp() был вызван, false в противном случае.
     */
    [[nodiscard]] bool wasGeneralHelpCalled() const {
        return wasGeneralHelpCalled_;
    }

    /**
     * @english
     * @brief Resets the mock's internal state (called commands, lists, and descriptions).
     *
     * @russian
     * @brief Сбрасывает внутреннее состояние mock'а (вызванные команды, списки и описания).
     */
    void clear() {
        calledCommandHelp_.clear();
        availableCommands_.clear();
        commandDescriptions_.clear();
        wasGeneralHelpCalled_ = false;
    }
};

} // namespace svcs::test::cli::mocks