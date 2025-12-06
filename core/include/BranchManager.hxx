/**
 * @file BranchManager.hxx
 * @brief Declaration of the BranchManager class for handling version control branches.
 *
 * @english
 * @details The BranchManager is responsible for all operations related to managing
 * branches within the SVCS repository, including creation, deletion, renaming, and
 * switching. It manages branch metadata in memory and persists changes to the
 * underlying file system using utility methods for loading and saving state.
 *
 * @russian
 * @details BranchManager отвечает за все операции, связанные с управлением ветвями
 * в репозитории SVCS, включая создание, удаление, переименование и переключение. Он управляет
 * метаданными ветвей в памяти и сохраняет изменения в файловой системе, используя служебные
 * методы для загрузки и сохранения состояния.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "../../services/ISubject.hxx"

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <unordered_map>

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
 * @class BranchManager
 * @brief Core service for managing version control branches (creation, deletion, switching).
 * @details This class abstracts the details of branch storage and provides a
 * high-level interface for branch manipulation. It uses an internal map to
 * track branches and communicates status/errors via the ISubject event bus.
 *
 * @russian
 * @class BranchManager
 * @brief Основной сервис для управления ветвями системы контроля версий (создание, удаление, переключение).
 * @details Этот класс абстрагирует детали хранения ветвей и предоставляет высокоуровневый
 * интерфейс для манипулирования ветвями. Он использует внутреннюю карту для отслеживания ветвей
 * и передает статус/ошибки через шину событий ISubject.
 *
 * @ingroup Core
 */
class BranchManager {
public:
    /**
     * @english
     * @struct Branch
     * @brief Structure to hold the metadata for a single branch.
     *
     * @russian
     * @struct Branch
     * @brief Структура для хранения метаданных одной ветви.
     */
    struct Branch {
        /**
         * @english
         * @brief The unique name of the branch.
         *
         * @russian
         * @brief Уникальное имя ветви.
         */
        std::string name;

        /**
         * @english
         * @brief The hash of the latest commit on this branch.
         *
         * @russian
         * @brief Хеш последнего коммита в этой ветви.
         */
        std::string head_commit;

        /**
         * @english
         * @brief Flag indicating if this is the currently active branch.
         *
         * @russian
         * @brief Флаг, указывающий, является ли это текущей активной ветвью.
         */
        bool is_current;

        /**
         * @english
         * @brief Default constructor
         *
         * @russian
         * @brief Конструктор по умолчанию
         */
        Branch() : name(""), head_commit(""), is_current(false) {}

        /**
         * @english
         * @brief Constructor for the Branch structure.
         * @param name The branch name.
         * @param head_commit The hash of the commit this branch points to.
         * @param is_current Flag indicating if the branch is active.
         *
         * @russian
         * @brief Конструктор для структуры Branch.
         * @param name Имя ветви.
         * @param head_commit Хеш коммита, на который указывает эта ветвь.
         * @param is_current Флаг, указывающий, активна ли ветвь.
         */
        Branch(std::string  name, std::string  head_commit, bool is_current = false)
            : name(std::move(name)), head_commit(std::move(head_commit)), is_current(is_current) {}
    };

    /**
     * @english
     * @brief Constructs a BranchManager.
     * @param event_bus A shared pointer to the event bus (ISubject) for communication.
     *
     * @russian
     * @brief Конструирует BranchManager.
     * @param event_bus Общий указатель на шину событий (ISubject) для связи.
     */
    explicit BranchManager(std::shared_ptr<ISubject> event_bus);

    /**
     * @english
     * @brief Creates the initial set of default branches (e.g., 'main').
     *
     * @russian
     * @brief Создает начальный набор ветвей по умолчанию (например, 'main').
     */
    void createDefaultBranches();

    // --- Public Branch Operations / Публичные операции с ветвями ---

    /**
     * @english
     * @brief Creates a new branch pointing to a specific commit (usually HEAD).
     * @param name The name of the new branch.
     * @return \c true if the branch was created successfully, \c false otherwise.
     *
     * @russian
     * @brief Создает новую ветвь, указывающую на определенный коммит (обычно HEAD).
     * @param name Имя новой ветви.
     * @return \c true, если ветвь успешно создана, \c false в противном случае.
     */
    bool createBranch(const std::string& name);

    /**
     * @english
     * @brief Creates a new branch from a specific commit.
     * @param name Branch name.
     * @param commit_hash Hash of the commit to branch from.
     * @return true if successful, false otherwise.
     *
     * @russian
     * @brief Создает новую ветвь из определенного коммита.
     * @param name Имя ветви.
     * @param commit_hash Хеш коммита, от которого создается ветвь.
     * @return true, если успешно, false в противном случае.
     */
    bool createBranchFromCommit(const std::string& name, const std::string& commit_hash);

    /**
     * @english
     * @brief Get the current HEAD commit hash.
     * @return std::string The commit hash of current HEAD, or empty string if not found.
     *
     * @russian
     * @brief Получает хеш коммита текущего HEAD.
     * @return std::string Хеш коммита текущего HEAD, или пустая строка, если не найден.
     */
    std::string getHeadCommit();

