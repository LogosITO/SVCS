/**
 * @file RemoteManager.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Declaration of the RemoteManager class and Remote struct.
 * @details This file defines the `Remote` structure to hold information about a single
 * remote repository and the `RemoteManager` class, which is responsible for
 * adding, removing, loading, and saving remote configurations from the
 * repository's config file.
 *
 * @russian
 * @brief Объявление класса RemoteManager и структуры Remote.
 * @details Этот файл определяет структуру `Remote` для хранения информации об одном
 * удаленном репозитории и класс `RemoteManager`, который отвечает за
 * добавление, удаление, загрузку и сохранение конфигураций удаленных репозиториев из
 * файла конфигурации репозитория.
 */
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <functional>

/**
 * @english
 * @namespace svcs::server
 * @brief Server-side components and administration commands.
 * @details Contains server management functionality including hub repository creation,
 * remote protocol handling, user management, and server administration commands.
 * This namespace encapsulates all server-specific operations that enable
 * multi-user collaboration and remote repository hosting.
 *
 * @russian
 * @namespace svcs::server
 * @brief Серверные компоненты и команды администрирования.
 * @details Содержит функциональность управления сервером, включая создание репозиториев-хабов,
 * обработку удаленных протоколов, управление пользователями и команды администрирования сервера.
 * Это пространство имен инкапсулирует все серверные операции, которые обеспечивают
 * многопользовательское сотрудничество и хостинг удаленных репозиториев.
 */
namespace svcs::server {

namespace fs = std::filesystem;

/**
 * @english
 * @struct Remote
 * @brief Represents a single remote repository configuration.
 * @details Stores the name, URL, and fetch specification (refspec) for a
 * configured remote. The fetch spec is automatically generated upon construction.
 *
 * @russian
 * @struct Remote
 * @brief Представляет конфигурацию одного удаленного репозитория.
 * @details Хранит имя, URL и спецификацию извлечения (refspec) для
 * настроенного удаленного репозитория. Спецификация извлечения автоматически генерируется при создании.
 */
struct Remote {
    /**
     * @english
     * @brief The shortname of the remote (e.g., "origin").
     *
     * @russian
     * @brief Короткое имя удаленного репозитория (например, "origin").
     */
    std::string name;

    /**
     * @english
     * @brief The URL (path or network address) of the remote.
     *
     * @russian
     * @brief URL (путь или сетевой адрес) удаленного репозитория.
     */
    std::string url;

    /**
     * @english
     * @brief The refspec defining what to fetch (e.g., "+refs/heads/*:refs/remotes/origin/*").
     *
     * @russian
     * @brief Refspec, определяющий что извлекать (например, "+refs/heads/*:refs/remotes/origin/*").
     */
    std::string fetch_spec;

    /**
     * @english
     * @brief Constructs a Remote struct.
     * @param name The shortname for the remote.
     * @param url The URL for the remote.
     *
     * @russian
     * @brief Конструирует структуру Remote.
     * @param name Короткое имя для удаленного репозитория.
     * @param url URL для удаленного репозитория.
     */
    Remote(const std::string& name, const std::string& url) :
        name(name), url(url), fetch_spec("+refs/heads/*:refs/remotes/" + name + "/*") {}
};

/**
 * @english
 * @class RemoteManager
 * @brief Manages the repository's remote configurations.
 * @details This class provides an interface to manage remote repositories.
 * It handles adding, removing, and listing remotes, as well as persisting
 * these configurations to and from the repository's `.svcs/config` file.
 *
 * @russian
 * @class RemoteManager
 * @brief Управляет конфигурациями удаленных репозиториев.
 * @details Этот класс предоставляет интерфейс для управления удаленными репозиториями.
 * Он обрабатывает добавление, удаление и перечисление удаленных репозиториев, а также сохранение
 * этих конфигураций в файл `.svcs/config` репозитория и загрузку из него.
 */
class RemoteManager {
private:
    /**
     * @english
     * @brief In-memory cache of remotes, keyed by name.
     *
     * @russian
     * @brief Кэш в памяти удаленных репозиториев, индексированный по имени.
     */
    std::unordered_map<std::string, Remote> remotes_;

