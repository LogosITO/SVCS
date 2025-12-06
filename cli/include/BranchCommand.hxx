/**
 * @file BranchCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the BranchCommand class for managing branches.
 * @details The BranchCommand is the interface for the 'svcs branch' command.
 * It supports various branch operations, including listing existing branches,
 * creating, deleting, renaming, and switching branches. It relies on the
 * BranchManager for core logic and the ISubject event bus for user communication.
 *
 * @russian
 * @brief Объявление класса BranchCommand для управления ветками.
 * @details BranchCommand является интерфейсом для команды 'svcs branch'.
 * Он поддерживает различные операции с ветками, включая список существующих веток,
 * создание, удаление, переименование и переключение веток. Он полагается на
 * BranchManager для основной логики и шину событий ISubject для общения с пользователем.
 */
#pragma once

#include "ICommand.hxx"
#include "../../core/include/BranchManager.hxx"
#include "../../services/ISubject.hxx"

#include <string>
#include <vector>
#include <memory>

/**
 * @english
 * @namespace svcs::cli
 * @brief Command-line interface components and command implementations.
 * @details Contains all CLI commands that users interact with directly,
 * including AddCommand, CommitCommand, StatusCommand, etc.
 *
 * @russian
 * @namespace svcs::cli
 * @brief Компоненты командной строки и реализации команд.
 * @details Содержит все CLI команды, с которыми пользователи взаимодействуют напрямую,
 * включая AddCommand, CommitCommand, StatusCommand и другие.
 */
namespace svcs::cli {

using namespace svcs::core;

/**
 * @english
 * @class BranchCommand
 * @brief Command for managing branches in the version control system.
 * @details Implements the 'svcs branch' functionality, providing an interface
 * for the user to interact with the repository's branch structure.
 *
 * @russian
 * @class BranchCommand
 * @brief Команда для управления ветками в системе контроля версий.
 * @details Реализует функциональность 'svcs branch', предоставляя интерфейс
 * для взаимодействия пользователя со структурой веток репозитория.
 */
class BranchCommand : public ICommand {
public:
    /**
     * @english
     * @brief Constructor for BranchCommand.
     * @param event_bus The event bus for user communication.
     * @param branch_manager The branch manager for core operations.
     *
     * @russian
     * @brief Конструктор для BranchCommand.
     * @param event_bus Шина событий для общения с пользователем.
     * @param branch_manager Менеджер веток для основных операций.
     */
    BranchCommand(std::shared_ptr<ISubject> event_bus,
                  std::shared_ptr<BranchManager> branch_manager);

    /**
     * @english
     * @brief Destructor.
     *
     * @russian
     * @brief Деструктор.
     */
    ~BranchCommand() override = default;

    /**
     * @english
     * @brief Gets the command name.
     * @return The command name "branch".
     *
     * @russian
     * @brief Получает имя команды.
     * @return Имя команды "branch".
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @english
     * @brief Gets the command description.
     * @return A brief description of the command.
     *
     * @russian
     * @brief Получает описание команды.
     * @return Краткое описание команды.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the command usage syntax.
     * @return The usage syntax string.
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка синтаксиса использования.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Executes the branch command with given arguments.
     * @param args Command line arguments.
     * @return true if execution succeeded, false otherwise.
     *
     * @russian
     * @brief Выполняет команду branch с заданными аргументами.
     * @param args Аргументы командной строки.
     * @return true если выполнение успешно, false в противном случае.
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
    /**
     * @english
     * @brief Lists all existing branches.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Выводит список всех существующих веток.
     * @return true если успешно, false в противном случае.
     */
    bool listBranches() const;

    /**
     * @english
     * @brief Creates a new branch from the current commit.
     * @param branch_name Name of the branch to create.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Создает новую ветку из текущего коммита.
     * @param branch_name Имя создаваемой ветки.
     * @return true если успешно, false в противном случае.
     */
    bool createBranch(const std::string& branch_name) const;

    /**
     * @english
     * @brief Creates a new branch from a specific commit.
     * @param branch_name Name of the branch to create.
     * @param commit_hash Hash of the commit to branch from.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Создает новую ветку из конкретного коммита.
     * @param branch_name Имя создаваемой ветки.
     * @param commit_hash Хеш коммита, от которого создается ветка.
     * @return true если успешно, false в противном случае.
     */
    bool createBranchFromCommit(const std::string& branch_name, const std::string& commit_hash) const;

    /**
     * @english
     * @brief Deletes a branch.
     * @param branch_name Name of the branch to delete.
     * @param force Force deletion even if not merged.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Удаляет ветку.
     * @param branch_name Имя удаляемой ветки.
     * @param force Принудительное удаление даже если не слияна.
     * @return true если успешно, false в противном случае.
     */
    bool deleteBranch(const std::string& branch_name, bool force = false) const;

    /**
     * @english
     * @brief Renames a branch.
     * @param old_name Current name of the branch.
     * @param new_name New name for the branch.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Переименовывает ветку.
     * @param old_name Текущее имя ветки.
     * @param new_name Новое имя для ветки.
     * @return true если успешно, false в противном случае.
     */
    bool renameBranch(const std::string& old_name, const std::string& new_name) const;

    /**
     * @english
     * @brief Shows the current branch name.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Показывает имя текущей ветки.
     * @return true если успешно, false в противном случае.
     */
    bool showCurrentBranch() const;

    /**
     * @english
     * @brief Switches to a different branch.
     * @param branch_name Name of the branch to switch to.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Переключается на другую ветку.
     * @param branch_name Имя ветки для переключения.
     * @return true если успешно, false в противном случае.
     */
    bool switchBranch(const std::string& branch_name) const;

    /**
     * @english
     * @brief Validates a branch name.
     * @param name The branch name to validate.
     * @return true if valid, false otherwise.
     *
     * @russian
     * @brief Проверяет валидность имени ветки.
     * @param name Имя ветки для проверки.
     * @return true если валидно, false в противном случае.
     */
    [[nodiscard]] static bool isValidBranchName(const std::string& name);

    /**
     * @english
     * @brief Checks if a branch exists.
     * @param name The branch name to check.
     * @return true if exists, false otherwise.
     *
     * @russian
     * @brief Проверяет существование ветки.
     * @param name Имя ветки для проверки.
     * @return true если существует, false в противном случае.
     */
    [[nodiscard]] bool branchExists(const std::string& name) const;

    /**
     * @english
     * @brief Validates a commit hash.
     * @param hash The commit hash to validate.
     * @return true if valid, false otherwise.
     *
     * @russian
     * @brief Проверяет валидность хеша коммита.
     * @param hash Хеш коммита для проверки.
     * @return true если валиден, false в противном случае.
     */
    [[nodiscard]] static bool isValidCommitHash(const std::string& hash);

private:
    /**
     * @english
     * @brief Event bus for user communication.
     *
     * @russian
     * @brief Шина событий для общения с пользователем.
     */
    std::shared_ptr<ISubject> event_bus_;

    /**
     * @english
     * @brief Branch manager for core operations.
     *
     * @russian
     * @brief Менеджер веток для основных операций.
     */
    std::shared_ptr<BranchManager> branch_manager_;
};

}