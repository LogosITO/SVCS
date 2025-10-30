/**
 * @file SaveCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the SaveCommand class for saving (committing) staged changes.
 *
 * @russian
 * @brief Объявление класса SaveCommand для сохранения (коммита) подготовленных изменений.
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
 * @brief Command for saving (committing) staged changes to the repository.
 * @details The SaveCommand creates a permanent snapshot of all staged changes
 * with a descriptive message.
 *
 * @russian
 * @brief Команда для сохранения (коммита) подготовленных изменений в репозиторий.
 * @details SaveCommand создает постоянный снимок всех подготовленных изменений
 * с описательным сообщением.
 */
class SaveCommand : public ICommand {
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
     * @brief Constructs the SaveCommand.
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     *
     * @russian
     * @brief Конструирует SaveCommand.
     * @param subject Общий указатель на шину событий для уведомлений.
     * @param repoManager Общий указатель на менеджер репозитория.
     */
    SaveCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the save command.
     * @param args Command arguments. Expected format: ["-m", "message"] or ["--message", "message"]
     * @return true if save succeeded, false otherwise.
     *
     * @russian
     * @brief Выполняет команду save.
     * @param args Аргументы команды. Ожидаемый формат: ["-m", "message"] или ["--message", "message"]
     * @return true если сохранение успешно, false в противном случае.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return "save"
     *
     * @russian
     * @brief Получает имя команды.
     * @return "save"
     */
    [[nodiscard]] std::string getName() const override { return "save"; }

    /**
     * @english
     * @brief Gets the description of the command.
     * @return "Save staged changes to the repository"
     *
     * @russian
     * @brief Получает описание команды.
     * @return "Сохранить подготовленные изменения в репозиторий"
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return "svcs save -m \"message\""
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return "svcs save -m \"message\""
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
     * @brief Parses command line arguments to extract the commit message.
     * @param args Command arguments.
     * @return The commit message, or empty string if not found.
     *
     * @russian
     * @brief Разбирает аргументы командной строки для извлечения сообщения коммита.
     * @param args Аргументы команды.
     * @return Сообщение коммита или пустая строка, если не найдено.
     */
    [[nodiscard]] static std::string parseMessage(const std::vector<std::string>& args);

    /**
     * @english
     * @brief Validates that a message is provided and not empty.
     * @param message The commit message to validate.
     * @return true if message is valid, false otherwise.
     *
     * @russian
     * @brief Проверяет, что сообщение предоставлено и не пустое.
     * @param message Сообщение коммита для проверки.
     * @return true если сообщение валидно, false в противном случае.
     */
    [[nodiscard]] bool validateMessage(const std::string& message) const;

    /**
     * @english
     * @brief Checks if there are any staged changes to save.
     * @return true if there are staged changes, false otherwise.
     *
     * @russian
     * @brief Проверяет, есть ли подготовленные изменения для сохранения.
     * @return true если есть подготовленные изменения, false в противном случае.
     */
    [[nodiscard]] bool hasStagedChanges() const;

    /**
     * @english
     * @brief Creates a save point (commit) with the given message.
     * @param message The commit message.
     * @return true if save succeeded, false otherwise.
     *
     * @russian
     * @brief Создает точку сохранения (коммит) с заданным сообщением.
     * @param message Сообщение коммита.
     * @return true если сохранение успешно, false в противном случае.
     */
    [[nodiscard]] bool createSavePoint(const std::string& message) const;
};