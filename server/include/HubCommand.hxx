/**
 * @file HubCommand.hxx  
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the HubCommand class for creating central bare repositories.
 * @details The HubCommand implements the 'svcs hub' command for creating
 * central bare repositories that serve as collaboration hubs for development teams.
 * These repositories lack working directories and are optimized for remote access.
 * The command creates the necessary directory structure and configuration files
 * required for a functional central repository.
 *
 * @russian
 * @brief Объявление класса HubCommand для создания центральных голых репозиториев.
 * @details HubCommand реализует команду 'svcs hub' для создания
 * центральных голых репозиториев, которые служат центрами сотрудничества для команд разработчиков.
 * Эти репозитории не имеют рабочих директорий и оптимизированы для удаленного доступа.
 * Команда создает необходимую структуру директорий и файлы конфигурации,
 * требуемые для функционального центрального репозитория.
 */
#pragma once

#include "../../cli/include/ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

/**
 * @english
 * @class HubCommand
 * @brief Command for creating central hub repositories for team collaboration.
 * @details Implements the 'svcs hub' functionality to initialize bare repositories
 * that act as central points for pushing, pulling and collaborating on code.
 * This command is essential for setting up remote collaboration infrastructure.
 *
 * @russian
 * @class HubCommand
 * @brief Команда для создания центральных репозиториев-хабов для командного сотрудничества.
 * @details Реализует функциональность 'svcs hub' для инициализации голых репозиториев,
 * которые действуют как центральные точки для отправки, получения и совместной работы над кодом.
 * Эта команда необходима для настройки инфраструктуры удаленного сотрудничества.
 */
class HubCommand : public ICommand {
public:
    /**
     * @english
     * @brief Constructs a HubCommand with necessary dependencies.
     * @param event_bus The event bus for notification system, used for user feedback.
     * @param repository_manager The repository manager for core repository operations.
     *
     * @russian
     * @brief Конструирует HubCommand с необходимыми зависимостями.
     * @param event_bus Шина событий для системы уведомлений, используется для обратной связи с пользователем.
     * @param repository_manager Менеджер репозитория для основных операций с репозиторием.
     */
    HubCommand(std::shared_ptr<ISubject> event_bus,
               std::shared_ptr<RepositoryManager> repository_manager);

    /**
     * @english
     * @brief Default destructor.
     *
     * @russian
     * @brief Деструктор по умолчанию.
     */
    ~HubCommand() override = default;

    /**
     * @english
     * @brief Gets the name of the command.
     * @return std::string The command name "hub".
     *
     * @russian
     * @brief Получает имя команды.
     * @return std::string Имя команды "hub".
     */
    [[nodiscard]] std::string getName() const override;

    /**
     * @english
     * @brief Gets a brief description of the command's purpose.
     * @return std::string Description of the hub command functionality.
     *
     * @russian
     * @brief Получает краткое описание назначения команды.
     * @return std::string Описание функциональности команды hub.
     */
    [[nodiscard]] std::string getDescription() const override;

    /**
     * @english
     * @brief Gets the usage syntax for the command.
     * @return std::string The usage pattern for the hub command.
     *
     * @russian
     * @brief Получает синтаксис использования для команды.
     * @return std::string Шаблон использования для команды hub.
     */
    [[nodiscard]] std::string getUsage() const override;

    /**
     * @english
     * @brief Executes the hub command with provided arguments.
     * @param args Command line arguments passed to the hub command.
     * @return bool True if command executed successfully, false otherwise.
     * @throws std::filesystem_error If filesystem operations fail.
     * @throws std::runtime_error If repository initialization fails.
     *
     * @russian
     * @brief Выполняет команду hub с предоставленными аргументами.
     * @param args Аргументы командной строки, переданные команде hub.
     * @return bool True если команда выполнена успешно, false в противном случае.
     * @throws std::filesystem_error Если операции файловой системы не удаются.
     * @throws std::runtime_error Если инициализация репозитория не удается.
     */
    bool execute(const std::vector<std::string>& args) override;

    /**
     * @english
     * @brief Displays help information for the hub command.
     * @details Shows usage examples, argument descriptions, and command purpose
     * to assist users in proper command usage.
     *
     * @russian
     * @brief Отображает справочную информацию для команды hub.
     * @details Показывает примеры использования, описания аргументов и назначение команды
     * для помощи пользователям в правильном использовании команды.
     */
    void showHelp() const override;

private:
    /**
     * @english
     * @brief Initializes a new hub repository at the specified path.
     * @param repo_path The filesystem path where to create the hub repository.
     * @return bool True if repository was successfully initialized, false otherwise.
     * @details Creates the complete directory structure and configuration files
     * required for a functional bare repository.
     *
     * @russian
     * @brief Инициализирует новый репозиторий-хаб по указанному пути.
     * @param repo_path Путь файловой системы, где создать репозиторий-хаб.
     * @return bool True если репозиторий успешно инициализирован, false в противном случае.
     * @details Создает полную структуру директорий и файлы конфигурации,
     * необходимые для функционального голого репозитория.
     */
    bool initializeHubRepository(const std::filesystem::path& repo_path) const;

