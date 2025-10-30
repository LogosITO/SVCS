/**
 * @file RemoteManager.hxx
 * @brief Declaration of the RemoteManager class and Remote struct.
 *
 * @details This file defines the `Remote` structure to hold information about a single
 * remote repository and the `RemoteManager` class, which is responsible for
 * adding, removing, loading, and saving remote configurations from the
 * repository's config file.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

/**
 * @struct Remote
 * @brief Represents a single remote repository configuration.
 *
 * @details Stores the name, URL, and fetch specification (refspec) for a
 * configured remote. The fetch spec is automatically generated upon construction.
 */
struct Remote {
    std::string name;       ///< The shortname of the remote (e.g., "origin").
    std::string url;        ///< The URL (path or network address) of the remote.
    std::string fetch_spec; ///< The refspec defining what to fetch (e.g., "+refs/heads/*:refs/remotes/origin/*").

    /**
     * @brief Constructs a Remote struct.
     *
     * @param name The shortname for the remote.
     * @param url The URL for the remote.
     */
    Remote(const std::string& name, const std::string& url) :
        name(name), url(url), fetch_spec("+refs/heads/*:refs/remotes/" + name + "/*") {}
};

/**
 * @class RemoteManager
 * @brief Manages the repository's remote configurations.
 *
 * @details This class provides an interface to manage remote repositories.
 * It handles adding, removing, and listing remotes, as well as persisting
 * these configurations to and from the repository's `.svcs/config` file.
 */
class RemoteManager {
private:
    std::unordered_map<std::string, Remote> remotes_; ///< In-memory cache of remotes, keyed by name.
    fs::path config_path_; ///< Path to the repository's configuration file.

public:
    /**
     * @brief Constructs a RemoteManager and loads existing remotes.
     *
     * @param repo_path The root path of the repository (e.g., "/path/to/my-project"),
     * *not* the .svcs directory.
     *
     * @details The constructor sets the path to the config file (repo_path / ".svcs" / "config")
     * and automatically calls `load()` to populate the remotes list.
     */
    explicit RemoteManager(const fs::path& repo_path);

    /**
     * @brief Adds a new remote configuration.
     *
     * @param name The shortname for the new remote (e.g., "origin").
     * @param url The URL for the new remote.
     * @return bool True if the remote was added successfully, false if a remote
     * with that name already exists.
     */
    bool addRemote(const std::string& name, const std::string& url);

    /**
     * @brief Removes a remote configuration by name.
     *
     * @param name The shortname of the remote to remove.
     * @return bool True if the remote was found and removed, false otherwise.
     */
    bool removeRemote(const std::string& name);

    /**
     * @brief Checks if a remote with the given name exists.
     *
     * @param name The name of the remote to check.
     * @return bool True if the remote exists, false otherwise.
     */
    bool hasRemote(const std::string& name) const;

    /**
     * @brief Gets the URL of a configured remote.
     *
     * @param name The name of the remote.
     * @return std::string The URL if the remote is found, otherwise an empty string.
     */
    std::string getRemoteUrl(const std::string& name) const;

    /**
     * @brief Lists the names of all configured remotes.
     *
     * @return std::vector<std::string> A vector containing the names of all remotes.
     */
    std::vector<std::string> listRemotes() const;

    /**
     * @brief Saves the current remote configurations back to the config file.
     *
     * @details This method overwrites the remote sections of the config file
     * with the current in-memory state.
     */
    void save() const;

    /**
     * @brief Loads (or reloads) the remote configurations from the config file.
     *
     * @details This method clears the current in-memory cache and parses the
     * config file to populate it.
     */
    void load();

    /**
     * @brief Gets the number of configured remotes.
     *
     * @return size_t The count of remotes.
     */
    size_t count() const { return remotes_.size(); }
};