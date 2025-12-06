/**
 * @file ServerCommandFactory.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the ServerCommandFactory class for server administration commands.
 * @details Implements the Factory pattern specifically for server management commands.
 * Creates commands for hub management, user administration, and repository access control.
 *
 * @russian
 * @brief Объявление класса ServerCommandFactory для команд администрирования сервера.
 * @details Реализует фабричный шаблон специально для команд управления сервером.
 * Создает команды для управления хабами, администрирования пользователей и контроля доступа к репозиториям.
 */
#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

/**
 * @english
 * @namespace svcs::server::cli
 * @brief Server administration command-line interface components.
 * @details Contains CLI commands specifically for server administration and management,
 * including hub repository creation, user management, and server configuration.
 * These commands are used by administrators to set up and maintain SVCS server instances.
 * This namespace bridges the server functionality with the command-line interface.
 *
 * @russian
 * @namespace svcs::server::cli
 * @brief Компоненты командной строки для администрирования сервера.
 * @details Содержит CLI команды, специально предназначенные для администрирования и управления сервером,
 * включая создание репозиториев-хабов, управление пользователями и конфигурацию сервера.
 * Эти команды используются администраторами для настройки и обслуживания экземпляров сервера SVCS.
 * Это пространство имен связывает серверную функциональность с интерфейсом командной строки.
 */
namespace svcs::server::cli {

using namespace svcs::core;
using namespace svcs::cli;
using namespace svcs::services;

/**
 * @english
 * @brief Factory for creating server administration ICommand objects.
 * @details The ServerCommandFactory manages the lifecycle of server-specific commands,
 * creating them dynamically using creator functions registered under unique names.
 * Commands are constructed with shared ownership of an event bus (@ref ISubject)
 * and a repository manager (@ref RepositoryManager). This factory is specifically
 * designed for server administration commands that require repository management.
 *
 * @russian
 * @brief Фабрика для создания объектов ICommand администрирования сервера.
 * @details ServerCommandFactory управляет жизненным циклом серверных команд,
 * создавая их динамически с использованием функций-создателей, зарегистрированных
 * под уникальными именами. Команды конструируются с общим владением шиной событий
 * (@ref ISubject) и менеджером репозитория (@ref RepositoryManager). Эта фабрика
 * специально разработана для команд администрирования сервера, требующих управления репозиториями.
 */
class ServerCommandFactory {
private:
    /**
     * @english
     * @brief Shared pointer to the event bus used for server command notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий, используемую для уведомлений серверных команд.
     */
    std::shared_ptr<ISubject> event_bus;

    /**
     * @english
     * @brief Shared pointer to the repository manager used for server repository operations.
     *
     * @russian
     * @brief Общий указатель на менеджер репозитория, используемый для серверных операций с репозиториями.
     */
    std::shared_ptr<RepositoryManager> repo_manager;

    /**
     * @english
     * @brief Map of server command creator functions.
     * @details Key: The server command name (e.g., "hub", "repo").
     * Value: A std::function that accepts an ISubject and a RepositoryManager
     * and returns a unique pointer to a concrete ICommand implementation.
     *
     * @russian
     * @brief Карта функций-создателей серверных команд.
     * @details Ключ: Имя серверной команды (например, "hub", "repo").
     * Значение: std::function, которая принимает ISubject и RepositoryManager
     * и возвращает уникальный указатель на конкретную реализацию ICommand.
     */
    std::unordered_map<std::string,
        std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                              std::shared_ptr<RepositoryManager>)>> creators;

