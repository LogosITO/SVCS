/**
 * @file Tree.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines the Tree class, which represents the hierarchical structure (directory state) of a VCS repository.
 * @details The Tree object stores a list of TreeEntry structures, linking file names and modes to the hash IDs of
 * Blob or other Tree objects. Its hash depends critically on the sorted content of these entries.
 *
 * @russian
 * @brief Определяет класс Tree, представляющий иерархическую структуру (состояние директории) репозитория СКВ.
 * @details Объект Tree хранит список структур TreeEntry, связывающих имена файлов и режимы с хеш-идентификаторами
 * объектов Blob или других Tree объектов. Его хеш критически зависит от отсортированного содержимого этих записей.
 */
#pragma once

#include "VcsObject.hxx"
#include <optional>
#include <string>
#include <vector>

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

/**
 * @english
 * @struct TreeEntry
 * @brief Represents a single item (file or subdirectory) within a Tree object.
 * @details This structure links a name and file mode to the hash ID of another VCS object.
 *
 * @russian
 * @struct TreeEntry
 * @brief Представляет отдельный элемент (файл или поддиректорию) внутри объекта Tree.
 * @details Эта структура связывает имя и режим файла с хеш-идентификатором другого объекта СКВ.
 */
struct TreeEntry {
    /**
     * @english
     * @brief File mode (permissions) and type identifier (e.g., "100644" for a blob, "040000" for a tree).
     *
     * @russian
     * @brief Режим файла (права доступа) и идентификатор типа (например, "100644" для blob, "040000" для tree).
     */
    std::string mode;

    /**
     * @english
     * @brief The name of the file or subdirectory.
     *
     * @russian
     * @brief Имя файла или поддиректории.
     */
    std::string name;

    /**
     * @english
     * @brief The hash ID (SHA-256) of the referenced object (Blob or Tree).
     *
     * @russian
     * @brief Хеш-идентификатор (SHA-256) ссылаемого объекта (Blob или Tree).
     */
    std::string hash_id;

    /**
     * @english
     * @brief The type of the referenced object ("blob" or "tree").
     *
     * @russian
     * @brief Тип ссылаемого объекта ("blob" или "tree").
     */
    std::string type;

    /**
     * @english
     * @brief Comparison operator required for sorting.
     * @details Entries must be sorted alphabetically by name before serialization to ensure a stable and consistent hash ID for the Tree object.
     * @param other The TreeEntry to compare against.
     * @return bool True if this entry should precede the 'other' entry in the sorted list.
     *
     * @russian
     * @brief Оператор сравнения, необходимый для сортировки.
     * @details Записи должны быть отсортированы по алфавиту по имени перед сериализацией для обеспечения стабильного и последовательного хеш-идентификатора объекта Tree.
     * @param other Запись TreeEntry для сравнения.
     * @return bool True, если эта запись должна предшествовать записи 'other' в отсортированном списке.
     */
    bool operator<(const TreeEntry& other) const;
};

/**
 * @english
 * @brief Creating tree entry.
 * @return Returns finished TreeEntry.
 * @param name Name of file or subdirectory.
 * @param hash_id The hash ID (SHA-256).
 *
 * @russian
 * @brief Создание записи дерева.
 * @return Возвращает готовую запись TreeEntry.
 * @param name Имя файла или поддиректории.
 * @param hash_id Хеш-идентификатор (SHA-256).
 */
TreeEntry createEntry(std::string name, std::string hash_id);

/**
 * @english
 * @class Tree
 * @brief Represents the state of a directory in the VCS, implementing the VcsObject contract.
 * @details Stores a collection of TreeEntry objects, effectively mapping names to hash pointers.
 *
 * @russian
 * @class Tree
 * @brief Представляет состояние директории в СКВ, реализуя контракт VcsObject.
 * @details Хранит коллекцию объектов TreeEntry, эффективно отображая имена на хеш-указатели.
 */
