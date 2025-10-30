/**
 * @file UndoCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the UndoCommand class for reverting changes.
 * @details The UndoCommand is responsible for reverting the repository's working
 * directory and history to the state of a previous commit. It supports reverting
 * the last commit or a specific commit identified by its hash. This operation
 * typically involves confirming the action before execution, unless a force flag
 * is used.
 *
 * @russian
 * @brief Объявление класса UndoCommand для отмены изменений.
 * @details UndoCommand отвечает за возврат рабочей директории репозитория
 * и истории к состоянию предыдущего коммита. Он поддерживает отмену
 * последнего коммита или конкретного коммита, идентифицированного по его хешу. Эта операция
 * обычно включает подтверждение действия перед выполнением, если не используется флаг force.
 */
#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <string>
#include <vector>

/**
 * @english
 * @class UndoCommand
 * @brief Command to revert the repository state to a previous commit.
 * @details Implements the 'svcs undo' functionality. It uses the RepositoryManager
 * to perform the actual history and working directory manipulation. It utilizes
 * the ISubject (event bus) for user output and confirmation prompts.
 *
 * @russian
 * @class UndoCommand
 * @brief Команда для возврата состояния репозитория к предыдущему коммиту.
 * @details Реализует функциональность 'svcs undo'. Он использует RepositoryManager
 * для выполнения фактических манипуляций с историей и рабочей директорией. Он использует
 * ISubject (шину событий) для вывода пользователю и запросов подтверждения.
 */
class UndoCommand : public ICommand {
public:
    /**
     * @english
     * @brief Constructor for UndoCommand.
     * @param subject Shared pointer to the event bus for communication (output, prompts).
     * @param repoManager Shared pointer to the RepositoryManager for core repository operations.
     *
     * @russian
     * @brief Конструктор для UndoCommand.
     * @param subject Общий указатель на шину событий для связи (вывод, запросы).
     * @param repoManager Общий указатель на RepositoryManager для основных операций с репозиторием.
     */
    UndoCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the undo command based on the provided arguments.
     * @details Parses arguments for a specific commit hash or the `--force` flag.
     * If no hash is provided, it attempts to undo the last commit.
     * @param args The command-line arguments (e.g., commit hash, --force).
     * @return true on successful execution (or if help is shown), false on error.
     *
     * @russian
     * @brief Выполняет команду undo на основе предоставленных аргументов.
     * @details Разбирает аргументы для конкретного хеша коммита или флага `--force`.
     * Если хеш не предоставлен, пытается отменить последний коммит.
     * @param args Аргументы командной строки (например, хеш коммита, --force).
     * @return true при успешном выполнении (или если показана справка), false при ошибке.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the command's primary name.
     * @return The string "undo".
     *
     * @russian
     * @brief Получает основное имя команды.
     * @return Строка "undo".
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @english
     * @brief Gets the short description of the command.
     * @return A string describing the command's function.
     *
     * @russian
     * @brief Получает краткое описание команды.
     * @return Строка, описывающая функцию команды.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return A string detailing how the command is used.
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка, детализирующая как используется команда.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Displays detailed help information for the command.
     *
     * @russian
     * @brief Отображает подробную справочную информацию для команды.
     */
    void showHelp() const override;

private:
    /**
     * @english
     * @brief Reverts the changes introduced by the last commit in the current branch.
     * @param force Flag to skip confirmation prompt (\c true for forced undo).
     * @return \c true if the undo operation was successful, \c false otherwise.
     *
     * @russian
     * @brief Отменяет изменения, внесенные последним коммитом в текущей ветке.
     * @param force Флаг для пропуска запроса подтверждения (\c true для принудительной отмены).
     * @return \c true если операция отмены была успешной, \c false в противном случае.
     */
    bool undoLastCommit(bool force) const;

    /**
     * @english
     * @brief Reverts the repository state to a specific commit hash.
     * @param commitHash The hash of the commit to revert to.
     * @param force Flag to skip confirmation prompt (\c true for forced undo).
     * @return \c true if the undo operation was successful, \c false otherwise.
     *
     * @russian
     * @brief Возвращает состояние репозитория к конкретному хешу коммита.
     * @param commitHash Хеш коммита, к которому нужно вернуться.
     * @param force Флаг для пропуска запроса подтверждения (\c true для принудительной отмены).
     * @return \c true если операция отмены была успешной, \c false в противном случае.
     */
    bool undoSpecificCommit(const std::string& commitHash, bool force) const;

    /**
     * @english
     * @brief Prompts the user for confirmation before performing the undo operation.
     * @details This prompt is skipped if the \c force flag is \c true.
     * @param commitMessage The message of the commit being undone.
     * @param commitHash The hash of the commit being undone.
     * @param force The flag indicating if the confirmation should be skipped.
     * @return \c true if the user confirms or if the operation is forced, \c false otherwise.
     *
     * @russian
     * @brief Запрашивает у пользователя подтверждение перед выполнением операции отмены.
     * @details Этот запрос пропускается, если флаг \c force установлен в \c true.
     * @param commitMessage Сообщение коммита, который отменяется.
     * @param commitHash Хеш коммита, который отменяется.
     * @param force Флаг, указывающий, следует ли пропустить подтверждение.
     * @return \c true если пользователь подтверждает или если операция принудительная, \c false в противном случае.
     */
    static bool confirmUndo(const std::string& commitMessage, const std::string& commitHash, bool force);

    /**
     * @english
     * @brief Resets the current repository state to the previous valid commit, discarding all staged and working directory changes.
     * @return \c true if the forced reset was successful, \c false otherwise.
     *
     * @russian
     * @brief Сбрасывает текущее состояние репозитория к предыдущему валидному коммиту, отбрасывая все подготовленные изменения и изменения рабочей директории.
     * @return \c true если принудительный сброс был успешным, \c false в противном случае.
     */
    bool forceResetRepository() const;

private:
    /**
     * @english
     * @brief Shared pointer to the event bus for communication.
     *
     * @russian
     * @brief Общий указатель на шину событий для связи.
     */
    std::shared_ptr<ISubject> event_bus;

    /**
     * @english
     * @brief Shared pointer to the RepositoryManager for core VCS logic.
     *
     * @russian
     * @brief Общий указатель на RepositoryManager для основной логики VCS.
     */
    std::shared_ptr<RepositoryManager> repo_manager;
};