/**
 * @file RemoveCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the RemoveCommand class for removing files from staging area.
 *
 * @russian
 * @brief Объявление класса RemoveCommand для удаления файлов из области подготовки.
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
 * @brief Command for removing files from the staging area.
 * @details The RemoveCommand removes files from the staging area (index),
 * effectively unstaging them while keeping the actual files intact.
 *
 * @russian
 * @brief Команда для удаления файлов из области подготовки.
 * @details RemoveCommand удаляет файлы из области подготовки (индекса),
 * эффективно убирая их из подготовки, сохраняя при этом сами файлы нетронутыми.
 */
class RemoveCommand : public ICommand {
private:
    /**
     * @english
     * @brief Event bus for notifications.
     *
     * @russian
     * @brief Шина событий для уведомлений.
     */
    std::shared_ptr<ISubject> eventBus_;

    /**
     * @english
     * @brief Repository manager for repository operations.
     *
     * @russian
     * @brief Менеджер репозитория для операций с репозиторием.
     */
    std::shared_ptr<RepositoryManager> repoManager_;

public:
    /**
     * @english
     * @brief Constructs the RemoveCommand.
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     *
     * @russian
     * @brief Конструирует RemoveCommand.
     * @param subject Общий указатель на шину событий для уведомлений.
     * @param repoManager Общий указатель на менеджер репозитория.
     */
    RemoveCommand(std::shared_ptr<ISubject> subject,
                  std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the remove command.
     * @param args Command arguments - file paths to remove from staging.
     * @return true if remove succeeded, false otherwise.
     *
     * @russian
     * @brief Выполняет команду remove.
     * @param args Аргументы команды - пути к файлам для удаления из подготовки.
     * @return true если удаление успешно, false в противном случае.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return "remove"
     *
     * @russian
     * @brief Получает имя команды.
     * @return "remove"
     */
    [[nodiscard]] std::string getName() const override { return "remove"; }

    /**
     * @english
     * @brief Gets the description of the command.
     * @return "Remove files from staging area"
     *
     * @russian
     * @brief Получает описание команды.
     * @return "Удалить файлы из области подготовки"
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return "svcs remove <file> [file2 ...]"
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return "svcs remove <file> [file2 ...]"
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Shows detailed help information for this command.
     *
     * @russian
     * @brief Показывает подробную справочную информацию для этой команды.
     */
    void showHelp() const override;

private:
    /**
     * @english
     * @brief Removes a single file from staging area.
     * @param filePath The file to remove from staging.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Удаляет один файл из области подготовки.
     * @param filePath Файл для удаления из подготовки.
     * @return true если успешно, false в противном случае.
     */
    bool removeFileFromStaging(const std::string& filePath) const;

    /**
     * @english
     * @brief Removes all files from staging area.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Удаляет все файлы из области подготовки.
     * @return true если успешно, false в противном случае.
     */
    bool removeAllFromStaging() const;

    /**
     * @english
     * @brief Shows confirmation prompt for removing all files.
     * @return true if user confirms, false otherwise.
     *
     * @russian
     * @brief Показывает запрос подтверждения для удаления всех файлов.
     * @return true если пользователь подтверждает, false в противном случае.
     */
    [[nodiscard]] bool confirmRemoveAll() const;
};

}