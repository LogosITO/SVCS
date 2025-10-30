/**
 * @file MergeCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the MergeCommand for merging branches.
 *
 * @russian
 * @brief Объявление команды MergeCommand для слияния веток.
 */
#pragma once

#include "ICommand.hxx"
#include "../../core/include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <string>
#include <vector>
#include <memory>

/**
 * @english
 * @class MergeCommand
 * @brief Command for merging branches in the version control system.
 *
 * @russian
 * @class MergeCommand
 * @brief Команда для слияния веток в системе контроля версий.
 */
class MergeCommand : public ICommand {
public:
    /**
     * @english
     * @brief Constructs a MergeCommand with event bus and repository manager.
     * @param event_bus The event bus for notifications
     * @param repo_manager The repository manager for merge operations
     *
     * @russian
     * @brief Конструирует MergeCommand с шиной событий и менеджером репозитория.
     * @param event_bus Шина событий для уведомлений
     * @param repo_manager Менеджер репозитория для операций слияния
     */
    MergeCommand(std::shared_ptr<ISubject> event_bus,
                 std::shared_ptr<RepositoryManager> repo_manager);

    /**
     * @english
     * @brief Destructor.
     *
     * @russian
     * @brief Деструктор.
     */
    ~MergeCommand() override = default;

    // ICommand interface implementation
    /**
     * @english
     * @brief Gets the command name.
     * @return The command name "merge"
     *
     * @russian
     * @brief Получает имя команды.
     * @return Имя команды "merge"
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @english
     * @brief Gets the command description.
     * @return A brief description of the command
     *
     * @russian
     * @brief Получает описание команды.
     * @return Краткое описание команды
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the command usage syntax.
     * @return The usage syntax string
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка синтаксиса использования
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Executes the merge command with given arguments.
     * @param args Command line arguments
     * @return true if execution succeeded, false otherwise
     *
     * @russian
     * @brief Выполняет команду merge с заданными аргументами.
     * @param args Аргументы командной строки
     * @return true если выполнение успешно, false в противном случае
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
     * @brief Debugs branch history for troubleshooting.
     * @param branch_name Name of the branch to debug
     * @param head_commit Head commit of the branch
     *
     * @russian
     * @brief Отлаживает историю ветки для устранения неполадок.
     * @param branch_name Имя ветки для отладки
     * @param head_commit Главный коммит ветки
     */
    void debugBranchHistory(const std::string& branch_name, const std::string& head_commit) const;

    /**
     * @english
     * @brief Merges a branch into the current branch.
     * @param branch_name Name of the branch to merge
     * @return true if successful, false otherwise
     *
     * @russian
     * @brief Сливает ветку в текущую ветку.
     * @param branch_name Имя ветки для слияния
     * @return true если успешно, false в противном случае
     */
    bool mergeBranch(const std::string& branch_name) const;

    /**
     * @english
     * @brief Aborts an ongoing merge operation.
     * @return true if successful, false otherwise
     *
     * @russian
     * @brief Прерывает выполняющуюся операцию слияния.
     * @return true если успешно, false в противном случае
     */
    bool abortMerge() const;

    /**
     * @english
     * @brief Checks if a merge is in progress.
     * @return true if merge is in progress, false otherwise
     *
     * @russian
     * @brief Проверяет, выполняется ли слияние.
     * @return true если слияние выполняется, false в противном случае
     */
    [[nodiscard]] bool isMergeInProgress() const;

    /**
     * @english
     * @brief Finds the common ancestor of two commits.
     * @param commit1 First commit hash
     * @param commit2 Second commit hash
     * @return Common ancestor commit hash
     *
     * @russian
     * @brief Находит общего предка двух коммитов.
     * @param commit1 Хеш первого коммита
     * @param commit2 Хеш второго коммита
     * @return Хеш коммита общего предка
     */
    std::string findCommonAncestor(const std::string& commit1, const std::string& commit2) const;

    /**
     * @english
     * @brief Performs a three-way merge.
     * @param ancestor Common ancestor commit
     * @param current Current branch commit
     * @param other Other branch commit to merge
     * @return true if merge successful, false if conflicts
     *
     * @russian
     * @brief Выполняет трехстороннее слияние.
     * @param ancestor Коммит общего предка
     * @param current Коммит текущей ветки
     * @param other Коммит другой ветки для слияния
     * @return true если слияние успешно, false если есть конфликты
     */
    bool performThreeWayMerge(const std::string& ancestor,
                             const std::string& current,
                             const std::string& other) const;

    /**
     * @english
     * @brief Detects merge conflicts between file versions.
     * @param ancestor_content Ancestor file content
     * @param current_content Current branch file content
     * @param other_content Other branch file content
     * @param merged_content Output merged content
     * @return true if no conflicts, false if conflicts detected
     *
     * @russian
     * @brief Обнаруживает конфликты слияния между версиями файлов.
     * @param ancestor_content Содержимое файла предка
     * @param current_content Содержимое файла текущей ветки
     * @param other_content Содержимое файла другой ветки
     * @param merged_content Выходное объединенное содержимое
     * @return true если нет конфликтов, false если обнаружены конфликты
     */
    static bool mergeFileContent(const std::string& ancestor_content,
                         const std::string& current_content,
                         const std::string& other_content,
                         std::string& merged_content);

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
     * @brief Repository manager for merge operations.
     *
     * @russian
     * @brief Менеджер репозитория для операций слияния.
     */
    std::shared_ptr<RepositoryManager> repo_manager_;
};