/**
 * @file RepositoryManager.hxx
 * @brief Declaration of the RepositoryManager class, managing all repository-level file operations.
 *
 * @english
 * @details This class is the core service provider for the SVCS application, handling
 * all low-level file system interactions related to the repository structure, staging area,
 * commits, and history. It relies on the ISubject interface for all internal logging and
 * error reporting.
 *
 * @russian
 * @details Этот класс является основным поставщиком услуг для приложения SVCS, обрабатывая
 * все низкоуровневые взаимодействия с файловой системой, связанные со структурой репозитория,
 * областью подготовки, коммитами и историей. Он полагается на интерфейс ISubject для всего
 * внутреннего логирования и отчетов об ошибках.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../services/ISubject.hxx"

#include <optional>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

/**
 * @english
 * @namespace svcs::core
 * @brief Core VCS data structures and object model.
 * @details Contains fundamental VCS object types like Blob, Tree, Commit
 * that form the building blocks of the version control system.
 *
 * @russian
 * @namespace svcs::core
 * @brief Основные структуры данных СКВ и модель объектов.
 * @details Содержит фундаментальные типы объектов СКВ, такие как Blob, Tree, Commit,
 * которые формируют строительные блоки системы контроля версий.
 */
namespace svcs::core {

using namespace svcs::services;

/**
 * @english
 * @struct CommitInfo
 * @brief Structure containing essential metadata for a single commit.
 *
 * @russian
 * @struct CommitInfo
 * @brief Структура, содержащая основные метаданные для одного коммита.
 */
struct CommitInfo {
    /**
     * @english
     * @brief The unique identifier (hash) of the commit.
     *
     * @russian
     * @brief Уникальный идентификатор (хеш) коммита.
     */
    std::string hash;

    /**
     * @english
     * @brief The message provided by the user when creating the commit.
     *
     * @russian
     * @brief Сообщение, предоставленное пользователем при создании коммита.
     */
    std::string message;

    /**
     * @english
     * @brief The count of files included in this commit.
     *
     * @russian
     * @brief Количество файлов, включенных в этот коммит.
     */
    int files_count = 0;

    /**
     * @english
     * @brief The author or user who created the commit.
     *
     * @russian
     * @brief Автор или пользователь, создавший коммит.
     */
    std::string author;

    /**
     * @english
     * @brief Timestamp of when the commit was created.
     *
     * @russian
     * @brief Временная метка создания коммита.
     */
    std::string timestamp;

    /**
     * @english
     * @brief The branch this commit belongs to.
     *
     * @russian
     * @brief Ветвь, к которой принадлежит этот коммит.
     */
    std::string branch;
};

/**
 * @english
 * @class RepositoryManager
 * @brief Manages all physical file and directory operations within the SVCS repository.
 * @details The RepositoryManager is the core layer responsible for interacting directly with
 * the file system for tasks like initialization, path management, staging, committing, and logging.
 *
 * @russian
 * @class RepositoryManager
 * @brief Управляет всеми физическими операциями с файлами и директориями в репозитории SVCS.
 * @details RepositoryManager - это основной уровень, отвечающий за прямое взаимодействие с
 * файловой системой для таких задач, как инициализация, управление путями, подготовка, коммиты и логирование.
 *
 * @ingroup Core
 */
class RepositoryManager {
private:
    /**
     * @english
     * @brief The full path to the root of the currently active SVCS repository (.svcs directory).
     *
     * @russian
     * @brief Полный путь к корню текущего активного репозитория SVCS (директория .svcs).
     */
    std::string currentRepoPath;

    /**
     * @english
     * @brief Shared pointer to the event bus for internal logging and notifications.
     *
     * @russian
     * @brief Общий указатель на шину событий для внутреннего логирования и уведомлений.
     */
    std::shared_ptr<ISubject> eventBus;

    // --- Private Utility Methods for Logging / Приватные служебные методы для логирования ---