    /**
     * @english
     * @brief Registers the set of server commands that should be available by default.
     * @details This function is called in the constructor to pre-configure the Factory
     * with all available server administration commands.
     *
     * @russian
     * @brief Регистрирует набор серверных команд, которые должны быть доступны по умолчанию.
     * @details Эта функция вызывается в конструкторе для предварительной настройки Фабрики
     * всеми доступными командами администрирования сервера.
     */
    void registerDefaultCommands();

public:
    /**
     * @english
     * @brief Constructs the ServerCommandFactory.
     * @param bus A shared pointer to the event bus.
     * @param repoManager A shared pointer to the repository manager.
     *
     * @russian
     * @brief Конструирует ServerCommandFactory.
     * @param bus Общий указатель на шину событий.
     * @param repoManager Общий указатель на менеджер репозитория.
     */
    explicit ServerCommandFactory(std::shared_ptr<ISubject> bus,
                                std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Deletes the copy constructor. The Server Factory should not be copied.
     *
     * @russian
     * @brief Удаляет конструктор копирования. Серверная Фабрика не должна копироваться.
     */
    ServerCommandFactory(const ServerCommandFactory&) = delete;

    /**
     * @english
     * @brief Deletes the assignment operator. The Server Factory should not be assigned.
     *
     * @russian
     * @brief Удаляет оператор присваивания. Серверная Фабрика не должна присваиваться.
     */
    ServerCommandFactory& operator=(const ServerCommandFactory&) = delete;

    /**
     * @english
     * @brief Registers a creator function for a new server command.
     * @param name The string name under which the server command will be callable.
     * @param creator The function that creates and returns a new ICommand object.
     *
     * @russian
     * @brief Регистрирует функцию-создатель для новой серверной команды.
     * @param name Строковое имя, под которым серверная команда будет вызываться.
     * @param creator Функция, которая создает и возвращает новый объект ICommand.
     */
    void registerCommand(const std::string& name,
                       std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                                             std::shared_ptr<RepositoryManager>)> creator);

    /**
     * @english
     * @brief Retrieves the brief description for a registered server command.
     * @details Creates a temporary instance of the command to retrieve its description.
     * @param name The name of the server command.
     * @return The command's description string, or an error message if the command is not registered.
     *
     * @russian
     * @brief Получает краткое описание для зарегистрированной серверной команды.
     * @details Создает временный экземпляр команды для получения ее описания.
     * @param name Имя серверной команды.
     * @return Строка описания команды или сообщение об ошибке, если команда не зарегистрирована.
     */
    std::string getCommandDescription(const std::string& name) const;

    /**
     * @english
     * @brief Displays the detailed help and usage for a registered server command.
     * @details Creates a temporary command instance and calls its showHelp method.
     * @param name The name of the server command.
     *
     * @russian
     * @brief Отображает подробную справку и использование для зарегистрированной серверной команды.
     * @details Создает временный экземпляр команды и вызывает его метод showHelp.
     * @param name Имя серверной команды.
     */
    void showCommandHelp(const std::string& name) const;

    /**
     * @english
     * @brief Creates and returns a server command object by its name.
     * @details Transfers ownership (via unique_ptr) of the created object to the caller.
     * @param name The name of the server command (the key in the creators map).
     * @return std::unique_ptr<ICommand> holding a new command object, or nullptr if the command is not registered.
     *
     * @russian
     * @brief Создает и возвращает объект серверной команды по его имени.
     * @details Передает владение (через unique_ptr) созданного объекта вызывающей стороне.
     * @param name Имя серверной команды (ключ в карте creators).
     * @return std::unique_ptr<ICommand>, содержащий новый объект команды, или nullptr если команда не зарегистрирована.
     */
    std::unique_ptr<ICommand> createCommand(const std::string& name) const;

    /**
     * @english
     * @brief Retrieves a list of names for all currently registered server commands.
     * @return A vector of strings containing the names of all registered server commands.
     *
     * @russian
     * @brief Получает список имен всех текущих зарегистрированных серверных команд.
     * @return Вектор строк, содержащий имена всех зарегистрированных серверных команд.
     */
    std::vector<std::string> getRegisteredCommands() const;

    /**
     * @english
     * @brief Checks if a server command with the given name exists.
     * @param name The name of the server command to check.
     * @return True if the command exists and can be created, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли серверная команда с заданным именем.
     * @param name Имя серверной команды для проверки.
     * @return True если команда существует и может быть создана, false в противном случае.
     */
    bool commandExists(const std::string& name) const;
};

}