    /**
     * @english
     * @brief Path to the repository's configuration file.
     *
     * @russian
     * @brief Путь к файлу конфигурации репозитория.
     */
    fs::path config_path_;

public:
    /**
     * @english
     * @brief Constructs a RemoteManager and loads existing remotes.
     * @param repo_path The root path of the repository (e.g., "/path/to/my-project"),
     * *not* the .svcs directory.
     * @details The constructor sets the path to the config file (repo_path / ".svcs" / "config")
     * and automatically calls `load()` to populate the remotes list.
     *
     * @russian
     * @brief Конструирует RemoteManager и загружает существующие удаленные репозитории.
     * @param repo_path Корневой путь репозитория (например, "/path/to/my-project"),
     * *не* директория .svcs.
     * @details Конструктор устанавливает путь к файлу конфигурации (repo_path / ".svcs" / "config")
     * и автоматически вызывает `load()` для заполнения списка удаленных репозиториев.
     */
    explicit RemoteManager(const fs::path& repo_path);

    /**
     * @english
     * @brief Adds a new remote configuration.
     * @param name The shortname for the new remote (e.g., "origin").
     * @param url The URL for the new remote.
     * @return bool True if the remote was added successfully, false if a remote
     * with that name already exists.
     *
     * @russian
     * @brief Добавляет новую конфигурацию удаленного репозитория.
     * @param name Короткое имя для нового удаленного репозитория (например, "origin").
     * @param url URL для нового удаленного репозитория.
     * @return bool True если удаленный репозиторий был успешно добавлен, false если удаленный репозиторий
     * с таким именем уже существует.
     */
    bool addRemote(const std::string& name, const std::string& url);

    /**
     * @english
     * @brief Removes a remote configuration by name.
     * @param name The shortname of the remote to remove.
     * @return bool True if the remote was found and removed, false otherwise.
     *
     * @russian
     * @brief Удаляет конфигурацию удаленного репозитория по имени.
     * @param name Короткое имя удаленного репозитория для удаления.
     * @return bool True если удаленный репозиторий был найден и удален, false в противном случае.
     */
    bool removeRemote(const std::string& name);

    /**
     * @english
     * @brief Checks if a remote with the given name exists.
     * @param name The name of the remote to check.
     * @return bool True if the remote exists, false otherwise.
     *
     * @russian
     * @brief Проверяет, существует ли удаленный репозиторий с заданным именем.
     * @param name Имя удаленного репозитория для проверки.
     * @return bool True если удаленный репозиторий существует, false в противном случае.
     */
    bool hasRemote(const std::string& name) const;

    /**
     * @english
     * @brief Gets the URL of a configured remote.
     * @param name The name of the remote.
     * @return std::string The URL if the remote is found, otherwise an empty string.
     *
     * @russian
     * @brief Получает URL настроенного удаленного репозитория.
     * @param name Имя удаленного репозитория.
     * @return std::string URL если удаленный репозиторий найден, иначе пустая строка.
     */
    std::string getRemoteUrl(const std::string& name) const;

    /**
     * @english
     * @brief Lists the names of all configured remotes.
     * @return std::vector<std::string> A vector containing the names of all remotes.
     *
     * @russian
     * @brief Перечисляет имена всех настроенных удаленных репозиториев.
     * @return std::vector<std::string> Вектор, содержащий имена всех удаленных репозиториев.
     */
    std::vector<std::string> listRemotes() const;

    /**
     * @english
     * @brief Saves the current remote configurations back to the config file.
     * @details This method overwrites the remote sections of the config file
     * with the current in-memory state.
     *
     * @russian
     * @brief Сохраняет текущие конфигурации удаленных репозиториев обратно в файл конфигурации.
     * @details Этот метод перезаписывает разделы удаленных репозиториев в файле конфигурации
     * текущим состоянием в памяти.
     */
    void save() const;

    /**
     * @english
     * @brief Loads (or reloads) the remote configurations from the config file.
     * @details This method clears the current in-memory cache and parses the
     * config file to populate it.
     *
     * @russian
     * @brief Загружает (или перезагружает) конфигурации удаленных репозиториев из файла конфигурации.
     * @details Этот метод очищает текущий кэш в памяти и анализирует
     * файл конфигурации для его заполнения.
     */
    void load();

    /**
     * @english
     * @brief Gets the number of configured remotes.
     * @return size_t The count of remotes.
     *
     * @russian
     * @brief Получает количество настроенных удаленных репозиториев.
     * @return size_t Количество удаленных репозиториев.
     */
    size_t count() const { return remotes_.size(); }
};

}