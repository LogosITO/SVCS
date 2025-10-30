/**
 * @file HelpService.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Service for providing help information about commands.
 *
 * @russian
 * @brief Сервис для предоставления справочной информации о командах.
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

// Forward declarations
class ISubject;
struct Event;

/**
 * @english
 * @brief Service that provides help information for commands.
 * @details This service implements the application logic for displaying help. It uses a set of
 * **callback functions** (`std::function`) to retrieve command data (names, descriptions,
 * usage) and display specific help. This design choice prevents a direct dependency
 * on the CommandFactory, thereby breaking a potential **circular dependency**.
 *
 * @russian
 * @brief Сервис, предоставляющий справочную информацию для команд.
 * @details Этот сервис реализует логику приложения для отображения справки. Он использует набор
 * **функций обратного вызова** (`std::function`) для получения данных команд (имена, описания,
 * использование) и отображения конкретной справки. Этот выбор дизайна предотвращает прямую зависимость
 * от CommandFactory, тем самым разрывая потенциальную **циклическую зависимость**.
 */
class HelpService {
private:
    /**
     * @english
     * @brief Shared pointer to the event bus used for logging or notifying the application.
     *
     * @russian
     * @brief Общий указатель на шину событий, используемую для логирования или уведомления приложения.
     */
    std::shared_ptr<ISubject> eventBus_;

    // Callback functions instead of direct CommandFactory dependency

    /**
     * @english
     * @brief Callback function to retrieve a list of all available command names.
     *
     * @russian
     * @brief Функция обратного вызова для получения списка всех доступных имен команд.
     */
    std::function<std::vector<std::string>()> getCommandsCallback_;

    /**
     * @english
     * @brief Callback function to retrieve the brief description of a specified command.
     *
     * @russian
     * @brief Функция обратного вызова для получения краткого описания указанной команды.
     */
    std::function<std::string(const std::string&)> getDescriptionCallback_;

    /**
     * @english
     * @brief Callback function to execute the full help output logic for a specified command.
     *
     * @russian
     * @brief Функция обратного вызова для выполнения полной логики вывода справки для указанной команды.
     */
    std::function<void(const std::string&)> showHelpCallback_;

    /**
     * @english
     * @brief Callback function to retrieve the usage syntax for a specified command.
     *
     * @russian
     * @brief Функция обратного вызова для получения синтаксиса использования для указанной команды.
     */
    std::function<std::string(const std::string&)> getUsageCallback_;

public:
    /**
     * @english
     * @brief Constructs the HelpService by injecting necessary dependencies as callbacks.
     * @param bus Shared pointer to the event bus for notifications.
     * @param getCommands Function (callback) that returns a list of available command names.
     * @param getDescription Function (callback) that returns the brief description of a command.
     * @param showHelp Function (callback) that triggers the full help display for a command.
     * @param getUsage Function (callback) that returns the usage syntax of a command (defaults to nullptr if not strictly needed).
     *
     * @russian
     * @brief Конструирует HelpService путем внедрения необходимых зависимостей в виде обратных вызовов.
     * @param bus Общий указатель на шину событий для уведомлений.
     * @param getCommands Функция (обратный вызов), которая возвращает список доступных имен команд.
     * @param getDescription Функция (обратный вызов), которая возвращает краткое описание команды.
     * @param showHelp Функция (обратный вызов), которая запускает полное отображение справки для команды.
     * @param getUsage Функция (обратный вызов), которая возвращает синтаксис использования команды (по умолчанию nullptr, если не требуется строго).
     */
    HelpService(std::shared_ptr<ISubject> bus,
                std::function<std::vector<std::string>()> getCommands,
                std::function<std::string(const std::string&)> getDescription,
                std::function<void(const std::string&)> showHelp,
                std::function<std::string(const std::string&)> getUsage = nullptr);

    /**
     * @english
     * @brief Executes the `getCommandsCallback_` to retrieve the list of all available command names.
     * @return Vector of strings containing the names of all registered commands.
     *
     * @russian
     * @brief Выполняет `getCommandsCallback_` для получения списка всех доступных имен команд.
     * @return Вектор строк, содержащий имена всех зарегистрированных команд.
     */
    [[nodiscard]] std::vector<std::string> getAvailableCommands() const;

    /**
     * @english
     * @brief Executes the `getDescriptionCallback_` to retrieve the brief description of a specific command.
     * @param commandName The name of the command.
     * @return The command description, or "Unknown command" if the command name is not valid or the callback fails.
     *
     * @russian
     * @brief Выполняет `getDescriptionCallback_` для получения краткого описания конкретной команды.
     * @param commandName Имя команды.
     * @return Описание команды или "Неизвестная команда", если имя команды недействительно или обратный вызов не сработал.
     */
    [[nodiscard]] std::string getCommandDescription(const std::string& commandName) const;

    /**
     * @english
     * @brief Executes the `showHelpCallback_` to trigger the display of detailed help for a specific command.
     * @param commandName The name of the command to show help for.
     *
     * @russian
     * @brief Выполняет `showHelpCallback_` для запуска отображения подробной справки для конкретной команды.
     * @param commandName Имя команды, для которой показывается справка.
     */
    void showCommandHelp(const std::string& commandName) const;

    /**
     * @english
     * @brief Checks for the existence of a command by comparing against the list returned by `getAvailableCommands`.
     * @param commandName The name of the command to check.
     * @return \c true if the command is registered, \c false otherwise.
     *
     * @russian
     * @brief Проверяет существование команды путем сравнения со списком, возвращенным `getAvailableCommands`.
     * @param commandName Имя команды для проверки.
     * @return \c true если команда зарегистрирована, \c false в противном случае.
     */
    [[nodiscard]] bool commandExists(const std::string& commandName) const;

    /**
     * @english
     * @brief Executes the `getUsageCallback_` to retrieve the usage syntax of a specific command.
     * @param commandName The name of the command.
     * @return The command usage string, or an empty string if the command is not found or the callback is not set.
     *
     * @russian
     * @brief Выполняет `getUsageCallback_` для получения синтаксиса использования конкретной команды.
     * @param commandName Имя команды.
     * @return Строка использования команды или пустая строка, если команда не найдена или обратный вызов не установлен.
     */
    [[nodiscard]] std::string getCommandUsage(const std::string& commandName) const;
};