    /**
     * @english
     * @brief Notifies the event bus with an error message.
     * @param message The error message to be logged.
     *
     * @russian
     * @brief Уведомляет шину событий сообщением об ошибке.
     * @param message Сообщение об ошибке для логирования.
     */
    void logError(const std::string& message) const;

    /**
     * @english
     * @brief Notifies the event bus with a debug message.
     * @param message The debug message to be logged.
     *
     * @russian
     * @brief Уведомляет шину событий отладочным сообщением.
     * @param message Отладочное сообщение для логирования.
     */
    void logDebug(const std::string& message) const;

    /**
     * @english
     * @brief Notifies the event bus with an informational message.
     * @param message The information message to be logged.
     *
     * @russian
     * @brief Уведомляет шину событий информационным сообщением.
     * @param message Информационное сообщение для логирования.
     */
    void logInfo(const std::string& message) const;

    // --- Private Utility Methods for File System Operations / Приватные служебные методы для операций с файловой системой ---

    /**
     * @english
     * @brief Attempts to create a directory at the specified path.
     * @param path The filesystem path where the directory should be created.
     * @return \c true if creation succeeded or the directory already exists, \c false otherwise.
     *
     * @russian
     * @brief Пытается создать директорию по указанному пути.
     * @param path Путь в файловой системе, где должна быть создана директория.
     * @return \c true, если создание прошло успешно или директория уже существует, \c false в противном случае.
     */
    bool createDirectory(const std::filesystem::path& path) const;

    /**
     * @english
     * @brief Attempts to create a file at the specified path with optional content.
     * @param path The filesystem path where the file should be created.
     * @param content Optional string content to write to the file.
     * @return \c true if the file was created or opened successfully, \c false otherwise.
     *
     * @russian
     * @brief Пытается создать файл по указанному пути с опциональным содержимым.
     * @param path Путь в файловой системе, где должен быть создан файл.
     * @param content Опциональное строковое содержимое для записи в файл.
     * @return \c true, если файл был создан или открыт успешно, \c false в противном случае.
     */
    bool createFile(const std::filesystem::path& path, const std::string& content = "") const;

    /**
     * @english
     * @brief Gets the current branch name from HEAD file.
     * @return The current branch name.
     *
     * @russian
     * @brief Получает имя текущей ветви из файла HEAD.
     * @return Имя текущей ветви.
     */
    std::string getCurrentBranchFromHead() const;

public:
    /**
     * @english
     * @brief Constructs a RepositoryManager.
     * @param bus A shared pointer to the event bus (ISubject) for communication.
     *
     * @russian
     * @brief Конструирует RepositoryManager.
     * @param bus Общий указатель на шину событий (ISubject) для связи.
     */
    explicit RepositoryManager(std::shared_ptr<ISubject> bus);

    /**
     * @english
     * @brief Updates HEAD reference.
     * @param commit_hash The commit hash to set as HEAD.
     *
     * @russian
     * @brief Обновляет ссылку HEAD.
     * @param commit_hash Хеш коммита для установки в качестве HEAD.
     */
    void updateHead(const std::string& commit_hash) const;

    /**
     * @english
     * @brief Initializes a new SVCS repository in the specified path.
     * @details Creates the necessary internal directories and files (e.g., .svcs/).
     * @param path The directory path where the repository should be created.
     * @param force If true, forces initialization even if the directory exists (implementation dependent).
     * @return \c true if initialization was successful, \c false otherwise.
     *
     * @russian
     * @brief Инициализирует новый репозиторий SVCS по указанному пути.
     * @details Создает необходимые внутренние директории и файлы (например, .svcs/).
     * @param path Путь директории, где должен быть создан репозиторий.
     * @param force Если true, принудительно инициализирует, даже если директория существует (зависит от реализации).
     * @return \c true, если инициализация прошла успешно, \c false в противном случае.
     */
    bool initializeRepository(const std::string& path, bool force = false);

