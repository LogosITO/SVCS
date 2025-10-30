/**
 * @file HelpCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the HelpCommand class for displaying command help information.
 *
 * @russian
 * @brief Объявление класса HelpCommand для отображения справочной информации о командах.
 */
#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../include/HelpService.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @english
 * @brief Command for displaying help information about other commands.
 * @details The **HelpCommand** is the application's built-in help utility. It can show a general
 * list of all available commands or display detailed usage and descriptions for a specific
 * command provided as an argument. It delegates the retrieval of command information
 * to the **HelpService** to maintain a clean separation of concerns.
 *
 * @russian
 * @brief Команда для отображения справочной информации о других командах.
 * @details **HelpCommand** является встроенной утилитой справки приложения. Она может показывать общий
 * список всех доступных команд или отображать подробное использование и описания для конкретной
 * команды, предоставленной в качестве аргумента. Она делегирует получение информации о командах
 * в **HelpService** для поддержания чистого разделения ответственности.
 */
class HelpCommand : public ICommand {
private:
    /**
     * @english
     * @brief Shared pointer to the event bus used for notifications (i.e., printing help messages to the user).
     *
     * @russian
     * @brief Общий указатель на шину событий, используемую для уведомлений (т.е. вывода справочных сообщений пользователю).
     */
    std::shared_ptr<ISubject> eventBus_;

    /**
     * @english
     * @brief Shared pointer to the service responsible for retrieving and formatting command help content.
     *
     * @russian
     * @brief Общий указатель на сервис, ответственный за получение и форматирование содержимого справки команд.
     */
    std::shared_ptr<HelpService> helpService_;

public:
    /**
     * @english
     * @brief Constructs the HelpCommand.
     * @param subject Shared pointer to the event bus for logging and output notifications.
     * @param helpService Shared pointer to the help service dependency.
     *
     * @russian
     * @brief Конструирует HelpCommand.
     * @param subject Общий указатель на шину событий для логирования и выходных уведомлений.
     * @param helpService Общий указатель на зависимость сервиса справки.
     */
    HelpCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<HelpService> helpService);

    /**
     * @english
     * @brief Executes the help command logic.
     * @param args Command arguments. If the vector is empty, general help is shown.
     * If it contains one command name, detailed help for that command is shown.
     * @return \c true always, as the help command is not expected to encounter a runtime failure.
     *
     * @russian
     * @brief Выполняет логику команды help.
     * @param args Аргументы команды. Если вектор пуст, показывается общая справка.
     * Если он содержит одно имя команды, показывается подробная справка для этой команды.
     * @return \c true всегда, так как команда help не ожидает возникновения ошибок выполнения.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return The string "help".
     *
     * @russian
     * @brief Получает имя команды.
     * @return Строка "help".
     */
    [[nodiscard]] std::string getName() const override { return "help"; }

    /**
     * @english
     * @brief Gets the brief description of the command.
     * @return The string "Show help information for commands".
     *
     * @russian
     * @brief Получает краткое описание команды.
     * @return Строка "Показать справочную информацию для команд".
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return The string "svcs help [command]".
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка "svcs help [command]".
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Shows detailed help information for this specific command via the event bus.
     *
     * @russian
     * @brief Показывает подробную справочную информацию для этой конкретной команды через шину событий.
     */
    void showHelp() const override;

private:
    /**
     * @english
     * @brief Shows general help, listing all available commands and their descriptions.
     * @details Utilizes the HelpService to retrieve the list of all registered commands.
     *
     * @russian
     * @brief Показывает общую справку, перечисляя все доступные команды и их описания.
     * @details Использует HelpService для получения списка всех зарегистрированных команд.
     */
    void showGeneralHelp() const;

    /**
     * @english
     * @brief Shows detailed usage, description, and explanation for a specific command.
     * @details Delegates the task of fetching and displaying the detailed help to the HelpService.
     * @param commandName The name of the command to show help for.
     *
     * @russian
     * @brief Показывает подробное использование, описание и объяснение для конкретной команды.
     * @details Делегирует задачу получения и отображения подробной справки в HelpService.
     * @param commandName Имя команды, для которой показывается справка.
     */
    void showCommandHelp(const std::string& commandName) const;
};