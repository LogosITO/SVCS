/**
 * @file CommandFactory.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the CommandFactory class, which implements the Factory pattern for commands.
 *
 * @russian
 * @brief Объявление класса CommandFactory, который реализует фабричный шаблон для команд.
 */
#pragma once

#include "../include/ICommand.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

/**
 * @english
 * @brief Factory for creating ICommand objects by their string name.
 * @details The CommandFactory manages the lifecycle of commands, creating them dynamically
 * using creator functions registered under unique names. Commands are constructed
 * with shared ownership of an event bus (@ref ISubject) and a repository manager
 * (@ref RepositoryManager).
 *
 * @russian
 * @brief Фабрика для создания объектов ICommand по их строковому имени.
 * @details CommandFactory управляет жизненным циклом команд, создавая их динамически
 * с использованием функций-создателей, зарегистрированных под уникальными именами. Команды конструируются
 * с общим владением шиной событий (@ref ISubject) и менеджером репозитория
 * (@ref RepositoryManager).
 */
class CommandFactory {
private:
    /**
     * @english
     * @brief Shared pointer to the event bus used for notifications (e.g., errors or info).
     *
     * @russian
     * @brief Общий указатель на шину событий, используемую для уведомлений (например, ошибки или информация).
     */
    std::shared_ptr<ISubject> event_bus;

    /**
     * @english
     * @brief Shared pointer to the repository manager used for executing SVCS operations.
     *
     * @russian
     * @brief Общий указатель на менеджер репозитория, используемый для выполнения операций SVCS.
     */
    std::shared_ptr<RepositoryManager> repo_manager;

    /**
     * @english
     * @brief Map of command creator functions.
     * @details Key: The command name (e.g., "add", "commit").
     * Value: A std::function that accepts an ISubject and a RepositoryManager
     * and returns a unique pointer to a concrete ICommand implementation.
     *
     * @russian
     * @brief Карта функций-создателей команд.
     * @details Ключ: Имя команды (например, "add", "commit").
     * Значение: std::function, которая принимает ISubject и RepositoryManager
     * и возвращает уникальный указатель на конкретную реализацию ICommand.
     */
    std::unordered_map<std::string,
        std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                              std::shared_ptr<RepositoryManager>)>> creators;

    /**
     * @english
     * @brief Registers the set of commands that should be available by default.
     * @details This function is typically called in the constructor to pre-configure the Factory.
     *
     * @russian
     * @brief Регистрирует набор команд, которые должны быть доступны по умолчанию.
     * @details Эта функция обычно вызывается в конструкторе для предварительной настройки Фабрики.
     */
    void registerDefaultCommands();

public:
    /**
     * @english
     * @brief Constructs the CommandFactory.
     * @param bus A shared pointer to the event bus.
     * @param repoManager A shared pointer to the repository manager.
     *
     * @russian
     * @brief Конструирует CommandFactory.
     * @param bus Общий указатель на шину событий.
     * @param repoManager Общий указатель на менеджер репозитория.
     */
    explicit CommandFactory(std::shared_ptr<ISubject> bus,
                          std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Deletes the copy constructor. The Factory should not be copied.
     *
     * @russian
     * @brief Удаляет конструктор копирования. Фабрика не должна копироваться.
     */
    CommandFactory(const CommandFactory&) = delete;

    /**
     * @english
     * @brief Deletes the assignment operator. The Factory should not be assigned.
     *
     * @russian
     * @brief Удаляет оператор присваивания. Фабрика не должна присваиваться.
     */
    CommandFactory& operator=(const CommandFactory&) = delete;

    /**
     * @english
     * @brief Registers a creator function for a new command.
     * @param name The string name under which the command will be callable.
     * @param creator The function that creates and returns a new ICommand object.
     *
     * @russian
     * @brief Регистрирует функцию-создатель для новой команды.
     * @param name Строковое имя, под которым команда будет вызываться.
     * @param creator Функция, которая создает и возвращает новый объект ICommand.
     */
    void registerCommand(const std::string& name,
                       std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                                             std::shared_ptr<RepositoryManager>)> creator);

    /**
     * @english
     * @brief Retrieves the brief description for a registered command.
     * @details To do this, it must create a temporary instance of the command.
     * @param name The name of the command.
     * @return The command's description string, or an error message if the command is not registered.
     *
     * @russian
     * @brief Получает краткое описание для зарегистрированной команды.
     * @details Для этого необходимо создать временный экземпляр команды.
     * @param name Имя команды.
     * @return Строка описания команды или сообщение об ошибке, если команда не зарегистрирована.
     */
    std::string getCommandDescription(const std::string& name) const;

    /**
     * @english
     * @brief Displays the detailed help and usage for a registered command.
     * @details This is accomplished by creating a temporary command instance and calling its showHelp method.
     * @param name The name of the command.
     *
     * @russian
     * @brief Отображает подробную справку и использование для зарегистрированной команды.
     * @details Это достигается созданием временного экземпляра команды и вызовом его метода showHelp.
     * @param name Имя команды.
     */
    void showCommandHelp(const std::string& name) const;

    /**
     * @english
     * @brief Creates and returns a command object by its name.
     * @details Transfers ownership (via unique_ptr) of the created object to the caller.
     * @param name The name of the command (the key in the creators map).
     * @return std::unique_ptr<ICommand> holding a new command object, or nullptr if the command is not registered.
     *
     * @russian
     * @brief Создает и возвращает объект команды по его имени.
     * @details Передает владение (через unique_ptr) созданного объекта вызывающей стороне.
     * @param name Имя команды (ключ в карте creators).
     * @return std::unique_ptr<ICommand>, содержащий новый объект команды, или nullptr если команда не зарегистрирована.
     */
    std::unique_ptr<ICommand> createCommand(const std::string& name) const;

    /**
     * @english
     * @brief Retrieves a list of names for all currently registered commands.
     * @return A vector of strings containing the names of all registered commands.
     *
     * @russian
     * @brief Получает список имен всех текущих зарегистрированных команд.
     * @return Вектор строк, содержащий имена всех зарегистрированных команд.
     */
    std::vector<std::string> getRegisteredCommands() const;
};