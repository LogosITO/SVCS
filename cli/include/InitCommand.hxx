/**
 * @file InitCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the InitCommand class.
 *
 * @russian
 * @brief Объявление класса InitCommand.
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
 * @brief Implements the "init" command for the Version Control System (VCS).
 * @details This command is responsible for initializing a new SVCS repository in the
 * current directory or a specified location, setting up the necessary
 * internal structure (e.g., the hidden .svcs directory).
 * The class inherits from ICommand, ensuring it adheres to the Command pattern
 * interface. It requires an event bus for communication and a repository manager
 * to perform the actual initialization logic.
 *
 * @russian
 * @brief Реализует команду "init" для системы контроля версий (VCS).
 * @details Эта команда отвечает за инициализацию нового репозитория SVCS в
 * текущей директории или указанном местоположении, настраивая необходимую
 * внутреннюю структуру (например, скрытую директорию .svcs).
 * Класс наследуется от ICommand, обеспечивая соответствие интерфейсу шаблона Command.
 * Он требует шину событий для связи и менеджер репозитория
 * для выполнения фактической логики инициализации.
 */
class InitCommand : public ICommand {
private:
    /**
     * @english
     * @brief Shared pointer to the event bus used for application-wide notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий, используемую для уведомлений во всем приложении.
     */
    std::shared_ptr<ISubject> eventBus_;

    /**
     * @english
     * @brief Shared pointer to the RepositoryManager responsible for repository operations.
     *
     * @russian
     * @brief Общий указатель на RepositoryManager, ответственный за операции с репозиторием.
     */
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @english
     * @brief Constructs an InitCommand.
     * @param subject A shared pointer to the event bus (ISubject).
     * @param repoManager A shared pointer to the RepositoryManager.
     *
     * @russian
     * @brief Конструирует InitCommand.
     * @param subject Общий указатель на шину событий (ISubject).
     * @param repoManager Общий указатель на RepositoryManager.
     */
    InitCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the repository initialization logic.
     * @details This method attempts to create the repository structure.
     * @param args A vector of strings containing command-line arguments (usually empty for 'init').
     * @return \c true if the repository was successfully initialized or if it already exists,
     * \c false if the initialization failed.
     *
     * @russian
     * @brief Выполняет логику инициализации репозитория.
     * @details Этот метод пытается создать структуру репозитория.
     * @param args Вектор строк, содержащих аргументы командной строки (обычно пустые для 'init').
     * @return \c true если репозиторий был успешно инициализирован или если он уже существует,
     * \c false если инициализация не удалась.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Returns the command name.
     * @return The string "init".
     *
     * @russian
     * @brief Возвращает имя команды.
     * @return Строка "init".
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @english
     * @brief Returns a brief description of the command's purpose.
     * @return A string containing the description.
     *
     * @russian
     * @brief Возвращает краткое описание назначения команды.
     * @return Строка, содержащая описание.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Returns the usage syntax for the command.
     * @return A string containing the usage: "svcs init".
     *
     * @russian
     * @brief Возвращает синтаксис использования для команды.
     * @return Строка, содержащая использование: "svcs init".
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Displays detailed help information about the command via the event bus.
     *
     * @russian
     * @brief Отображает подробную справочную информацию о команде через шину событий.
     */
    void showHelp() const override;
};