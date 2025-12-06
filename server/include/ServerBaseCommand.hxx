/**
 * @file ServerBaseCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Base class for server administration commands using ICommand interface.
 * @details Provides common functionality for server management commands
 * with event bus and repository manager support. Serves as the foundation
 * for all server-specific administrative commands.
 *
 * @russian
 * @brief Базовый класс для команд администрирования сервера с использованием интерфейса ICommand.
 * @details Предоставляет общую функциональность для команд управления сервером
 * с поддержкой шины событий и менеджера репозиториев. Служит основой
 * для всех серверных административных команд.
 */

#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../services/Event.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <string>

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
 * @class ServerBaseCommand
 * @brief Abstract base class for all server administration commands.
 * @details Inherits from ICommand and provides common infrastructure
 * for server management operations including event notification and
 * repository management. All server-specific commands should derive
 * from this class.
 *
 * @russian
 * @class ServerBaseCommand
 * @brief Абстрактный базовый класс для всех команд администрирования сервера.
 * @details Наследуется от ICommand и предоставляет общую инфраструктуру
 * для операций управления сервером, включая уведомления о событиях и
 * управление репозиториями. Все серверные команды должны наследоваться
 * от этого класса.
 */
class ServerBaseCommand : public ICommand {
public:
    /**
     * @english
     * @brief Constructs a ServerBaseCommand with necessary dependencies.
     * @param event_bus Shared pointer to the event bus for system notifications.
     * @param repo_manager Shared pointer to the repository manager for repository operations.
     *
     * @russian
     * @brief Конструирует ServerBaseCommand с необходимыми зависимостями.
     * @param event_bus Общий указатель на шину событий для системных уведомлений.
     * @param repo_manager Общий указатель на менеджер репозиториев для операций с репозиториями.
     */
    ServerBaseCommand(std::shared_ptr<ISubject> event_bus,
                      std::shared_ptr<RepositoryManager> repo_manager);

    /**
     * @english
     * @brief Virtual destructor for proper cleanup of derived classes.
     *
     * @russian
     * @brief Виртуальный деструктор для корректной очистки производных классов.
     */
    virtual ~ServerBaseCommand() = default;

    /**
     * @english
     * @brief Pure virtual method to get the command name.
     * @return Command name as string.
     * @details Must be implemented by derived classes to provide unique command identifier.
     *
     * @russian
     * @brief Чисто виртуальный метод для получения имени команды.
     * @return Имя команды в виде строки.
     * @details Должен быть реализован производными классами для предоставления уникального идентификатора команды.
     */
    [[nodiscard]] virtual std::string getName() const override = 0;

    /**
     * @english
     * @brief Pure virtual method to get the command description.
     * @return Command description as string.
     * @details Must be implemented by derived classes to describe command functionality.
     *
     * @russian
     * @brief Чисто виртуальный метод для получения описания команды.
     * @return Описание команды в виде строки.
     * @details Должен быть реализован производными классами для описания функциональности команды.
     */
    [[nodiscard]] virtual std::string getDescription() const override = 0;

    /**
     * @english
     * @brief Pure virtual method to get the command usage syntax.
     * @return Command usage syntax as string.
     * @details Must be implemented by derived classes to show proper command usage.
     *
     * @russian
     * @brief Чисто виртуальный метод для получения синтаксиса использования команды.
     * @return Синтаксис использования команды в виде строки.
     * @details Должен быть реализован производными классами для отображения правильного использования команды.
     */
    [[nodiscard]] virtual std::string getUsage() const override = 0;

    /**
     * @english
     * @brief Pure virtual method to execute the command logic.
     * @param args Vector of string arguments passed to the command.
     * @return true if command execution was successful, false otherwise.
     * @details Must be implemented by derived classes to provide command-specific logic.
     *
     * @russian
     * @brief Чисто виртуальный метод для выполнения логики команды.
     * @param args Вектор строковых аргументов, переданных команде.
     * @return true если выполнение команды было успешным, false в противном случае.
     * @details Должен быть реализован производными классами для предоставления специфичной для команды логики.
     */
    virtual bool execute(const std::vector<std::string>& args) override = 0;

    /**
     * @english
     * @brief Pure virtual method to display detailed help information.
     * @details Must be implemented by derived classes to show command-specific help.
     *
     * @russian
     * @brief Чисто виртуальный метод для отображения подробной справочной информации.
     * @details Должен быть реализован производными классами для отображения специфичной для команды справки.
     */
    virtual void showHelp() const override = 0;

protected:
    /**
     * @english
     * @brief Sends an informational notification via the event bus.
     * @param message The informational message to send.
     * @details Utility method for derived classes to send info notifications.
     *
     * @russian
     * @brief Отправляет информационное уведомление через шину событий.
     * @param message Информационное сообщение для отправки.
     * @details Вспомогательный метод для производных классов для отправки информационных уведомлений.
     */
    void notifyInfo(const std::string& message) const;

    /**
     * @english
     * @brief Sends an error notification via the event bus.
     * @param message The error message to send.
     * @details Utility method for derived classes to send error notifications.
     *
     * @russian
     * @brief Отправляет уведомление об ошибке через шину событий.
     * @param message Сообщение об ошибке для отправки.
     * @details Вспомогательный метод для производных классов для отправки уведомлений об ошибках.
     */
    void notifyError(const std::string& message) const;

    /**
     * @english
     * @brief Shared pointer to the event bus for system notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий для системных уведомлений.
     */
    std::shared_ptr<ISubject> event_bus_;

    /**
     * @english
     * @brief Shared pointer to the repository manager for repository operations.
     *
     * @russian
     * @brief Общий указатель на менеджер репозиториев для операций с репозиториями.
     */
    std::shared_ptr<RepositoryManager> repo_manager_;
};

}