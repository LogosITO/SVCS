/**
 * @file AddCommand.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the AddCommand class.
 *
 * @russian
 * @brief Объявление класса AddCommand.
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
 * @brief Defines the status of a file within the SVCS.
 *
 * @russian
 * @brief Определяет статус файла в SVCS.
 */
enum class FileStatus {
    /**
     * @english
     * @brief File is present but not tracked by SVCS.
     *
     * @russian
     * @brief Файл присутствует, но не отслеживается SVCS.
     */
    UNTRACKED,

    /**
     * @english
     * @brief File is tracked and has been modified since the last commit.
     *
     * @russian
     * @brief Файл отслеживается и был изменен с последнего коммита.
     */
    MODIFIED,

    /**
     * @english
     * @brief File is tracked but has been deleted from the working directory.
     *
     * @russian
     * @brief Файл отслеживается, но был удален из рабочей директории.
     */
    DELETED,

    /**
     * @english
     * @brief File is tracked and has no changes since the last commit.
     *
     * @russian
     * @brief Файл отслеживается и не имеет изменений с последнего коммита.
     */
    UNMODIFIED
};

/**
 * @english
 * @brief Implements the "add" command for the Simple Version Control System (SVCS).
 * @details This command is responsible for adding (staging) the specified files
 * to the staging area of the repository for a subsequent commit.
 * The class inherits from ICommand and uses the Command pattern
 * to execute its operations. It depends on ISubject (event bus)
 * for user notifications and RepositoryManager for repository
 * operations.
 *
 * @russian
 * @brief Реализует команду "add" для Simple Version Control System (SVCS).
 * @details Эта команда отвечает за добавление (подготовку) указанных файлов
 * в область подготовки репозитория для последующего коммита.
 * Класс наследуется от ICommand и использует шаблон Command
 * для выполнения своих операций. Он зависит от ISubject (шина событий)
 * для уведомлений пользователя и RepositoryManager для операций
 * с репозиторием.
 */
class AddCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<RepositoryManager> repoManager_;

    /**
     * @english
     * @brief Structure to hold options for the "add" command.
     *
     * @russian
     * @brief Структура для хранения опций команды "add".
     */
    struct AddOptions {
        /**
         * @english
         * @brief Don't actually add files, just show what would be done.
         *
         * @russian
         * @brief Не добавлять файлы фактически, только показать что было бы сделано.
         */
        bool dryRun = false;

        /**
         * @english
         * @brief Interactively stage changes.
         *
         * @russian
         * @brief Интерактивно подготавливать изменения.
         */
        bool interactive = false;

        /**
         * @english
         * @brief Interactively choose hunks of changes to stage.
         *
         * @russian
         * @brief Интерактивно выбирать фрагменты изменений для подготовки.
         */
        bool patch = false;

        /**
         * @english
         * @brief Only stage files already tracked.
         *
         * @russian
         * @brief Подготавливать только уже отслеживаемые файлы.
         */
        bool update = false;

        /**
         * @english
         * @brief Allow adding ignored files.
         *
         * @russian
         * @brief Разрешить добавление игнорируемых файлов.
         */
        bool force = false;

        /**
         * @english
         * @brief Show help message.
         *
         * @russian
         * @brief Показать сообщение справки.
         */
        bool showHelp = false;

        /**
         * @english
         * @brief List of files/directories to add.
         *
         * @russian
         * @brief Список файлов/директорий для добавления.
         */
        std::vector<std::string> files;

        /**
         * @english
         * @brief Patterns to exclude from adding.
         *
         * @russian
         * @brief Шаблоны для исключения из добавления.
         */
        std::vector<std::string> excludePatterns;
    };

    // New methods
    /**
     * @english
     * @brief Handles the interactive staging mode.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Обрабатывает интерактивный режим подготовки.
     * @return true если успешно, false в противном случае.
     */
    [[nodiscard]] bool addInteractive() const;

    /**
     * @english
     * @brief Handles the patch mode for staging specific hunks.
     * @param filePath The path to the file to stage hunks from.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Обрабатывает режим патча для подготовки конкретных фрагментов.
     * @param filePath Путь к файлу для подготовки фрагментов.
     * @return true если успешно, false в противном случае.
     */
    [[nodiscard]] bool addPatch(const std::string& filePath) const;

    /**
     * @english
     * @brief Shows the result of a dry run without modifying the index.
     * @param files The list of files that would be processed.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Показывает результат пробного запуска без изменения индекса.
     * @param files Список файлов, которые были бы обработаны.
     * @return true если успешно, false в противном случае.
     */
    [[nodiscard]] bool showDryRun(const std::vector<std::string>& files) const;

    /**
     * @english
     * @brief Determines if a file should be added based on its status and force flag.
     * @param file The path to the file.
     * @param force True if force flag is set.
     * @return true if the file should be added, false otherwise.
     *
     * @russian
     * @brief Определяет, должен ли файл быть добавлен на основе его статуса и флага force.
     * @param file Путь к файлу.
     * @param force True если установлен флаг force.
     * @return true если файл должен быть добавлен, false в противном случае.
     */
    [[nodiscard]] bool shouldAddFile(const std::string& file, bool force) const;

    // Argument Parsing
    /**
     * @english
     * @brief Parses the command line arguments into AddOptions structure.
     * @param args The vector of string arguments passed to the command.
     * @return The populated AddOptions structure.
     *
     * @russian
     * @brief Разбирает аргументы командной строки в структуру AddOptions.
     * @param args Вектор строковых аргументов, переданных команде.
     * @return Заполненная структура AddOptions.
     */
    [[nodiscard]] AddOptions parseArguments(const std::vector<std::string>& args) const;