    /**
     * @english
     * @brief Creates the directory structure for a hub repository.
     * @param svcs_path The path to the .svcs directory of the repository.
     * @return bool True if directory structure was created successfully.
     * @details Creates objects, refs/heads, refs/tags, hooks, and info directories.
     *
     * @russian
     * @brief Создает структуру директорий для репозитория-хаба.
     * @param svcs_path Путь к директории .svcs репозитория.
     * @return bool True если структура директорий создана успешно.
     * @details Создает директории objects, refs/heads, refs/tags, hooks и info.
     */
    bool createHubDirectoryStructure(const std::filesystem::path& svcs_path) const;

    /**
     * @english
     * @brief Creates the configuration file for the hub repository.
     * @param svcs_path The path to the .svcs directory of the repository.
     * @return bool True if config file was created successfully.
     * @details Creates a config file with repository format version,
     * file mode settings, and bare repository flag.
     *
     * @russian
     * @brief Создает файл конфигурации для репозитория-хаба.
     * @param svcs_path Путь к директории .svcs репозитория.
     * @return bool True если файл конфигурации создан успешно.
     * @details Создает файл конфигурации с версией формата репозитория,
     * настройками режима файлов и флагом голого репозитория.
     */
    bool createHubConfigFile(const std::filesystem::path& svcs_path) const;

    /**
     * @english
     * @brief Creates the HEAD file pointing to the default branch.
     * @param svcs_path The path to the .svcs directory of the repository.
     * @return bool True if HEAD file was created successfully.
     * @details Creates HEAD file that references the main branch as default.
     *
     * @russian
     * @brief Создает файл HEAD, указывающий на ветку по умолчанию.
     * @param svcs_path Путь к директории .svcs репозитория.
     * @return bool True если файл HEAD создан успешно.
     * @details Создает файл HEAD, который ссылается на основную ветку как на ветку по умолчанию.
     */
    bool createHubHEADFile(const std::filesystem::path& svcs_path) const;

    /**
     * @english
     * @brief Validates if the provided path is suitable for a hub repository.
     * @param path The filesystem path to validate.
     * @return bool True if path is valid for repository creation.
     * @details Checks if path is not empty and can be normalized to absolute path.
     *
     * @russian
     * @brief Проверяет, подходит ли предоставленный путь для репозитория-хаба.
     * @param path Путь файловой системы для проверки.
     * @return bool True если путь валиден для создания репозитория.
     * @details Проверяет, что путь не пуст и может быть нормализован в абсолютный путь.
     */
    [[nodiscard]] bool isValidHubPath(const std::filesystem::path& path) const;

    /**
     * @english
     * @brief Checks if the target path is available for repository creation.
     * @param path The filesystem path to check for availability.
     * @return bool True if path is available (doesn't exist or is empty directory).
     * @details Ensures we don't overwrite existing non-empty directories.
     *
     * @russian
     * @brief Проверяет, доступен ли целевой путь для создания репозитория.
     * @param path Путь файловой системы для проверки доступности.
     * @return bool True если путь доступен (не существует или является пустой директорией).
     * @details Гарантирует, что мы не перезапишем существующие непустые директории.
     */
    [[nodiscard]] bool isPathAvailable(const std::filesystem::path& path) const;

    /**
     * @english
     * @brief Sends an informational message through the event bus.
     * @param message The informational message to display.
     *
     * @russian
     * @brief Отправляет информационное сообщение через шину событий.
     * @param message Информационное сообщение для отображения.
     */
    void notifyInfo(const std::string& message) const;

    /**
     * @english
     * @brief Sends an error message through the event bus.
     * @param message The error message to display.
     *
     * @russian
     * @brief Отправляет сообщение об ошибке через шину событий.
     * @param message Сообщение об ошибке для отображения.
     */
    void notifyError(const std::string& message) const;

    /**
     * @english
     * @brief Sends a success message through the event bus.
     * @param message The success message to display.
     *
     * @russian
     * @brief Отправляет сообщение об успехе через шину событий.
     * @param message Сообщение об успехе для отображения.
     */
    void notifySuccess(const std::string& message) const;

private:
    /**
     * @english
     * @brief Event bus for user notifications and feedback.
     *
     * @russian
     * @brief Шина событий для уведомлений пользователя и обратной связи.
     */
    std::shared_ptr<ISubject> event_bus_;

    /**
     * @english
     * @brief Repository manager for core repository operations.
     *
     * @russian
     * @brief Менеджер репозитория для основных операций с репозиторием.
     */
    std::shared_ptr<RepositoryManager> repository_manager_;
};