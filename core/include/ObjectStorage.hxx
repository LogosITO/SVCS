/**
 * @file ObjectStorage.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Manages the reading, writing, and lifecycle of VCS objects on the disk using the Git format.
 * @details This class handles file path construction, serialization, Zlib compression/decompression,
 * and acts as a factory for restoring concrete VcsObject types (Blob, Tree, Commit).
 * It uses an injected ISubject for internal event logging.
 *
 * @russian
 * @brief Управляет чтением, записью и жизненным циклом объектов СКВ на диске с использованием формата Git.
 * @details Этот класс обрабатывает построение путей к файлам, сериализацию, сжатие/распаковку Zlib
 * и действует как фабрика для восстановления конкретных типов VcsObject (Blob, Tree, Commit).
 * Использует внедренный ISubject для внутреннего логирования событий.
 */
#pragma once

#include "VcsObject.hxx"
#include <filesystem>
#include <string>
#include <memory>
#include <utility>
#include "../../services/ISubject.hxx"

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
 * @class ObjectStorage
 * @brief Manages the version control object database, including persistence and object restoration.
 * @details Implements low-level file system operations (path generation, reading, writing),
 * compression, decompression, and object deserialization.
 *
 * @russian
 * @class ObjectStorage
 * @brief Управляет базой данных объектов системы контроля версий, включая сохранение и восстановление объектов.
 * @details Реализует низкоуровневые операции файловой системы (генерация путей, чтение, запись),
 * сжатие, распаковку и десериализацию объектов.
 */
class ObjectStorage {
private:
    /**
     * @english
     * @brief The base path to the object directory (e.g., ".svcs/objects").
     *
     * @russian
     * @brief Базовый путь к директории объектов (например, ".svcs/objects").
     */
    const std::filesystem::path objects_dir;

    /**
     * @english
     * @brief Smart pointer to the ISubject interface for publishing internal events (e.g., success, error).
     *
     * @russian
     * @brief Умный указатель на интерфейс ISubject для публикации внутренних событий (например, успех, ошибка).
     */
    std::shared_ptr<ISubject> subject;

    /**
     * @english
     * @brief Compresses data using Zlib's raw deflate standard (Git format).
     * @param data The raw data (including header) to compress.
     * @return std::string The compressed binary data.
     * @throw std::runtime_error if Zlib compression fails.
     *
     * @russian
     * @brief Сжимает данные с использованием стандарта raw deflate Zlib (формат Git).
     * @param data Необработанные данные (включая заголовок) для сжатия.
     * @return std::string Сжатые бинарные данные.
     * @throw std::runtime_error если сжатие Zlib завершается неудачно.
     */
    [[nodiscard]] std::string compress(const std::string& data) const;

    /**
     * @english
     * @brief Decompresses Zlib raw deflate data.
     * @param compressed_data The compressed binary data.
     * @return std::string The decompressed data (including header).
     * @throw std::runtime_error if Zlib decompression fails or stream is corrupt.
     *
     * @russian
     * @brief Распаковывает данные Zlib raw deflate.
     * @param compressed_data Сжатые бинарные данные.
     * @return std::string Распакованные данные (включая заголовок).
     * @throw std::runtime_error если распаковка Zlib завершается неудачно или поток поврежден.
     */
    [[nodiscard]] std::string decompress(const std::string& compressed_data) const;

    /**
     * @english
     * @brief Object Factory: Creates a concrete VcsObject from its type and content.
     * @details This internal factory is responsible for mapping type strings to concrete VcsObject subclasses.
     * @param type The object type ("blob", "tree", or "commit").
     * @param content The object's serialized data (without header).
     * @return std::unique_ptr<VcsObject> A pointer to the newly created object.
     * @throw std::runtime_error if the type is unknown.
     *
     * @russian
     * @brief Фабрика объектов: Создает конкретный VcsObject из его типа и содержимого.
     * @details Эта внутренняя фабрика отвечает за сопоставление строк типов с конкретными подклассами VcsObject.
     * @param type Тип объекта ("blob", "tree" или "commit").
     * @param content Сериализованные данные объекта (без заголовка).
     * @return std::unique_ptr<VcsObject> Указатель на вновь созданный объект.
     * @throw std::runtime_error если тип неизвестен.
     */
    [[nodiscard]] std::unique_ptr<VcsObject> createObjectFromContent(
        const std::string& type,
        const std::string& content
    ) const;

public:
    /**
     * @english
     * @brief Constructor for ObjectStorage.
     * @param root_path The root path of the repository (e.g., the directory containing ".svcs").
     * @param subject Shared pointer to the ISubject interface for event logging. Defaults to nullptr if logging is not required yet.
     *
     * @russian
     * @brief Конструктор для ObjectStorage.
     * @param root_path Корневой путь репозитория (например, директория, содержащая ".svcs").
     * @param subject Общий указатель на интерфейс ISubject для логирования событий. По умолчанию nullptr, если логирование пока не требуется.
     */
    explicit ObjectStorage(const std::string& root_path, const std::shared_ptr<ISubject>& subject = nullptr);

