/**
 * @file StatusCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the StatusCommand class for showing repository status.
 *
 * @russian
 * @brief Объявление класса StatusCommand для отображения статуса репозитория.
 */
#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <memory>
#include <vector>
#include <string>
#include <filesystem>

/**
 * @english
 * @brief Command for showing the current status of the repository.
 * @details The StatusCommand displays:
 * - Current branch
 * - Staged changes (ready to save)
 * - Unstaged changes (not yet added)
 * - Untracked files (new files)
 * - Status of specific files when provided as arguments
 *
 * @russian
 * @brief Команда для отображения текущего статуса репозитория.
 * @details StatusCommand отображает:
 * - Текущую ветку
 * - Подготовленные изменения (готовые к сохранению)
 * - Неподготовленные изменения (еще не добавленные)
 * - Неотслеживаемые файлы (новые файлы)
 * - Статус конкретных файлов при предоставлении в качестве аргументов
 */
class StatusCommand : public ICommand {
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
     * @brief Constructs the StatusCommand.
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     *
     * @russian
     * @brief Конструирует StatusCommand.
     * @param subject Общий указатель на шину событий для уведомлений.
     * @param repoManager Общий указатель на менеджер репозитория.
     */
    StatusCommand(std::shared_ptr<ISubject> subject,
                  std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the status command.
     * @param args Command arguments - if provided, shows status for specific files.
     * @return true always, as status command cannot fail.
     *
     * @russian
     * @brief Выполняет команду status.
     * @param args Аргументы команды - если предоставлены, показывает статус для конкретных файлов.
     * @return true всегда, так как команда status не может завершиться неудачей.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return "status"
     *
     * @russian
     * @brief Получает имя команды.
     * @return "status"
     */
    [[nodiscard]] std::string getName() const override { return "status"; }

    /**
     * @english
     * @brief Gets the description of the command.
     * @return "Show the working tree status"
     *
     * @russian
     * @brief Получает описание команды.
     * @return "Показать статус рабочего дерева"
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return "svcs status [file1 file2 ...]"
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return "svcs status [file1 file2 ...]"
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
     * @brief Shows the full repository status.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Показывает полный статус репозитория.
     * @return true если успешно, false в противном случае.
     */
    [[nodiscard]] bool showFullStatus() const;

    /**
     * @english
     * @brief Shows status for specific files.
     * @param files List of files to show status for.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Показывает статус для конкретных файлов.
     * @param files Список файлов для отображения статуса.
     * @return true если успешно, false в противном случае.
     */
    [[nodiscard]] bool showFileStatus(const std::vector<std::string>& files) const;

    /**
     * @english
     * @brief Shows the current branch information.
     *
     * @russian
     * @brief Показывает информацию о текущей ветке.
     */
    void showBranchInfo() const;

    /**
     * @english
     * @brief Shows staged changes (files ready to save).
     *
     * @russian
     * @brief Показывает подготовленные изменения (файлы готовые к сохранению).
     */
    void showStagedChanges() const;

    /**
     * @english
     * @brief Shows unstaged changes (modified files not yet added).
     *
     * @russian
     * @brief Показывает неподготовленные изменения (измененные файлы еще не добавленные).
     */
    void showUnstagedChanges() const;

    /**
     * @english
     * @brief Shows untracked files (new files not yet added).
     *
     * @russian
     * @brief Показывает неотслеживаемые файлы (новые файлы еще не добавленные).
     */
    void showUntrackedFiles() const;

    /**
     * @english
     * @brief Gets the current branch name.
     * @return Current branch name, or "main" by default.
     *
     * @russian
     * @brief Получает имя текущей ветки.
     * @return Имя текущей ветки или "main" по умолчанию.
     */
    [[nodiscard]] std::string getCurrentBranch() const;

    /**
     * @english
     * @brief Checks if a file has been modified compared to staged version.
     * @param filePath The path to the file to check.
     * @return true if file has been modified, false otherwise.
     *
     * @russian
     * @brief Проверяет, был ли файл изменен по сравнению с подготовленной версией.
     * @param filePath Путь к файлу для проверки.
     * @return true если файл был изменен, false в противном случае.
     */
    [[nodiscard]] bool isFileModified(const std::filesystem::path& filePath) const;

    /**
     * @english
     * @brief Finds all untracked files in the repository.
     * @return Vector of paths to untracked files.
     *
     * @russian
     * @brief Находит все неотслеживаемые файлы в репозитории.
     * @return Вектор путей к неотслеживаемым файлам.
     */
    [[nodiscard]] std::vector<std::filesystem::path> findUntrackedFiles() const;

    /**
     * @english
     * @brief Formats file status with colors/icons (simple version).
     * @param status The status character.
     * @param filePath The file path.
     * @return Formatted status string.
     *
     * @russian
     * @brief Форматирует статус файла с цветами/иконками (простая версия).
     * @param status Символ статуса.
     * @param filePath Путь к файлу.
     * @return Отформатированная строка статуса.
     */
    [[nodiscard]] static std::string formatFileStatus(char status, const std::string& filePath);

    /**
     * @english
     * @brief Gets the status of a specific file.
     * @param filePath The file to check.
     * @return Status character and description.
     *
     * @russian
     * @brief Получает статус конкретного файла.
     * @param filePath Файл для проверки.
     * @return Символ статуса и описание.
     */
    [[nodiscard]] std::pair<char, std::string> getFileStatus(const std::string& filePath) const;
};