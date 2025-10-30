/**
 * @file ICommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the ICommand interface for all executable CLI commands.
 * @details ICommand defines the basic contract that any command in the system must conform to,
 * ensuring a uniform way to retrieve metadata (name, description, usage)
 * and execute the command's logic.
 *
 * @russian
 * @brief Объявление интерфейса ICommand для всех исполняемых CLI команд.
 * @details ICommand определяет базовый контракт, которому должна соответствовать любая команда в системе,
 * обеспечивая единообразный способ получения метаданных (имя, описание, использование)
 * и выполнения логики команды.
 */
#pragma once

#include <string>
#include <vector>

/**
 * @english
 * @class ICommand
 * @brief Interface (abstract base class) for all commands executed via the CLI.
 * @details Every concrete command (e.g., InitCommand, CommitCommand) must inherit
 * this interface and implement all its pure virtual methods.
 *
 * @russian
 * @class ICommand
 * @brief Интерфейс (абстрактный базовый класс) для всех команд, выполняемых через CLI.
 * @details Каждая конкретная команда (например, InitCommand, CommitCommand) должна наследовать
 * этот интерфейс и реализовывать все его чисто виртуальные методы.
 */
class ICommand {
public:
    /**
     * @english
     * @brief Virtual destructor.
     * @details Ensures correct cleanup of derived classes via a base class pointer.
     *
     * @russian
     * @brief Виртуальный деструктор.
     * @details Обеспечивает корректную очистку производных классов через указатель на базовый класс.
     */
    virtual ~ICommand() = default;

    /**
     * @english
     * @brief Retrieves the unique name of the command (e.g., "init").
     * @return The command name as a string.
     *
     * @russian
     * @brief Получает уникальное имя команды (например, "init").
     * @return Имя команды в виде строки.
     */
    [[nodiscard]] virtual std::string getName() const = 0;

    /**
     * @english
     * @brief Retrieves a brief description of what the command does.
     * @return The command's description.
     *
     * @russian
     * @brief Получает краткое описание того, что делает команда.
     * @return Описание команды.
     */
    [[nodiscard]] virtual std::string getDescription() const = 0;

    /**
     * @english
     * @brief Retrieves the command's usage syntax.
     * @return A string with the syntax (e.g., "vcs commit <message>").
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка с синтаксисом (например, "vcs commit <message>").
     */
    [[nodiscard]] virtual std::string getUsage() const = 0;

    /**
     * @english
     * @brief Executes the main logic of the command.
     * @param args A vector of string arguments passed to the command (excluding the command name itself).
     * @return true if the command execution was successful; false on error.
     *
     * @russian
     * @brief Выполняет основную логику команды.
     * @param args Вектор строковых аргументов, переданных команде (исключая само имя команды).
     * @return true если выполнение команды было успешным; false при ошибке.
     */
    virtual bool execute(const std::vector<std::string>& args) = 0;

    /**
     * @english
     * @brief Displays detailed help information about the command (Usage and Description).
     *
     * @russian
     * @brief Отображает подробную справочную информацию о команде (использование и описание).
     */
    virtual void showHelp() const = 0;
};