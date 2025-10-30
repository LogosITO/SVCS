/**
 * @file ClearCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the ClearCommand class for removing SVCS repository.
 *
 * @russian
 * @brief Объявление класса ClearCommand для удаления репозитория SVCS.
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
 * @brief Command for removing SVCS repository structure.
 * @details The ClearCommand safely removes the .svcs directory and all repository data.
 *
 * @russian
 * @brief Команда для удаления структуры репозитория SVCS.
 * @details ClearCommand безопасно удаляет директорию .svcs и все данные репозитория.
 */
class ClearCommand : public ICommand {
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
     * @brief Constructs the ClearCommand.
     * @param subject Shared pointer to the event bus for notifications.
     * @param repoManager Shared pointer to the repository manager.
     *
     * @russian
     * @brief Конструирует ClearCommand.
     * @param subject Общий указатель на шину событий для уведомлений.
     * @param repoManager Общий указатель на менеджер репозитория.
     */
    ClearCommand(std::shared_ptr<ISubject> subject,
                 std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the clear command.
     * @param args Command arguments (--force to skip confirmation).
     * @return true if clear succeeded, false otherwise.
     *
     * @russian
     * @brief Выполняет команду clear.
     * @param args Аргументы команды (--force для пропуска подтверждения).
     * @return true если очистка успешна, false в противном случае.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return "clear"
     *
     * @russian
     * @brief Получает имя команды.
     * @return "clear"
     */
    [[nodiscard]] std::string getName() const override { return "clear"; }

    /**
     * @english
     * @brief Gets the description of the command.
     * @return "Remove SVCS repository from current directory"
     *
     * @russian
     * @brief Получает описание команды.
     * @return "Удалить репозиторий SVCS из текущей директории"
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return "svcs clear [--force]"
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return "svcs clear [--force]"
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
     * @brief Confirms the clear operation with user.
     * @return true if user confirms, false otherwise.
     *
     * @russian
     * @brief Подтверждает операцию очистки с пользователем.
     * @return true если пользователь подтверждает, false в противном случае.
     */
    [[nodiscard]] static bool confirmClear();

    /**
     * @english
     * @brief Removes the repository.
     * @return true if removal succeeded, false otherwise.
     *
     * @russian
     * @brief Удаляет репозиторий.
     * @return true если удаление успешно, false в противном случае.
     */
    [[nodiscard]] bool removeRepository() const;
};