    /**
     * @english
     * @brief Checks if a SVCS repository is initialized in the given path or its parents.
     * @param path The path to start searching from (defaults to the current directory ".").
     * @return \c true if a repository structure is found, \c false otherwise.
     *
     * @russian
     * @brief Проверяет, инициализирован ли репозиторий SVCS по заданному пути или его родителям.
     * @param path Путь, с которого начинать поиск (по умолчанию текущая директория ".").
     * @return \c true, если найдена структура репозитория, \c false в противном случае.
     */
    bool isRepositoryInitialized(const std::string& path = ".");

    /**
     * @english
     * @brief Updates branch reference file with commit hash.
     * @param branchName The name of the branch to update.
     * @param commitHash The commit hash to set as branch head.
     *
     * @russian
     * @brief Обновляет файл ссылки ветви хешем коммита.
     * @param branchName Имя ветви для обновления.
     * @param commitHash Хеш коммита для установки в качестве головы ветви.
     */
    void updateBranchReference(const std::string& branchName, const std::string& commitHash) const;

    /**
     * @english
     * @brief Returns the determined root path of the currently active repository.
     * @return The string path to the repository root.
     *
     * @russian
     * @brief Возвращает определенный корневой путь текущего активного репозитория.
     * @return Строковый путь к корню репозитория.
     */
    [[nodiscard]] std::filesystem::path getRepositoryPath() const;

    // --- Staging and History Management Methods / Методы управления подготовкой и историей ---

    /**
     * @english
     * @brief Attempts to add a file to the staging area (index).
     * @details This typically involves reading the file and updating the index state.
     * @param filePath The path to the file to be staged.
     * @return \c true if the file was successfully added, \c false otherwise.
     *
     * @russian
     * @brief Пытается добавить файл в область подготовки (индекс).
     * @details Обычно это включает чтение файла и обновление состояния индекса.
     * @param filePath Путь к файлу для подготовки.
     * @return \c true, если файл успешно добавлен, \c false в противном случае.
     */
    bool addFileToStaging(const std::string& filePath);

    /**
     * @english
     * @brief Retrieves the hash of the current HEAD commit in the repository.
     * @return The commit hash string.
     *
     * @russian
     * @brief Извлекает хеш текущего коммита HEAD в репозитории.
     * @return Строка хеша коммита.
     */
    std::string getHeadCommit() const;

    /**
     * @english
     * @brief Creates a commit from the currently staged files.
     * @param message Commit message provided by the user.
     * @return The newly created commit hash if successful, empty string otherwise.
     *
     * @russian
     * @brief Создает коммит из текущих подготовленных файлов.
     * @param message Сообщение коммита, предоставленное пользователем.
     * @return Хеш вновь созданного коммита при успехе, пустая строка в противном случае.
     */
    std::string createCommit(const std::string& message);

    /**
     * @english
     * @brief Updates commit references when a commit is removed.
     * @param removedCommitHash The hash of the commit being removed.
     * @param newParentHash The new parent hash for dependent commits.
     *
     * @russian
     * @brief Обновляет ссылки коммитов при удалении коммита.
     * @param removedCommitHash Хеш удаляемого коммита.
     * @param newParentHash Новый хеш родителя для зависимых коммитов.
     */
    void updateCommitReferences(const std::string& removedCommitHash, const std::string& newParentHash) const;

    /**
     * @english
     * @brief Reverts the repository state to a previous commit.
     * @details This is typically used to undo the effects of a previous 'save' (commit).
     * @param commit_hash The hash of the commit to revert to (or the one before the commit to undo).
     * @return \c true if the revert was successful, \c false otherwise.
     *
     * @russian
     * @brief Откатывает состояние репозитория к предыдущему коммиту.
     * @details Обычно используется для отмены эффектов предыдущего 'save' (коммита).
     * @param commit_hash Хеш коммита, к которому нужно откатиться (или коммита перед тем, который нужно отменить).
     * @return \c true, если откат прошел успешно, \c false в противном случае.
     */
    bool revertCommit(const std::string& commit_hash);

