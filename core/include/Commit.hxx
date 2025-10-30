/**
 * @file Commit.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines the Commit class, representing a permanent snapshot in the VCS history.
 * @details A Commit object links a Tree object (the repository state) with parent commits
 * and essential metadata like author, timestamp, and the commit message, forming the basis of the project's history.
 *
 * @russian
 * @brief Определяет класс Commit, представляющий постоянный снимок в истории СКВ.
 * @details Объект Commit связывает объект Tree (состояние репозитория) с родительскими коммитами
 * и основными метаданными, такими как автор, временная метка и сообщение коммита, формируя основу истории проекта.
 */
#pragma once

#include "VcsObject.hxx"

#include <string>
#include <vector>
#include <ctime>

/**
 * @english
 * @class Commit
 * @brief Represents a single, immutable historical point (snapshot) in the repository's history.
 * @details This object binds the content structure (Tree hash) with the metadata (author, message, parent history).
 *
 * @russian
 * @class Commit
 * @brief Представляет единую, неизменяемую историческую точку (снимок) в истории репозитория.
 * @details Этот объект связывает структуру содержимого (хеш Tree) с метаданными (автор, сообщение, история родителей).
 */
class Commit : public VcsObject {
private:
    /**
     * @english
     * @brief Hash ID of the root Tree object representing the state of the repository at this commit.
     *
     * @russian
     * @brief Хеш ID корневого объекта Tree, представляющего состояние репозитория в этом коммите.
     */
    std::string tree_hash;

    /**
     * @english
     * @brief Hash IDs of the parent commits (usually one for normal commits, or two for merge commits).
     *
     * @russian
     * @brief Хеш ID родительских коммитов (обычно один для обычных коммитов или два для коммитов слияния).
     */
    std::vector<std::string> parent_hashes;

    /**
     * @english
     * @brief Name and email of the author (e.g., "John Doe <john.doe@example.com>").
     *
     * @russian
     * @brief Имя и email автора (например, "Иван Иванов <ivan.ivanov@example.com>").
     */
    std::string author;

    /**
     * @english
     * @brief Timestamp (UNIX time) of the commit creation.
     *
     * @russian
     * @brief Временная метка (UNIX время) создания коммита.
     */
    std::time_t timestamp;

    /**
     * @english
     * @brief The commit message provided by the user, often spanning multiple lines.
     *
     * @russian
     * @brief Сообщение коммита, предоставленное пользователем, часто занимающее несколько строк.
     */
    std::string message;

public:
    /**
     * @english
     * @brief Default constructor.
     *
     * @russian
     * @brief Конструктор по умолчанию.
     */
    Commit() : tree_hash(""), parent_hashes({}), author(""), timestamp(time_t()), message("") {};

    /**
     * @english
     * @brief Main constructor for the Commit object.
     * @details Initializes the metadata fields, sorts the parent hashes for consistency, and immediately computes the object's hash ID.
     * @param thash Hash of the root Tree object.
     * @param phashes Hashes of the immediate parent commit(s).
     * @param auth Author and email string.
     * @param msg User-defined commit message.
     * @param tstamp Time when the commit was created (default is current time).
     *
     * @russian
     * @brief Основной конструктор для объекта Commit.
     * @details Инициализирует поля метаданных, сортирует хеши родителей для согласованности и немедленно вычисляет хеш ID объекта.
     * @param thash Хеш корневого объекта Tree.
     * @param phashes Хеши непосредственных родительских коммитов.
     * @param auth Строка автора и email.
     * @param msg Пользовательское сообщение коммита.
     * @param tstamp Время создания коммита (по умолчанию текущее время).
     */
    Commit(
        std::string thash,
        std::vector<std::string> phashes,
        std::string auth,
        std::string msg,
        std::time_t tstamp = std::time(nullptr)
    );

    // VcsObject overrides

    /**
     * @english
     * @brief Returns the type of the VCS object.
     * @return std::string Always returns "commit".
     * @copydoc VcsObject::getType()
     *
     * @russian
     * @brief Возвращает тип объекта СКВ.
     * @return std::string Всегда возвращает "commit".
     */
    [[nodiscard]] std::string getType() const override;

