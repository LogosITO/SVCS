/**
 * @file RepoCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the RepoCommand class.
 * @details RepoCommand implements the ICommand interface to manage repository-level
 * configurations, primarily focusing on managing **remotes**.
 * This command acts as a facade, delegating the actual management of remotes
 * to the @ref RemoteManager class after ensuring the command is executed within
 * a valid repository context (implicitly handled by @ref RepositoryManager).
 * The command supports subcommands like `add`, `remove`, `list`, and `rename`.
 * Inherits from ServerBaseCommand for common server command functionality.
 *
 * @russian
 * @brief Объявление класса RepoCommand.
 * @details RepoCommand реализует интерфейс ICommand для управления конфигурациями
 * на уровне репозитория, в основном сосредотачиваясь на управлении **удаленными репозиториями**.
 * Эта команда действует как фасад, делегируя фактическое управление удаленными репозиториями
 * классу @ref RemoteManager после обеспечения выполнения команды в контексте
 * валидного репозитория (неявно обрабатывается @ref RepositoryManager).
 * Команда поддерживает подкоманды такие как `add`, `remove`, `list` и `rename`.
 * Наследуется от ServerBaseCommand для общей функциональности серверных команд.
 */
#pragma once

#include "ServerBaseCommand.hxx"
#include "../../core/include/Repository.hxx"
#include "RemoteManager.hxx"
#include <memory>
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

/**
 * @english
 * @class RepoCommand
 * @brief Command handler for managing repository remotes and configurations.
 * @details Implements the "svcs repo" command, providing subcommands for
 * managing the list of remote repositories associated with the current project.
 * Inherits from ServerBaseCommand for common server command functionality.
 *
 * @russian
 * @class RepoCommand
 * @brief Обработчик команд для управления удаленными репозиториями и конфигурациями.
 * @details Реализует команду "svcs repo", предоставляя подкоманды для
 * управления списком удаленных репозиториев, связанных с текущим проектом.
 * Наследуется от ServerBaseCommand для общей функциональности серверных команд.
 */
class RepoCommand : public ServerBaseCommand {
public:
    /**
     * @english
     * @brief Constructs a RepoCommand.
     * @param event_bus The application's event bus.
     * @param repo_manager The repository locator and manager.
     *
     * @russian
     * @brief Конструирует RepoCommand.
     * @param event_bus Шина событий приложения.
     * @param repo_manager Локатор и менеджер репозитория.
     */
    RepoCommand(std::shared_ptr<ISubject> event_bus,
                std::shared_ptr<RepositoryManager> repo_manager);

    /**
     * @english
     * @brief Gets the name of the command.
     * @return The command name "repo".
     *
     * @russian
     * @brief Получает имя команды.
     * @return Имя команды "repo".
     */
    std::string getName() const override;

    /**
     * @english
     * @brief Gets a brief description of the command.
     * @return Description of the command's purpose.
     *
     * @russian
     * @brief Получает краткое описание команды.
     * @return Описание назначения команды.
     */
    std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return The usage syntax string.
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка синтаксиса использования.
     */
    std::string getUsage() const override;

    /**
     * @english
     * @brief Executes the RepoCommand logic.
     * @param args The list of arguments, where args[0] is expected to be a subcommand (e.g., "add", "remove").
     * @return bool True if the command executes successfully, false otherwise.
     * @details Finds the current repository and delegates the task based on the subcommand.
     *
     * @russian
     * @brief Выполняет логику RepoCommand.
     * @param args Список аргументов, где args[0] ожидается как подкоманда (например, "add", "remove").
     * @return bool True если команда выполнена успешно, false в противном случае.
     * @details Находит текущий репозиторий и делегирует задачу на основе подкоманды.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Displays help information for the command.
     *
     * @russian
     * @brief Отображает справочную информацию для команды.
     */
    void showHelp() const override;

private:
    // --- Subcommand Handlers ---

    /**
     * @english
     * @brief Handles the "repo add" subcommand.
     * @param remote_manager The manager instance for remotes in the current repo.
     * @param args Arguments containing the remote name and URL. Expected format: `{"add", <name>, <url>}`.
     * @return bool True on successful addition, false otherwise.
     *
     * @russian
     * @brief Обрабатывает подкоманду "repo add".
     * @param remote_manager Экземпляр менеджера для удаленных репозиториев в текущем репозитории.
     * @param args Аргументы, содержащие имя удаленного репозитория и URL. Ожидаемый формат: `{"add", <name>, <url>}`.
     * @return bool True при успешном добавлении, false в противном случае.
     */
    bool handleAdd(RemoteManager& remote_manager, const std::vector<std::string>& args);

    /**
     * @english
     * @brief Handles the "repo remove" subcommand.
     * @param remote_manager The manager instance for remotes.
     * @param args Arguments containing the remote name. Expected format: `{"remove", <name>}`.
     * @return bool True on successful removal, false otherwise.
     *
     * @russian
     * @brief Обрабатывает подкоманду "repo remove".
     * @param remote_manager Экземпляр менеджера для удаленных репозиториев.
     * @param args Аргументы, содержащие имя удаленного репозитория. Ожидаемый формат: `{"remove", <name>}`.
     * @return bool True при успешном удалении, false в противном случае.
     */
    bool handleRemove(RemoteManager& remote_manager, const std::vector<std::string>& args);

    /**
     * @english
     * @brief Handles the "repo list" subcommand.
     * @param remote_manager The manager instance for remotes.
     * @return bool Always true, lists all configured remotes.
     *
     * @russian
     * @brief Обрабатывает подкоманду "repo list".
     * @param remote_manager Экземпляр менеджера для удаленных репозиториев.
     * @return bool Всегда true, перечисляет все настроенные удаленные репозитории.
     */
    bool handleList(RemoteManager& remote_manager);

    /**
     * @english
     * @brief Handles the "repo rename" subcommand.
     * @param remote_manager The manager instance for remotes.
     * @param args Arguments containing the old and new remote name. Expected format: `{"rename", <old_name>, <new_name>}`.
     * @return bool True on successful rename, false otherwise.
     * @note Renaming a remote requires removing the old one and adding a new one,
     * potentially updating refspecs if they were manually configured.
     *
     * @russian
     * @brief Обрабатывает подкоманду "repo rename".
     * @param remote_manager Экземпляр менеджера для удаленных репозиториев.
     * @param args Аргументы, содержащие старое и новое имя удаленного репозитория. Ожидаемый формат: `{"rename", <old_name>, <new_name>}`.
     * @return bool True при успешном переименовании, false в противном случае.
     * @note Переименование удаленного репозитория требует удаления старого и добавления нового,
     * потенциально обновляя refspecs, если они были настроены вручную.
     */
    bool handleRename(RemoteManager& remote_manager, const std::vector<std::string>& args);
};

}