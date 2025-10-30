/**
 * @file Index.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Definition of IndexEntry data structure and Index struct
 *
 * @russian
 * @brief Определение структуры данных IndexEntry и структуры Index
 */
#pragma once

#include "../include/ObjectStorage.hxx"

#include <filesystem>
#include <string>
#include <map>

namespace fs = std::filesystem;

/**
 * @english
 * @brief Represents a single file entry in the staging area (Index).
 * @details An IndexEntry stores information about a file that is staged for the next commit,
 * including its object ID (hash) and file metadata used to detect modifications.
 *
 * @russian
 * @brief Представляет одну запись файла в области подготовки (Index).
 * @details IndexEntry хранит информацию о файле, который подготовлен для следующего коммита,
 * включая его ID объекта (хеш) и метаданные файла, используемые для обнаружения изменений.
 */
struct IndexEntry {
    /**
     * @english
     * @brief The SHA-256 hash of the file's content (the Blob object ID).
     *
     * @russian
     * @brief Хеш SHA-256 содержимого файла (ID объекта Blob).
     */
    std::string blob_hash;

    /**
     * @english
     * @brief The file's path, relative to the repository's root directory.
     *
     * @russian
     * @brief Путь к файлу относительно корневой директории репозитория.
     */
    fs::path file_path;

    /**
     * @english
     * @brief The time of the file's last modification, used to detect changes.
     *
     * @russian
     * @brief Время последнего изменения файла, используется для обнаружения изменений.
     */
    fs::file_time_type last_modified;

    /**
     * @english
     * @brief The size of the file in bytes.
     *
     * @russian
     * @brief Размер файла в байтах.
     */
    long long file_size = 0;
};

/**
 * @english
 * @brief Manages the Staging Area (Index) of the VCS repository.
 * @details The Index class is responsible for reading and writing the list of staged files
 * to the index file (.svcs/index), managing the 'svcs add' operation, and providing
 * the necessary structure for commit creation.
 *
 * @russian
 * @brief Управляет областью подготовки (Index) репозитория СКВ.
 * @details Класс Index отвечает за чтение и запись списка подготовленных файлов
 * в файл индекса (.svcs/index), управление операцией 'svcs add' и предоставление
 * необходимой структуры для создания коммитов.
 */
class Index {
private:
    /**
     * @english
     * @brief The map holding all staged entries, keyed by file_path.
     *
     * @russian
     * @brief Карта, содержащая все подготовленные записи, индексированные по file_path.
     */
    std::map<fs::path, IndexEntry> entries;

    /**
     * @english
     * @brief The full path to the index file (e.g., .svcs/index).
     *
     * @russian
     * @brief Полный путь к файлу индекса (например, .svcs/index).
     */
    fs::path index_file_path;

    /**
     * @english
     * @brief The root path of the working directory.
     *
     * @russian
     * @brief Корневой путь рабочей директории.
     */
    fs::path repo_root_path;

    /**
     * @english
     * @brief Reference to the ObjectStorage manager for saving Blobs.
     *
     * @russian
     * @brief Ссылка на менеджер ObjectStorage для сохранения Blob'ов.
     */
    ObjectStorage& storage_;

    /**
     * @english
     * @brief Retrieves the size and last write time of a file from the disk.
     * @param full_path The absolute path to the file.
     * @param size Reference to store the file size.
     * @param mtime Reference to store the last modified time.
     *
     * @russian
     * @brief Извлекает размер и время последней записи файла с диска.
     * @param full_path Абсолютный путь к файлу.
     * @param size Ссылка для хранения размера файла.
     * @param mtime Ссылка для хранения времени последнего изменения.
     */
    static void getFileMetaData(const fs::path& full_path,
        long long& size, fs::file_time_type& mtime);

public:
    /**
     * @english
     * @brief Constructs the Index manager, setting up paths and loading the index.
     * @param vcs_root_path The path to the VCS control directory (e.g., .svcs).
     * @param repo_root_path The path to the repository's working directory root.
     * @param storage Reference to the ObjectStorage instance.
     *
     * @russian
     * @brief Конструирует менеджер Index, настраивая пути и загружая индекс.
     * @param vcs_root_path Путь к управляющей директории СКВ (например, .svcs).
     * @param repo_root_path Путь к корню рабочей директории репозитория.
     * @param storage Ссылка на экземпляр ObjectStorage.
     */
    Index(const fs::path& vcs_root_path, fs::path  repo_root_path, ObjectStorage& storage);