class Tree : public VcsObject {
private:
    /**
     * @english
     * @brief The list of all items contained within this directory.
     *
     * @russian
     * @brief Список всех элементов, содержащихся в этой директории.
     */
    std::vector<TreeEntry> entries;

public:
    /**
     * @english
     * @brief Constructor for the Tree object.
     * @details The constructor must internally sort the entries before calling computeHash().
     * @param entries A collection of TreeEntry structures representing the directory content.
     *
     * @russian
     * @brief Конструктор для объекта Tree.
     * @details Конструктор должен внутренне отсортировать записи перед вызовом computeHash().
     * @param entries Коллекция структур TreeEntry, представляющих содержимое директории.
     */
    explicit Tree(std::vector<TreeEntry> entries);

    // VcsObject overrides

    /**
     * @english
     * @brief Returns the type of the VCS object.
     * @return std::string Always returns "tree".
     * @copydoc VcsObject::getType()
     *
     * @russian
     * @brief Возвращает тип объекта СКВ.
     * @return std::string Всегда возвращает "tree".
     */
    [[nodiscard]] std::string getType() const override;

    /**
     * @english
     * @brief Serializes the Tree content into a standardized string format for hashing and storage.
     * @details The serialization must ensure the entries are sorted to guarantee hash consistency.
     * @return std::string The standardized, serialized directory structure.
     * @copydoc VcsObject::serialize()
     *
     * @russian
     * @brief Сериализует содержимое Tree в стандартизированный строковый формат для хеширования и хранения.
     * @details Сериализация должна гарантировать, что записи отсортированы для обеспечения согласованности хеша.
     * @return std::string Стандартизированная, сериализованная структура директории.
     */
    [[nodiscard]] std::string serialize() const override;

    /**
     * @english
     * @brief Creates a Tree object from a serialized string read from the object database.
     * @param raw_content The serialized string data, typically read from a decompressed object file.
     * @return Tree A fully reconstructed Tree object.
     *
     * @russian
     * @brief Создает объект Tree из сериализованной строки, прочитанной из базы данных объектов.
     * @param raw_content Сериализованные строковые данные, обычно прочитанные из распакованного файла объекта.
     * @return Tree Полностью восстановленный объект Tree.
     */
    static Tree deserialize(const std::string& raw_content);

    /**
     * @english
     * @brief Adds a new entry to the Tree or updates an existing entry if the name matches.
     * @details If the entries list is modified, the Tree's hash becomes invalid and must be recomputed before saving.
     * @param entry The TreeEntry to add or update.
     *
     * @russian
     * @brief Добавляет новую запись в Tree или обновляет существующую запись, если имя совпадает.
     * @details Если список записей изменен, хеш Tree становится недействительным и должен быть пересчитан перед сохранением.
     * @param entry Запись TreeEntry для добавления или обновления.
     */
    void addEntry(const TreeEntry& entry);

    /**
     * @english
     * @brief Removes an entry (file or subdirectory) from the Tree by name.
     * @param name The name of the entry to remove.
     * @return bool True if the entry was found and removed, otherwise false.
     *
     * @russian
     * @brief Удаляет запись (файл или поддиректорию) из Tree по имени.
     * @param name Имя записи для удаления.
     * @return bool True, если запись найдена и удалена, иначе false.
     */
    bool removeEntry(const std::string& name);

    /**
     * @english
     * @brief Searches for a specific entry within the Tree by name.
     * @param name The name of the entry to find.
     * @return std::optional<TreeEntry> An optional containing the entry if found, otherwise empty.
     *
     * @russian
     * @brief Ищет конкретную запись внутри Tree по имени.
     * @param name Имя записи для поиска.
     * @return std::optional<TreeEntry> Опциональное значение, содержащее запись, если найдена, иначе пустое.
     */
    [[nodiscard]] std::optional<TreeEntry> findEntry(const std::string& name) const;

    /**
     * @english
     * @brief Returns the list of entries stored in the Tree.
     * @return std::vector<TreeEntry>& A constant reference to the internal vector of entries.
     *
     * @russian
     * @brief Возвращает список записей, хранящихся в Tree.
     * @return std::vector<TreeEntry>& Константная ссылка на внутренний вектор записей.
     */
    [[nodiscard]] const std::vector<TreeEntry>& getEntries() const;
};

}