    /**
     * @english
     * @brief Deletes an existing branch.
     * @details Prevents deletion of the current branch unless \c force is true.
     * @param name The name of the branch to delete.
     * @param force If \c true, forces deletion even if the branch is not fully merged (implementation dependent).
     * @return \c true if the branch was deleted successfully, \c false otherwise.
     *
     * @russian
     * @brief Удаляет существующую ветвь.
     * @details Предотвращает удаление текущей ветви, если только \c force не равно true.
     * @param name Имя ветви для удаления.
     * @param force Если \c true, принудительно удаляет, даже если ветвь не полностью слита (зависит от реализации).
     * @return \c true, если ветвь успешно удалена, \c false в противном случае.
     */
    bool deleteBranch(const std::string& name, bool force = false);

    /**
     * @english
     * @brief Renames an existing branch.
     * @param old_name The current name of the branch.
     * @param new_name The desired new name for the branch.
     * @return \c true if the branch was renamed successfully, \c false otherwise.
     *
     * @russian
     * @brief Переименовывает существующую ветвь.
     * @param old_name Текущее имя ветви.
     * @param new_name Желаемое новое имя для ветви.
     * @return \c true, если ветвь успешно переименована, \c false в противном случае.
     */
    bool renameBranch(const std::string& old_name, const std::string& new_name);

    /**
     * @english
     * @brief Switches the active branch to the specified name.
     * @details This typically involves updating the HEAD reference and checking out files
     * (the file checkout logic may reside in the RepositoryManager).
     * @param name The name of the branch to switch to.
     * @return \c true if the switch was successful, \c false otherwise.
     *
     * @russian
     * @brief Переключает активную ветвь на указанное имя.
     * @details Обычно это включает обновление ссылки HEAD и извлечение файлов
     * (логика извлечения файлов может находиться в RepositoryManager).
     * @param name Имя ветви для переключения.
     * @return \c true, если переключение было успешным, \c false в противном случае.
     */
    bool switchBranch(const std::string& name);

    // --- Public Branch Queries / Публичные запросы о ветвях ---

    /**
     * @english
     * @brief Retrieves a list of all known branches, including their state.
     * @return A vector of Branch structures.
     *
     * @russian
     * @brief Извлекает список всех известных ветвей, включая их состояние.
     * @return Вектор структур Branch.
     */
    std::vector<Branch> getAllBranches() const;

    /**
     * @english
     * @brief Retrieves the name of the currently active branch.
     * @return The string name of the current branch.
     *
     * @russian
     * @brief Извлекает имя текущей активной ветви.
     * @return Строковое имя текущей ветви.
     */
    std::string getCurrentBranch() const;

    /**
     * @english
     * @brief Updates the commit hash that a specific branch points to.
     * @param branch_name The name of the branch to update.
     * @param commit_hash The new commit hash.
     * @return \c true if the update was successful, \c false otherwise.
     *
     * @russian
     * @brief Обновляет хеш коммита, на который указывает указанная ветвь.
     * @param branch_name Имя ветви для обновления.
     * @param commit_hash Новый хеш коммита.
     * @return \c true, если обновление было успешным, \c false в противном случае.
     */
    bool updateBranchHead(const std::string& branch_name, const std::string& commit_hash);

    /**
     * @english
     * @brief Checks if a branch with the given name exists.
     * @param name The name of the branch to check.
     * @return \c true if the branch exists, \c false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли ветвь с заданным именем.
     * @param name Имя ветви для проверки.
     * @return \c true, если ветвь существует, \c false в противном случае.
     */
    bool branchExists(const std::string& name) const;

    /**
     * @english
     * @brief Retrieves the commit hash that a specific branch points to.
     * @param branch_name The name of the branch.
     * @return The HEAD commit hash of the branch, or an empty string if the branch doesn't exist.
     *
     * @russian
     * @brief Извлекает хеш коммита, на который указывает конкретная ветвь.
     * @param branch_name Имя ветви.
     * @return Хеш HEAD коммита ветви, или пустая строка, если ветвь не существует.
     */
    std::string getBranchHead(const std::string& branch_name) const;

    // --- Public Utility Methods / Публичные служебные методы ---

    /**
     * @english
     * @brief Checks if the given string is a valid name for a new branch.
     * @param name The string to validate.
     * @return \c true if the name is valid, \c false otherwise.
     *
     * @russian
     * @brief Проверяет, является ли заданная строка допустимым именем для новой ветви.
     * @param name Строка для проверки.
     * @return \c true, если имя допустимо, \c false в противном случае.
     */
    static bool isValidBranchName(const std::string& name);

    /**
     * @english
     * @brief Checks if a commit exists (implementation assumed to check the object store).
     * @param commit_hash Hash to check.
     * @return true if commit exists, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли коммит (реализация, предположительно, проверяет хранилище объектов).
     * @param commit_hash Хеш для проверки.
     * @return true, если коммит существует, false в противном случае.
     */
    static bool commitExists(const std::string& commit_hash);

private:
    // --- Private Persistence Methods / Приватные методы сохранения состояния ---

