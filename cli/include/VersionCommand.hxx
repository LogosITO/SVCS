/**
 * @file VersionCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the VersionCommand class for displaying version information.
 *
 * @russian
 * @brief Объявление класса VersionCommand для отображения информации о версии.
 */
#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @english
 * @brief Command for displaying version information about SVCS.
 * @details The VersionCommand shows the current version, build information,
 * and copyright details.
 *
 * @russian
 * @brief Команда для отображения информации о версии SVCS.
 * @details VersionCommand показывает текущую версию, информацию о сборке
 * и детали авторских прав.
 */
class VersionCommand : public ICommand {
private:
    /**
     * @english
     * @brief Event bus for notifications.
     *
     * @russian
     * @brief Шина событий для уведомлений.
     */
    std::shared_ptr<ISubject> eventBus_;

public:
    /**
     * @english
     * @brief Constructs the VersionCommand.
     * @param subject Shared pointer to the event bus for notifications.
     *
     * @russian
     * @brief Конструирует VersionCommand.
     * @param subject Общий указатель на шину событий для уведомлений.
     */
    explicit VersionCommand(std::shared_ptr<ISubject> subject);

    /**
     * @english
     * @brief Executes the version command.
     * @param args Command arguments (ignored for version command).
     * @return true always, as version command cannot fail.
     *
     * @russian
     * @brief Выполняет команду version.
     * @param args Аргументы команды (игнорируются для команды version).
     * @return true всегда, так как команда version не может завершиться неудачей.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return "version"
     *
     * @russian
     * @brief Получает имя команды.
     * @return "version"
     */
    [[nodiscard]] std::string getName() const override { return "version"; }

    /**
     * @english
     * @brief Gets the description of the command.
     * @return "Show version information"
     *
     * @russian
     * @brief Получает описание команды.
     * @return "Показать информацию о версии"
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax of the command.
     * @return "svcs version"
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return "svcs version"
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
     * @brief Gets the full version string.
     * @return Formatted version information.
     *
     * @russian
     * @brief Получает полную строку версии.
     * @return Отформатированная информация о версии.
     */
    [[nodiscard]] static std::string getVersionString();

    /**
     * @english
     * @brief Gets build information (debug/release, compiler, etc.).
     * @return Build information string.
     *
     * @russian
     * @brief Получает информацию о сборке (debug/release, компилятор и т.д.).
     * @return Строка информации о сборке.
     */
    [[nodiscard]] static std::string getBuildInfo();

    /**
     * @english
     * @brief Gets copyright information.
     * @return Copyright string.
     *
     * @russian
     * @brief Получает информацию об авторских правах.
     * @return Строка авторских прав.
     */
    [[nodiscard]] static std::string getCopyright();
};