    /**
     * @english
     * @brief Retrieves the CommitInfo structure for a given commit hash.
     * @param commit_hash The hash of the commit to retrieve.
     * @return An optional containing the CommitInfo if found, or std::nullopt otherwise.
     *
     * @russian
     * @brief Извлекает структуру CommitInfo для заданного хеша коммита.
     * @param commit_hash Хеш коммита для извлечения.
     * @return Опциональный объект, содержащий CommitInfo, если найден, или std::nullopt в противном случае.
     */
    std::optional<CommitInfo> getCommitByHash(const std::string& commit_hash) const;

    /**
     * @english
     * @brief Retrieves the hash of the parent commit for a given commit.
     * @param commit_hash The hash of the child commit.
     * @return The parent commit hash string (empty if it is the initial commit).
     *
     * @russian
     * @brief Извлекает хеш родительского коммита для заданного коммита.
     * @param commit_hash Хеш дочернего коммита.
     * @return Строка хеша родительского коммита (пустая, если это начальный коммит).
     */
    std::string getParentCommitHash(const std::string& commit_hash) const;

    /**
     * @english
     * @brief Generates a commit hash from content.
     * @param content The content to hash.
     * @return The generated commit hash.
     *
     * @russian
     * @brief Генерирует хеш коммита из содержимого.
     * @param content Содержимое для хеширования.
     * @return Сгенерированный хеш коммита.
     */
    static std::string generateCommitHash(const std::string& content);

    /**
     * @english
     * @brief Restores the working directory files to the state recorded in a specific commit.
     * @param commit The CommitInfo structure representing the target state.
     * @return \c true if files were restored successfully, \c false otherwise.
     *
     * @russian
     * @brief Восстанавливает файлы рабочей директории до состояния, записанного в конкретном коммите.
     * @param commit Структура CommitInfo, представляющая целевое состояние.
     * @return \c true, если файлы успешно восстановлены, \c false в противном случае.
     */
    bool restoreFilesFromCommit(const CommitInfo& commit) const;

    /**
     * @english
     * @brief Clears the contents of the staging area (index).
     * @return \c true if the staging area was successfully cleared, \c false otherwise.
     *
     * @russian
     * @brief Очищает содержимое области подготовки (индекса).
     * @return \c true, если область подготовки успешно очищена, \c false в противном случае.
     */
    bool clearStagingArea();

    /**
     * @english
     * @brief Creates a commit from staged changes. (Convenience method often used by the SaveCommand).
     * @param message Commit message.
     * @return \c true if the save (commit) was successful, \c false otherwise.
     *
     * @russian
     * @brief Создает коммит из подготовленных изменений. (Удобный метод, часто используемый SaveCommand).
     * @param message Сообщение коммита.
     * @return \c true, если сохранение (коммит) прошло успешно, \c false в противном случае.
     */
    bool saveStagedChanges(const std::string& message);

    /**
     * @english
     * @brief Retrieves a list of all files currently marked as staged (indexed).
     * @return A vector of strings containing the file paths in the staging area.
     *
     * @russian
     * @brief Извлекает список всех файлов, в настоящее время помеченных как подготовленные (индексированные).
     * @return Вектор строк, содержащий пути к файлам в области подготовки.
     */
    std::vector<std::string> getStagedFiles();

    /**
     * @english
     * @brief Retrieves the complete commit history for the current branch.
     * @return Vector of CommitInfo objects representing the commit history, typically the newest first.
     *
     * @russian
     * @brief Извлекает полную историю коммитов для текущей ветви.
     * @return Вектор объектов CommitInfo, представляющих историю коммитов, обычно от самых новых к старым.
     */
    std::vector<CommitInfo> getCommitHistory() const;