    /**
     * @english
     * @brief Serializes the Commit metadata into a standardized, canonical format for hashing and storage.
     * @details The serialized format includes key/value pairs (tree, parent, author) followed by the message.
     * @return std::string The standardized, serialized commit data.
     * @copydoc VcsObject::serialize()
     *
     * @russian
     * @brief Сериализует метаданные Commit в стандартизированный канонический формат для хеширования и хранения.
     * @details Сериализованный формат включает пары ключ/значение (tree, parent, author), за которыми следует сообщение.
     * @return std::string Стандартизированные сериализованные данные коммита.
     */
    [[nodiscard]] std::string serialize() const override;

    /**
     * @english
     * @brief Creates a Commit object from a serialized string read from the object database.
     * @param raw_content The serialized string data, typically read from a decompressed object file.
     * @return Commit A fully reconstructed Commit object.
     *
     * @russian
     * @brief Создает объект Commit из сериализованной строки, прочитанной из базы данных объектов.
     * @param raw_content Сериализованные строковые данные, обычно прочитанные из распакованного файла объекта.
     * @return Commit Полностью восстановленный объект Commit.
     */
    static Commit deserialize(const std::string& raw_content);

    // Getters

    /**
     * @english
     * @brief Returns the hash ID of the associated root Tree object.
     * @return const std::string& The tree hash.
     *
     * @russian
     * @brief Возвращает хеш ID связанного корневого объекта Tree.
     * @return const std::string& Хеш дерева.
     */
    [[nodiscard]] const std::string& getTreeHash() const;

    /**
     * @english
     * @brief Returns the hash IDs of the parent commits.
     * @return const std::vector<std::string>& The list of parent hashes (sorted).
     *
     * @russian
     * @brief Возвращает хеш ID родительских коммитов.
     * @return const std::vector<std::string>& Список хешей родителей (отсортированный).
     */
    [[nodiscard]] const std::vector<std::string>& getParentHashes() const;

    /**
     * @english
     * @brief Returns the author and email string for the commit.
     * @return const std::string& The author string.
     *
     * @russian
     * @brief Возвращает строку автора и email для коммита.
     * @return const std::string& Строка автора.
     */
    [[nodiscard]] const std::string& getAuthor() const;

    /**
     * @english
     * @brief Returns the UNIX timestamp of the commit creation.
     * @return std::time_t The timestamp value.
     *
     * @russian
     * @brief Возвращает UNIX временную метку создания коммита.
     * @return std::time_t Значение временной метки.
     */
    [[nodiscard]] std::time_t getTimestamp() const;

    /**
     * @english
     * @brief Returns the user-defined commit message.
     * @return const std::string& The commit message.
     *
     * @russian
     * @brief Возвращает пользовательское сообщение коммита.
     * @return const std::string& Сообщение коммита.
     */
    [[nodiscard]] const std::string& getMessage() const;

    /**
     * @english
     * @brief Sets the commit message.
     * @param msg The new commit message.
     *
     * @russian
     * @brief Устанавливает сообщение коммита.
     * @param msg Новое сообщение коммита.
     */
    void setMessage(const std::string& msg);

    /**
     * @english
     * @brief Sets the commit timestamp.
     * @param tstamp The new timestamp value.
     *
     * @russian
     * @brief Устанавливает временную метку коммита.
     * @param tstamp Новое значение временной метки.
     */
    void setTimestamp(std::time_t tstamp);

    /**
     * @english
     * @brief Sets the commit timestamp to the current time.
     *
     * @russian
     * @brief Устанавливает временную метку коммита на текущее время.
     */
    void setTimestampNow();

    /**
     * @english
     * @brief Sets a parent commit hash.
     * @param parent The parent commit hash.
     *
     * @russian
     * @brief Устанавливает хеш родительского коммита.
     * @param parent Хеш родительского коммита.
     */
    void setParent(const std::string& parent);
};