    /**
     * @english
     * @brief Destructor.
     *
     * @russian
     * @brief Деструктор.
     */
    virtual ~ObjectStorage();

    /**
     * @english
     * @brief Sets or replaces the event subject used by the ObjectStorage.
     * @param subj The new ISubject pointer to use for publishing events.
     *
     * @russian
     * @brief Устанавливает или заменяет субъект событий, используемый ObjectStorage.
     * @param subj Новый указатель ISubject для использования при публикации событий.
     */
    void setSubject(std::shared_ptr<ISubject> subj) { subject = std::move(subj); };

    /**
     * @english
     * @brief Forms the full filesystem path for an object based on its hash.
     * @details Uses the first two characters of the hash for the subdirectory name
     * and the remaining 38 characters for the filename.
     * @param hash The full 40-character SHA-1 hash ID.
     * @return std::string The full, platform-agnostic file path.
     *
     * @russian
     * @brief Формирует полный путь в файловой системе для объекта на основе его хеша.
     * @details Использует первые два символа хеша для имени поддиректории
     * и оставшиеся 38 символов для имени файла.
     * @param hash Полный 40-символьный ID хеша SHA-1.
     * @return std::string Полный, независимый от платформы путь к файлу.
     */
    [[nodiscard]] std::string getObjectPath(const std::string& hash) const;

    /**
     * @english
     * @brief Saves a VcsObject to the object database.
     * @details Serializes the object, prefixes it with a header, compresses it, and writes it to a file.
     * Publishes events on success/failure.
     * @param obj The VcsObject to save.
     * @return bool True if saving was successful.
     * @throw std::runtime_error if hash is invalid or file IO fails.
     *
     * @russian
     * @brief Сохраняет VcsObject в базе данных объектов.
     * @details Сериализует объект, добавляет заголовок, сжимает его и записывает в файл.
     * Публикует события об успехе/неудаче.
     * @param obj VcsObject для сохранения.
     * @return bool True, если сохранение прошло успешно.
     * @throw std::runtime_error если хеш недействителен или операции ввода-вывода завершаются неудачно.
     */
    [[nodiscard]] virtual bool saveObject(const VcsObject& obj) const;

    /**
     * @english
     * @brief Loads an object from the disk by its hash ID.
     * @details Reads the compressed file, decompresses it, performs integrity checks, and deserializes the object
     * using the internal factory method.
     * @param hash The hash ID of the object to load.
     * @return std::unique_ptr<VcsObject> The restored object instance.
     * @throw std::runtime_error if the object is not found, corrupted, or invalid.
     *
     * @russian
     * @brief Загружает объект с диска по его ID хеша.
     * @details Читает сжатый файл, распаковывает его, выполняет проверки целостности и десериализует объект
     * с использованием внутреннего фабричного метода.
     * @param hash ID хеша объекта для загрузки.
     * @return std::unique_ptr<VcsObject> Восстановленный экземпляр объекта.
     * @throw std::runtime_error если объект не найден, поврежден или недействителен.
     */
    [[nodiscard]] virtual std::unique_ptr<VcsObject> loadObject(const std::string& hash) const;

    /**
     * @english
     * @brief Checks if an object with the given hash exists on disk.
     * @param hash The hash ID to check.
     * @return bool True if the object file exists.
     *
     * @russian
     * @brief Проверяет, существует ли объект с заданным хешем на диске.
     * @param hash ID хеша для проверки.
     * @return bool True, если файл объекта существует.
     */
    [[nodiscard]] bool objectExists(const std::string& hash) const;
};

}