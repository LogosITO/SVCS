/**
 * @file VcsObject.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Definition of the abstract base class for all Version Control System objects.
 * @details This class establishes the contract and core properties (like the hash ID)
 * required for any object stored in the SVCS object database (e.g., Commit, Blob, Tree).
 *
 * @russian
 * @brief Определение абстрактного базового класса для всех объектов системы контроля версий.
 * @details Этот класс устанавливает контракт и основные свойства (такие как хеш-идентификатор),
 * необходимые для любого объекта, хранящегося в базе данных объектов SVCS (например, Commit, Blob, Tree).
 */
#pragma once

#include <string>

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
 * @class VcsObject
 * @brief Abstract base class representing any storable, addressable object within the VCS.
 * @details VcsObject enforces the mandatory presence of a hash ID, serialization logic,
 * and type identification for all derived classes. It serves as the core domain
 * entity for the object database.
 *
 * @russian
 * @class VcsObject
 * @brief Абстрактный базовый класс, представляющий любой сохраняемый, адресуемый объект в СКВ.
 * @details VcsObject обеспечивает обязательное наличие хеш-идентификатора, логики сериализации
 * и идентификации типа для всех производных классов. Он служит основной доменной
 * сущностью для базы данных объектов.
 */
class VcsObject {
protected:
    /**
     * @english
     * @brief The unique cryptographic hash identifier (e.g., SHA-256) for this object.
     * @details Protected to allow internal setting via computeHash() but prevent external modification.
     *
     * @russian
     * @brief Уникальный криптографический хеш-идентификатор (например, SHA-256) для этого объекта.
     * @details Защищенный, чтобы разрешить внутреннюю установку через computeHash(), но предотвратить внешнее изменение.
     */
    std::string hash_id;

    /**
     * @english
     * @brief Computes the cryptographic hash ID for the object's content.
     * @details This method should prepend the object's type and content length (Git-style)
     * before hashing the full content and setting the internal hash_id.
     * @param content The serialized string data of the object (usually returned by serialize()).
     *
     * @russian
     * @brief Вычисляет криптографический хеш-идентификатор для содержимого объекта.
     * @details Этот метод должен добавлять тип объекта и длину содержимого (в стиле Git)
     * перед хешированием полного содержимого и установкой внутреннего hash_id.
     * @param content Сериализованные строковые данные объекта (обычно возвращаемые serialize()).
     */
    void computeHash(const std::string& content);

public:
    /**
     * @english
     * @brief Default constructor. Required for derived classes.
     *
     * @russian
     * @brief Конструктор по умолчанию. Требуется для производных классов.
     */
    VcsObject() = default;

    /**
     * @english
     * @brief Copy constructor. Required for derived classes to correctly copy the hash_id.
     * @param other The VcsObject to copy from.
     *
     * @russian
     * @brief Конструктор копирования. Требуется для производных классов для корректного копирования hash_id.
     * @param other Объект VcsObject для копирования.
     */
    VcsObject(const VcsObject& other) = default;

    /**
     * @english
     * @brief Virtual destructor.
     * @details Ensures proper cleanup when deleting derived objects via a base class pointer.
     *
     * @russian
     * @brief Виртуальный деструктор.
     * @details Обеспечивает правильную очистку при удалении производных объектов через указатель на базовый класс.
     */
    virtual ~VcsObject() = default;

    /**
     * @english
     * @brief Pure virtual method to serialize the object's core data.
     * @details The returned string content is what will be hashed and stored in the database.
     * @return std::string The standardized, serialized data of the object.
     *
     * @russian
     * @brief Чисто виртуальный метод для сериализации основных данных объекта.
     * @details Возвращаемое строковое содержимое будет хешировано и сохранено в базе данных.
     * @return std::string Стандартизированные, сериализованные данные объекта.
     */
    [[nodiscard]] virtual std::string serialize() const = 0;

    /**
     * @english
     * @brief Pure virtual method to get the type of the VCS object.
     * @details This is crucial for object storage and reconstruction (e.g., "commit", "blob", "tree").
     * @return std::string The type identifier of the object.
     *
     * @russian
     * @brief Чисто виртуальный метод для получения типа объекта СКВ.
     * @details Это критически важно для хранения объектов и их восстановления (например, "commit", "blob", "tree").
     * @return std::string Идентификатор типа объекта.
     */
    [[nodiscard]] virtual std::string getType() const = 0;

    /**
     * @english
     * @brief Calculates SHA-256 hash for given canonical content.
     * @param content Git-format object content (header + raw_data).
     * @return std::string Calculated 64-character hash.
     *
     * @russian
     * @brief Вычисляет SHA-256 хеш для заданного канонического содержимого.
     * @param content Содержимое объекта Git-формата (header + raw_data).
     * @return std::string Вычисленный 64-символьный хеш.
     */
    static std::string calculateHash(const std::string& content);

    /**
     * @english
     * @brief Returns the unique hash identifier of the object.
     * @return std::string The cryptographic hash ID.
     *
     * @russian
     * @brief Возвращает уникальный хеш-идентификатор объекта.
     * @return std::string Криптографический хеш-идентификатор.
     */
    [[nodiscard]] std::string getHashId() const;
};


/**
 * @english
 * @class TestableObject
 * @brief Helper class for testing the non-abstract methods of VcsObject.
 * @details This class implements all pure virtual methods of VcsObject with simple stubs,
 * allowing its instances to be created for unit testing the hashing logic
 * in VcsObject::computeHash.
 *
 * @russian
 * @class TestableObject
 * @brief Вспомогательный класс для тестирования неабстрактных методов VcsObject.
 * @details Этот класс реализует все чисто виртуальные методы VcsObject простыми заглушками,
 * позволяя создавать его экземпляры для модульного тестирования логики хеширования
 * в VcsObject::computeHash.
 */
class TestableObject : public VcsObject {
private:
    /**
     * @english
     * @brief The object type name returned by getType().
     *
     * @russian
     * @brief Имя типа объекта, возвращаемое getType().
     */
    std::string type_name;

    /**
     * @english
     * @brief The content returned by serialize().
     *
     * @russian
     * @brief Содержимое, возвращаемое serialize().
     */
    std::string content_data;

public:
    /**
     * @english
     * @brief Primary constructor.
     * @param type The name of the object type (e.g., "test_blob").
     * @param data The content that will be serialized and hashed.
     *
     * @russian
     * @brief Основной конструктор.
     * @param type Имя типа объекта (например, "test_blob").
     * @param data Содержимое, которое будет сериализовано и хешировано.
     */
    TestableObject(std::string  type, std::string  data);

    /**
     * @english
     * @brief Implementation of the pure virtual method. Returns the stored content.
     * @copydoc VcsObject::serialize()
     *
     * @russian
     * @brief Реализация чисто виртуального метода. Возвращает сохраненное содержимое.
     */
    [[nodiscard]] std::string serialize() const override;

    /**
     * @english
     * @brief Implementation of the pure virtual method. Returns the stored type name.
     * @copydoc VcsObject::getType()
     *
     * @russian
     * @brief Реализация чисто виртуального метода. Возвращает сохраненное имя типа.
     */
    [[nodiscard]] std::string getType() const override;
};

}