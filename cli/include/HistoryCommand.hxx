/**
 * @file HistoryCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the HistoryCommand class for viewing save history (commits).
 *
 * @russian
 * @brief Объявление класса HistoryCommand для просмотра истории сохранений (коммитов).
 */
#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @english
 * @brief Command for viewing the history of saves (commits) in the repository.
 * @details The **HistoryCommand** displays the chronological history of all saves
 * with their messages, authors, timestamps, and unique identifiers. It supports
 * different output formats, including default, one-line, and detailed views.
 *
 * @russian
 * @brief Команда для просмотра истории сохранений (коммитов) в репозитории.
 * @details **HistoryCommand** отображает хронологическую историю всех сохранений
 * с их сообщениями, авторами, временными метками и уникальными идентификаторами. Он поддерживает
 * различные форматы вывода, включая стандартный, однострочный и подробный виды.
 */
class HistoryCommand : public ICommand {
private:
    /**
     * @english
     * @brief Shared pointer to the event bus for user notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий для уведомлений пользователя.
     */
    std::shared_ptr<ISubject> eventBus_;

    /**
     * @english
     * @brief Shared pointer to the repository manager for accessing commit data.
     *
     * @russian
     * @brief Общий указатель на менеджер репозитория для доступа к данным коммитов.
     */
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @english
     * @brief Constructor for HistoryCommand.
     * @param subject A shared pointer to the ISubject (event bus).
     * @param repoManager A shared pointer to the RepositoryManager.
     *
     * @russian
     * @brief Конструктор для HistoryCommand.
     * @param subject Общий указатель на ISubject (шину событий).
     * @param repoManager Общий указатель на RepositoryManager.
     */
    HistoryCommand(std::shared_ptr<ISubject> subject,
                   std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the "history" command with the given arguments.
     * @details This method is the main entry point for the command, handling argument parsing
     * and delegating to the appropriate history display function.
     * @param args The vector of string arguments for the command (e.g., limit, format options).
     * @return true if the command executed successfully, false otherwise.
     *
     * @russian
     * @brief Выполняет команду "history" с заданными аргументами.
     * @details Этот метод является основной точкой входа для команды, обрабатывая разбор аргументов
     * и делегируя соответствующей функции отображения истории.
     * @param args Вектор строковых аргументов для команды (например, лимит, опции формата).
     * @return true если команда выполнена успешно, false в противном случае.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return The command name, "history".
     *
     * @russian
     * @brief Получает имя команды.
     * @return Имя команды, "history".
     */
    [[nodiscard]] std::string getName() const override { return "history"; }

    /**
     * @english
     * @brief Gets a brief description of the command.
     * @return A string describing the command's purpose.
     *
     * @russian
     * @brief Получает краткое описание команды.
     * @return Строка, описывающая назначение команды.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the command's usage syntax.
     * @return A string showing how to use the command.
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка, показывающая как использовать команду.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Displays the detailed help information for the command.
     *
     * @russian
     * @brief Отображает подробную справочную информацию для команды.
     */
    void showHelp() const override;

private:
    /**
     * @english
     * @brief Parses the command line arguments to determine display options.
     * @param args The command line arguments.
     * @param showOneline Output parameter: true to show history in one line format.
     * @param limit Output parameter: the maximum number of commits to display (0 for no limit).
     * @param showFull Output parameter: true to show history in detailed/full format.
     * @return true if arguments were parsed successfully, false otherwise.
     *
     * @russian
     * @brief Разбирает аргументы командной строки для определения опций отображения.
     * @param args Аргументы командной строки.
     * @param showOneline Выходной параметр: true для показа истории в однострочном формате.
     * @param limit Выходной параметр: максимальное количество коммитов для отображения (0 без ограничений).
     * @param showFull Выходной параметр: true для показа истории в подробном/полном формате.
     * @return true если аргументы успешно разобраны, false в противном случае.
     */
    bool parseArguments(const std::vector<std::string>& args,
                       bool& showOneline, int& limit, bool& showFull) const;

    /**
     * @english
     * @brief Displays the commit history using the default format.
     * @param entries A vector of CommitInfo objects to display.
     *
     * @russian
     * @brief Отображает историю коммитов с использованием стандартного формата.
     * @param entries Вектор объектов CommitInfo для отображения.
     */
    void showDefaultHistory(const std::vector<CommitInfo>& entries) const;

    /**
     * @english
     * @brief Displays the commit history using the concise one-line format.
     * @param entries A vector of CommitInfo objects to display.
     *
     * @russian
     * @brief Отображает историю коммитов с использованием краткого однострочного формата.
     * @param entries Вектор объектов CommitInfo для отображения.
     */
    void showOnelineHistory(const std::vector<CommitInfo>& entries) const;

    /**
     * @english
     * @brief Displays the commit history using the detailed/full format.
     * @param entries A vector of CommitInfo objects to display.
     *
     * @russian
     * @brief Отображает историю коммитов с использованием подробного/полного формата.
     * @param entries Вектор объектов CommitInfo для отображения.
     */
    void showDetailedHistory(const std::vector<CommitInfo>& entries) const;

    /**
     * @english
     * @brief Formats a raw timestamp string into a user-friendly readable format.
     * @param timestamp The raw timestamp string (e.g., ISO 8601).
     * @return The formatted timestamp string.
     *
     * @russian
     * @brief Форматирует сырую строку временной метки в удобочитаемый формат.
     * @param timestamp Сырая строка временной метки (например, ISO 8601).
     * @return Отформатированная строка временной метки.
     */
    [[nodiscard]] static std::string formatTimestamp(const std::string& timestamp);

    /**
     * @english
     * @brief Truncates a string to a specified maximum length, adding an ellipsis if truncated.
     * @param str The string to truncate.
     * @param length The maximum desired length.
     * @return The truncated string.
     *
     * @russian
     * @brief Обрезает строку до указанной максимальной длины, добавляя многоточие при обрезке.
     * @param str Строка для обрезки.
     * @param length Желаемая максимальная длина.
     * @return Обрезанная строка.
     */
    [[nodiscard]] static std::string truncateString(const std::string& str, size_t length);
};