    /**
     * @english
     * @brief Loads branch data from the repository's file system into the internal map.
     *
     * @russian
     * @brief Загружает данные ветвей из файловой системы репозитория во внутреннюю карту.
     */
    void loadBranches();

    /**
     * @english
     * @brief Persists the current state of the internal branch map to the file system.
     *
     * @russian
     * @brief Сохраняет текущее состояние внутренней карты ветвей в файловую систему.
     */
    void saveBranches();

    /**
     * @english
     * @brief Saves a specific branch's head commit to its file on the file system.
     * @param branch_name The name of the branch.
     * @param commit_hash The commit hash to save.
     *
     * @russian
     * @brief Сохраняет хеш HEAD коммита конкретной ветви в ее файл в файловой системе.
     * @param branch_name Имя ветви.
     * @param commit_hash Хеш коммита для сохранения.
     */
    static void saveBranchToFile(const std::string &branch_name, const std::string &commit_hash);

    /**
     * @english
     * @brief Deletes the file associated with a specific branch on the file system.
     * @param branch_name The name of the branch to delete.
     *
     * @russian
     * @brief Удаляет файл, связанный с конкретной ветвью, в файловой системе.
     * @param branch_name Имя ветви для удаления.
     */
    static void deleteBranchFile(const std::string &branch_name);

    /**
     * @english
     * @brief Loads the name of the currently active branch from the file system (from HEAD).
     *
     * @russian
     * @brief Загружает имя текущей активной ветви из файловой системы (из HEAD).
     */
    void loadCurrentBranch();

    /**
     * @english
     * @brief Persists the name of the currently active branch to the file system (updates HEAD).
     * @return \c true if the operation was successful, \c false otherwise.
     *
     * @russian
     * @brief Сохраняет имя текущей активной ветви в файловую систему (обновляет HEAD).
     * @return \c true, если операция была успешной, \c false в противном случае.
     */
    bool saveCurrentBranch() const;

    // --- Private File System Abstraction Methods / Приватные методы абстракции файловой системы ---

    /**
     * @english
     * @brief Reads the entire content of a file.
     * @param path The path to the file.
     * @return The content of the file.
     *
     * @russian
     * @brief Считывает все содержимое файла.
     * @param path Путь к файлу.
     * @return Содержимое файла.
     */
    static std::string readFile(const std::string& path);

    /**
     * @english
     * @brief Writes content to a file, overwriting existing content.
     * @param path The path to the file.
     * @param content The string content to write.
     *
     * @russian
     * @brief Записывает содержимое в файл, перезаписывая существующее.
     * @param path Путь к файлу.
     * @param content Строковое содержимое для записи.
     */
    static void writeFile(const std::string& path, const std::string& content);

    /**
     * @english
     * @brief Checks if a file exists at the given path.
     * @param path The path to the file.
     * @return \c true if the file exists, \c false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли файл по заданному пути.
     * @param path Путь к файлу.
     * @return \c true, если файл существует, \c false в противном случае.
     */
    static bool fileExists(const std::string& path);

    /**
     * @english
     * @brief Creates a directory.
     * @param path The path of the directory to create.
     *
     * @russian
     * @brief Создает каталог.
     * @param path Путь к создаваемому каталогу.
     */
    static void createDirectory(const std::string& path);

    // --- Private Path Generation Methods / Приватные методы генерации путей ---

    /**
     * @english
     * @brief Generates the full path to the file where branch metadata is stored (usually a directory path for refs/heads/).
     * @return The file path string.
     *
     * @russian
     * @brief Генерирует полный путь к файлу, где хранятся метаданные ветви (обычно путь к каталогу для refs/heads/).
     * @return Строка пути к файлу.
     */
    static std::string getBranchesFilePath();

    /**
     * @english
     * @brief Generates the full path to the HEAD file, which indicates the current branch.
     * @return The file path string.
     *
     * @russian
     * @brief Генерирует полный путь к файлу HEAD, который указывает на текущую ветвь.
     * @return Строка пути к файлу.
     */
    static std::string getHeadFilePath();

    /**
     * @english
     * @brief Generates the full path to the directory containing branch data (e.g., .svcs/refs/heads).
     * @return The directory path string.
     *
     * @russian
     * @brief Генерирует полный путь к каталогу, содержащему данные ветвей (например, .svcs/refs/heads).
     * @return Строка пути к каталогу.
     */
    static std::string getBranchesDirectory();

    /**
     * @english
     * @brief Internal map storing all branches, indexed by branch name.
     *
     * @russian
     * @brief Внутренняя карта, хранящая все ветви, индексированные по имени ветви.
     */
    std::unordered_map<std::string, Branch> branches;

    /**
     * @english
     * @brief The name of the currently active branch.
     *
     * @russian
     * @brief Имя текущей активной ветви.
     */
    std::string current_branch;

    /**
     * @english
     * @brief Shared pointer to the event bus for communication.
     *
     * @russian
     * @brief Общий указатель на шину событий для связи.
     */
    std::shared_ptr<ISubject> event_bus;
};

}