    /**
     * @english
     * @brief Destructor.
     *
     * @russian
     * @brief Деструктор.
     */
    ~Index();

    /**
     * @english
     * @brief Adds or updates an entry in the staging area.
     * @param entry The IndexEntry object to be added or updated.
     *
     * @russian
     * @brief Добавляет или обновляет запись в области подготовки.
     * @param entry Объект IndexEntry для добавления или обновления.
     */
    void addEntry(const IndexEntry& entry);

    /**
     * @english
     * @brief Retrieves an entry from the index by its relative file path.
     * @param file_path The relative path of the file.
     * @return Pointer to the constant IndexEntry, or nullptr if not found.
     *
     * @russian
     * @brief Извлекает запись из индекса по относительному пути файла.
     * @param file_path Относительный путь файла.
     * @return Указатель на константный IndexEntry или nullptr, если не найден.
     */
    [[nodiscard]] const IndexEntry* getEntry(const fs::path& file_path) const;

    /**
     * @english
     * @brief Loads the index entries from the index file on disk (temporary text format).
     * @throw std::runtime_error If the index file cannot be read.
     *
     * @russian
     * @brief Загружает записи индекса из файла индекса на диске (временный текстовый формат).
     * @throw std::runtime_error Если файл индекса не может быть прочитан.
     */
    void load();

    /**
     * @english
     * @brief Saves the current state of the index entries to the index file (temporary text format).
     * @throw std::runtime_error If the index file cannot be written.
     *
     * @russian
     * @brief Сохраняет текущее состояние записей индекса в файл индекса (временный текстовый формат).
     * @throw std::runtime_error Если файл индекса не может быть записан.
     */
    void save() const;

    /**
     * @english
     * @brief Another saving function (like load) but returns bool.
     * @throw std::runtime_error If the index file cannot be written.
     *
     * @russian
     * @brief Еще одна функция сохранения (как load), но возвращает bool.
     * @throw std::runtime_error Если файл индекса не может быть записан.
     */
    [[nodiscard]] bool write() const;

    /**
     * @english
     * @brief Checks if a file in the working directory has been modified since it was last staged.
     * @param file_path The relative path of the file to check.
     * @return true If the file is modified, deleted, or untracked; false otherwise.
     *
     * @russian
     * @brief Проверяет, был ли файл в рабочей директории изменен с момента последней подготовки.
     * @param file_path Относительный путь проверяемого файла.
     * @return true Если файл изменен, удален или не отслеживается; false в противном случае.
     */
    [[nodiscard]] bool isFileModified(const fs::path& file_path) const;

    /**
     * @english
     * @brief Stages a file by creating a Blob object, saving it to storage, and
     * updating the Index entry (Core logic of 'svcs add').
     * @param relative_path The path to the file, relative to the repository's root directory.
     * @throw std::runtime_error If the path is invalid or if read/write operations fail.
     *
     * @russian
     * @brief Подготавливает файл, создавая объект Blob, сохраняя его в хранилище и
     * обновляя запись Index (Основная логика 'svcs add').
     * @param relative_path Путь к файлу относительно корневой директории репозитория.
     * @throw std::runtime_error Если путь недействителен или операции чтения/записи завершаются неудачно.
     */
    void stage_file(const fs::path& relative_path);

    /**
     * @english
     * @brief Creates the hierarchical Tree structure based on the current staged entries
     * and saves the Tree objects to ObjectStorage.
     * @return The SHA-1 hash (ID) of the final root Tree object.
     * @throw std::runtime_error If the index is empty or tree creation fails.
     *
     * @russian
     * @brief Создает иерархическую структуру Tree на основе текущих подготовленных записей
     * и сохраняет объекты Tree в ObjectStorage.
     * @return Хеш SHA-1 (ID) конечного корневого объекта Tree.
     * @throw std::runtime_error Если индекс пуст или создание дерева завершается неудачно.
     */
    std::string createTreeObject();
};