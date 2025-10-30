/**
* @file RemoteManager.cxx
 * @brief Implementation of the RemoteManager class.
 *
 * @details This file implements the methods for the RemoteManager class.
 * It handles the loading, saving, and in-memory management of remote
 * repository configurations.
 *
 * Persistence is handled by reading from and writing to a dedicated 'remotes'
 * file (e.g., /path/to/repo/remotes) in a Git-config-like format.
 *
 * @note This implementation assumes the configuration is in a separate 'remotes'
 * file at the root of the repository, *not* inside `.svcs/config`.
 *
 * @see RemoteManager.hxx
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/RemoteManager.hxx"
#include <fstream>
#include <sstream>
#include <iostream>

RemoteManager::RemoteManager(const fs::path& repo_path)
    : config_path_(repo_path / "remotes") {
    load();
}

bool RemoteManager::addRemote(const std::string& name, const std::string& url) {
    if (name.empty() || url.empty()) {
        return false;
    }

    if (remotes_.find(name) != remotes_.end()) {
        return false;
    }

    remotes_.emplace(name, Remote(name, url));
    save();
    return true;
}

bool RemoteManager::removeRemote(const std::string& name) {
    auto it = remotes_.find(name);
    if (it == remotes_.end()) {
        return false;
    }

    remotes_.erase(it);
    save();
    return true;
}

bool RemoteManager::hasRemote(const std::string& name) const {
    return remotes_.find(name) != remotes_.end();
}

std::string RemoteManager::getRemoteUrl(const std::string& name) const {
    auto it = remotes_.find(name);
    if (it == remotes_.end()) {
        return "";
    }
    return it->second.url;
}

std::vector<std::string> RemoteManager::listRemotes() const {
    std::vector<std::string> result;
    for (const auto& [name, remote] : remotes_) {
        result.push_back(name);
    }
    return result;
}

void RemoteManager::save() const {
    std::ofstream file(config_path_);
    if (!file.is_open()) {
        return;
    }

    for (const auto& [name, remote] : remotes_) {
        file << "[remote \"" << name << "\"]\n";
        file << "    url = " << remote.url << "\n";
        file << "    fetch = " << remote.fetch_spec << "\n";
        file << "\n";
    }
}

void RemoteManager::load() {
    remotes_.clear();

    if (!fs::exists(config_path_)) {
        return;
    }

    std::ifstream file(config_path_);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    std::string current_remote;
    std::string current_url;

    while (std::getline(file, line)) {
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;

        size_t end = line.find_last_not_of(" \t");
        std::string trimmed = line.substr(start, end - start + 1);

        if (trimmed.empty()) continue;

        if (trimmed[0] == '[' && trimmed.back() == ']') {
            std::string section = trimmed.substr(1, trimmed.length() - 2);
            if (section.compare(0, 7, "remote ") == 0) {
                current_remote = section.substr(8, section.length() - 9);
            }
            continue;
        }

        size_t equals_pos = trimmed.find('=');
        if (equals_pos != std::string::npos && !current_remote.empty()) {
            std::string key = trimmed.substr(0, equals_pos);
            std::string value = trimmed.substr(equals_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == "url") {
                current_url = value;
            } else if (key == "fetch") {
                // For now we just store the URL, fetch spec is generated
            }
        }

        if (!current_remote.empty() && !current_url.empty()) {
            remotes_.emplace(current_remote, Remote(current_remote, current_url));
            current_remote.clear();
            current_url.clear();
        }
    }
}