    /**
     * @english
     * @brief Retrieves the commit history for a specific branch.
     * @param branch_name The name of the branch.
     * @return Vector of CommitInfo objects for the branch.
     *
     * @russian
     * @brief Извлекает историю коммитов для конкретной ветви.
     * @param branch_name Имя ветви.
     * @return Вектор объектов CommitInfo для ветви.
     */
    std::vector<CommitInfo> getBranchHistory(const std::string& branch_name) const;

    /**
     * @english
     * @brief Gets the current branch name.
     * @return The current branch name.
     *
     * @russian
     * @brief Получает имя текущей ветви.
     * @return Имя текущей ветви.
     */
    std::string getCurrentBranch() const;

    /**
     * @english
     * @brief Attempts to recursively remove the entire SVCS repository structure (e.g., the .svcs directory) and its contents.
     * @param path The root path of the repository to remove.
     * @return \c true if removal was successful, \c false otherwise.
     *
     * @russian
     * @brief Пытается рекурсивно удалить всю структуру репозитория SVCS (например, директорию .svcs) и ее содержимое.
     * @param path Корневой путь удаляемого репозитория.
     * @return \c true, если удаление прошло успешно, \c false в противном случае.
     */
    bool removeRepository(const std::filesystem::path& path) const;

    /**
     * @english
     * @brief Checks if a branch exists.
     * @param branch_name The branch name to check.
     * @return true if branch exists, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли ветвь.
     * @param branch_name Имя ветви для проверки.
     * @return true, если ветвь существует, false в противном случае.
     */
    bool branchExists(const std::string& branch_name) const;

    /**
     * @english
     * @brief Gets the head commit of a branch.
     * @param branch_name The branch name.
     * @return The commit hash of the branch head.
     *
     * @russian
     * @brief Получает головной коммит ветви.
     * @param branch_name Имя ветви.
     * @return Хеш коммита головы ветви.
     */
    std::string getBranchHead(const std::string& branch_name) const;

    /**
     * @english
     * @brief Gets the content of a file at a specific commit.
     * @param commit_hash The commit hash.
     * @param file_path The file path.
     * @return The file content.
     *
     * @russian
     * @brief Получает содержимое файла в конкретном коммите.
     * @param commit_hash Хеш коммита.
     * @param file_path Путь к файлу.
     * @return Содержимое файла.
     */
    std::string getFileContentAtCommit(const std::string& commit_hash, const std::string& file_path) const;

    /**
     * @english
     * @brief Gets all files changed in a commit.
     * @param commit_hash The commit hash.
     * @return Vector of file paths.
     *
     * @russian
     * @brief Получает все файлы, измененные в коммите.
     * @param commit_hash Хеш коммита.
     * @return Вектор путей к файлам.
     */
    std::vector<std::string> getCommitFiles(const std::string& commit_hash) const;

    /**
     * @english
     * @brief Sets merge state (for conflict resolution).
     * @param branch_name Branch being merged.
     * @param commit_hash Commit being merged.
     *
     * @russian
     * @brief Устанавливает состояние слияния (для разрешения конфликтов).
     * @param branch_name Ветвь, которая сливается.
     * @param commit_hash Коммит, который сливается.
     */
    void setMergeState(const std::string& branch_name, const std::string& commit_hash) const;

    /**
     * @english
     * @brief Clears merge state (after merge completion/abort).
     *
     * @russian
     * @brief Очищает состояние слияния (после завершения/прерывания слияния).
     */
    void clearMergeState() const;

    /**
     * @english
     * @brief Checks if a merge is in progress.
     * @return true if merge is in progress, false otherwise.
     *
     * @russian
     * @brief Проверяет, выполняется ли слияние.
     * @return true, если слияние выполняется, false в противном случае.
     */
    bool isMergeInProgress() const;

    /**
     * @english
     * @brief Gets the branch being merged.
     * @return Branch name if merge in progress, empty string otherwise.
     *
     * @russian
     * @brief Получает ветвь, которая сливается.
     * @return Имя ветви, если слияние выполняется, пустая строка в противном случае.
     */
    std::string getMergeBranch() const;
};

}