public:
    /**
     * @english
     * @brief Constructor for AddCommand.
     * @param subject A shared pointer to the ISubject (event bus).
     * @param repoManager A shared pointer to the RepositoryManager.
     *
     * @russian
     * @brief Конструктор для AddCommand.
     * @param subject Общий указатель на ISubject (шина событий).
     * @param repoManager Общий указатель на RepositoryManager.
     */
    AddCommand(std::shared_ptr<ISubject> subject,
               std::shared_ptr<RepositoryManager> repoManager);

    /**
     * @english
     * @brief Executes the "add" command with the given arguments.
     * @param args The vector of string arguments for the command.
     * @return true if the command executed successfully, false otherwise.
     *
     * @russian
     * @brief Выполняет команду "add" с заданными аргументами.
     * @param args Вектор строковых аргументов для команды.
     * @return true если команда выполнена успешно, false в противном случае.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Retrieves all file paths within a given directory, recursively.
     * @param directory The path to the directory.
     * @return A vector of strings containing all file paths.
     *
     * @russian
     * @brief Получает все пути к файлам в заданной директории рекурсивно.
     * @param directory Путь к директории.
     * @return Вектор строк, содержащий все пути к файлам.
     */
    [[nodiscard]] std::vector<std::string> getAllFilesInDirectory(const std::string& directory) const;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return The command name, "add".
     *
     * @russian
     * @brief Получает имя команды.
     * @return Имя команды, "add".
     */
    [[nodiscard]] std::string getName() const override { return "add"; }

    /**
     * @english
     * @brief Gets a brief description of the command.
     * @return A string describing the command's purpose.
     *
     * @russian
     * @brief Получает краткое описание команды.
     * @return Строка, описывающая назначение команды.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the command's usage syntax.
     * @return A string showing how to use the command.
     *
     * @russian
     * @brief Получает синтаксис использования команды.
     * @return Строка, показывающая как использовать команду.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Displays the detailed help information for the command.
     *
     * @russian
     * @brief Отображает подробную справочную информацию для команды.
     */
    void showHelp() const override;
};