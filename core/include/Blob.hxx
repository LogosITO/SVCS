/**
 * @file Blob.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines the Blob class, which represents raw file content in the VCS object database.
 * @details This file provides the contract for storing file data as immutable objects,
 * central to the data integrity of the system.
 *
 * @russian
 * @brief Определяет класс Blob, представляющий необработанное содержимое файла в базе данных объектов СКВ.
 * @details Этот файл предоставляет контракт для хранения данных файла как неизменяемых объектов,
 * что является центральным элементом обеспечения целостности данных системы.
 */
#pragma once

#include "VcsObject.hxx"
#include <string>

/**
 * @english
 * @class Blob
 * @brief Represents the content of a file (Binary Large Object).
 * @details This is the lowest-level object in the VCS, storing only raw file bytes.
 * The Blob object is immutable; its hash is solely dependent on its content.
 *
 * @russian
 * @class Blob
 * @brief Представляет содержимое файла (Binary Large Object).
 * @details Это объект самого низкого уровня в СКВ, хранящий только необработанные байты файла.
 * Объект Blob является неизменяемым; его хеш зависит исключительно от его содержимого.
 */
class Blob : public VcsObject {
private:
    /**
     * @english
     * @brief The raw content data of the file.
     *
     * @russian
     * @brief Необработанные данные содержимого файла.
     */
    std::string data;

public:
    /**
     * @english
     * @brief Constructor for the Blob object.
     * @details Initializes the object and immediately computes its hash ID based on the raw_data.
     * @param raw_data The raw file content to be stored and hashed.
     *
     * @russian
     * @brief Конструктор для объекта Blob.
     * @details Инициализирует объект и немедленно вычисляет его хеш ID на основе raw_data.
     * @param raw_data Необработанное содержимое файла, которое будет сохранено и хешировано.
     */
    Blob(std::string  raw_data);

    // VcsObject overrides

    /**
     * @english
     * @brief Returns the type of the VCS object.
     * @return std::string Always returns "blob".
     * @copydoc VcsObject::getType()
     *
     * @russian
     * @brief Возвращает тип объекта СКВ.
     * @return std::string Всегда возвращает "blob".
     */
    [[nodiscard]] std::string getType() const override;

    /**
     * @english
     * @brief Serializes the object's core data for hashing and storage.
     * @return std::string Returns the raw file content (data) itself.
     * @copydoc VcsObject::serialize()
     *
     * @russian
     * @brief Сериализует основные данные объекта для хеширования и хранения.
     * @return std::string Возвращает само необработанное содержимое файла (data).
     */
    [[nodiscard]] std::string serialize() const override;

    /**
     * @english
     * @brief Returns the raw data content stored in the Blob.
     * @return const std::string& A constant reference to the file content.
     *
     * @russian
     * @brief Возвращает необработанное содержимое данных, хранящееся в Blob.
     * @return const std::string& Константная ссылка на содержимое файла.
     */
    [[nodiscard]] const std::string& getData() const;
};