/**
 * @file BranchManager.cxx
 * @brief Implementation of the BranchManager class.
 *
 * @details This file implements the core branch management logic, handling 
 * operations like creation, deletion, renaming, and switching branches, as well as 
 * persistent storage and retrieval of branch metadata within the `.svcs` directory structure.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/BranchManager.hxx"

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

BranchManager::BranchManager(std::shared_ptr<ISubject> event_bus)
    : event_bus(std::move(event_bus))
    , current_branch("main") {
    loadBranches();
    loadCurrentBranch();
}

bool BranchManager::createBranch(const std::string& branch_name) {
    std::string current_head = getHeadCommit();
    
    if (current_head.empty()) {
        return false; 
    }
    
    return createBranchFromCommit(branch_name, current_head);
}

bool BranchManager::deleteBranch(const std::string& name, bool force) {
    if (!branchExists(name)) {
        return false;
    }
    
    if (getCurrentBranch() == name) {
        return false;
    }
    
    branches.erase(name);
    saveBranches();
    
    if (getCurrentBranch() == name) {
        current_branch = "main";
        saveCurrentBranch();
    }
    
    if (event_bus) {
        event_bus->notify({Event::GENERAL_INFO, "Deleted branch: " + name, "branch-manager"});
    }
    
    return true;
}

bool BranchManager::renameBranch(const std::string& old_name, const std::string& new_name) {
    if (!branchExists(old_name) || !isValidBranchName(new_name) || branchExists(new_name)) {
        return false;
    }
    
    auto it = branches.find(old_name);
    if (it != branches.end()) {
        Branch new_branch(new_name, it->second.head_commit, it->second.is_current);
        branches.erase(it);
        branches.emplace(new_name, new_branch);
        
        if (getCurrentBranch() == old_name) {
            current_branch = new_name;
            saveCurrentBranch();
        }
        
        saveBranches();
        
        if (event_bus) {
            event_bus->notify({Event::Type::GENERAL_INFO, "Renamed branch " + old_name + " to " + new_name});
        }
        
        return true;
    }
    
    return false;
}

bool BranchManager::updateBranchHead(const std::string& branch_name, const std::string& commit_hash) {
    auto it = branches.find(branch_name);
    if (it != branches.end()) {
        it->second.head_commit = commit_hash;
        saveBranches();
        return true;
    }
    return false;
}

bool BranchManager::switchBranch(const std::string& name) {
    if (!branchExists(name)) {
        return false;
    }
    
    current_branch = name;
    saveCurrentBranch();
    
    if (event_bus) {
        event_bus->notify({Event::Type::GENERAL_INFO, "Switched to branch: " + name});
    }
    
    return true;
}

std::vector<BranchManager::Branch> BranchManager::getAllBranches() const {
    std::vector<Branch> result;
    result.reserve(branches.size());
    
    for (const auto& [name, branch] : branches) {
        Branch updated_branch = branch;
        updated_branch.is_current = (name == current_branch);
        result.push_back(updated_branch);
    }
    
    std::sort(result.begin(), result.end(), 
              [](const Branch& a, const Branch& b) { return a.name < b.name; });
    
    return result;
}

std::string BranchManager::getHeadCommit() {
    try {
        std::string head_file_path = getHeadFilePath();
        
        // ДИАГНОСТИКА: проверяем существование HEAD файла
        if (!fileExists(head_file_path)) {
            if (event_bus) {
                event_bus->notify({Event::DEBUG_MESSAGE, 
                                  "HEAD file not found: " + head_file_path, "branch-manager"});
            }
            return "";
        }
        
        std::string head_ref = readFile(head_file_path);
        
        // УБЕРИТЕ ЛИШНИЕ СИМВОЛЫ ПЕРЕНОСА СТРОКИ
        head_ref.erase(std::remove(head_ref.begin(), head_ref.end(), '\n'), head_ref.end());
        head_ref.erase(std::remove(head_ref.begin(), head_ref.end(), '\r'), head_ref.end());
        
        if (event_bus) {
            event_bus->notify({Event::DEBUG_MESSAGE, 
                              "HEAD reference content: '" + head_ref + "'", "branch-manager"});
        }
        
        if (head_ref.find("ref: refs/heads/") == 0) {
            std::string branch_name = head_ref.substr(16); // "ref: refs/heads/"
            
            if (event_bus) {
                event_bus->notify({Event::DEBUG_MESSAGE, 
                                  "Resolving branch: " + branch_name, "branch-manager"});
            }
            
            // Получаем HEAD коммит через getBranchHead
            std::string branch_head = getBranchHead(branch_name);
            
            if (event_bus) {
                event_bus->notify({Event::DEBUG_MESSAGE, 
                                  "Branch '" + branch_name + "' head: '" + branch_head + "'", "branch-manager"});
            }
            
            return branch_head;
        }
        // Если HEAD указывает напрямую на коммит (detached HEAD)
        else if (!head_ref.empty()) {
            if (event_bus) {
                event_bus->notify({Event::DEBUG_MESSAGE, 
                                  "HEAD points directly to commit: " + head_ref, "branch-manager"});
            }
            return head_ref;
        }
        
        if (event_bus) {
            event_bus->notify({Event::DEBUG_MESSAGE, 
                              "HEAD reference is empty or invalid", "branch-manager"});
        }
        return "";
        
    } catch (const std::exception& e) {
        if (event_bus) {
            event_bus->notify({Event::DEBUG_MESSAGE, 
                              "Error in getHeadCommit: " + std::string(e.what()), "branch-manager"});
        }
        return "";
    }
}

std::string BranchManager::getCurrentBranch() const {
    return current_branch;
}

bool BranchManager::branchExists(const std::string& name) const {
    return branches.find(name) != branches.end();
}

std::string BranchManager::getBranchHead(const std::string& branch_name) const {
    auto it = branches.find(branch_name);
    return it != branches.end() ? it->second.head_commit : "";
}

bool BranchManager::isValidBranchName(const std::string& name) {
    if (name.empty()) return false;
    
    std::regex invalid_chars(R"([~^:?*\[\]\\])");
    if (std::regex_search(name, invalid_chars)) {
        return false;
    }
    
    if (name.back() == '/') {
        return false;
    }
    
    if (name.find("//") != std::string::npos) {
        return false;
    }
    
    if (name == "." || name == "..") {
        return false;
    }
    
    return true;
}

bool BranchManager::createBranchFromCommit(const std::string& name, const std::string& commit_hash) {
    if (!isValidBranchName(name)) {
        return false;
    }
    
    if (branchExists(name)) {
        return false;
    }
    
    if (!commitExists(commit_hash)) {
        if (event_bus) {
            event_bus->notify({Event::ERROR_MESSAGE, "Commit not found: " + commit_hash, "branch-manager"});
        }
        return false;
    }
    
    branches.emplace(name, Branch(name, commit_hash));
    saveBranches();
    
    if (event_bus) {
        std::string short_hash = commit_hash.length() >= 8 ? commit_hash.substr(0, 8) : commit_hash;
        event_bus->notify({Event::GENERAL_INFO, "Created branch '" + name + "' from commit " + short_hash, "branch-manager"});
    }
    
    return true;
}

bool BranchManager::commitExists(const std::string& commit_hash) const {
    if (commit_hash.empty()) {
        return false;
    }
    
    std::vector<std::string> exact_paths = {
        ".svcs/objects/" + commit_hash,
        ".svcs/commits/" + commit_hash,
        ".svcs/refs/commits/" + commit_hash,
        ".svcs/logs/" + commit_hash,
        ".svcs/objects/" + commit_hash.substr(0, 2) + "/" + commit_hash.substr(2)
    };
    
    for (const auto& path : exact_paths) {
        if (fileExists(path)) {
            return true;
        }
    }
    
    if (commit_hash.length() >= 4) {
        std::string partial_hash = commit_hash;
        
        std::vector<std::string> search_dirs = {
            ".svcs/objects",
            ".svcs/commits", 
            ".svcs/refs/commits",
            ".svcs/logs"
        };
        
        for (const auto& dir : search_dirs) {
            if (!fileExists(dir)) continue;
            
            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
                    if (!entry.is_regular_file()) continue;
                    
                    std::string filename = entry.path().filename().string();
                    std::string stem = entry.path().stem().string();
                    
                    if (filename.find(partial_hash) == 0 || stem.find(partial_hash) == 0) {
                        return true;
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                if (event_bus) {
                    event_bus->notify({Event::DEBUG_MESSAGE, "Directory error while searching commits: " + std::string(e.what()), "branch-manager"});
                }
                continue;
            }
        }
    }
    
    std::string history_file = ".svcs/history";
    if (fileExists(history_file)) {
        try {
            std::string content = readFile(history_file);
            if (content.find(commit_hash) != std::string::npos) {
                return true;
            }
        } catch (const std::exception&) {
        }
    }
    
    return false;
}

void BranchManager::loadBranches() {
    std::string branches_file = getBranchesFilePath();
    if (!fileExists(branches_file)) {
        branches.emplace("main", Branch("main", ""));
        saveBranches();
        return;
    }
    
    try {
        std::string content = readFile(branches_file);
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string name = line.substr(0, pos);
                std::string commit_hash = line.substr(pos + 1);
                branches.emplace(name, Branch(name, commit_hash));
            }
        }
    } catch (const std::exception& e) {
        if (event_bus) {
            event_bus->notify({Event::Type::GENERAL_INFO, "Failed to load branches: " + std::string(e.what())});
        }
        branches.emplace("main", Branch("main", ""));
    }
}

void BranchManager::saveBranches() {
    try {
        createDirectory(getBranchesDirectory());
        
        std::ofstream file(getBranchesFilePath());
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open branches file for writing");
        }
        
        for (const auto& [name, branch] : branches) {
            file << name << ":" << branch.head_commit << "\n";
        }
        
        file.close();
    } catch (const std::exception& e) {
        if (event_bus) {
            event_bus->notify({Event::Type::ERROR_MESSAGE, "Failed to save branches: " + std::string(e.what())});
        }
    }
}

void BranchManager::loadCurrentBranch() {
    std::string head_file = getHeadFilePath();
    if (fileExists(head_file)) {
        try {
            std::string content = readFile(head_file);
            if (content.find("ref: refs/heads/") == 0) {
                current_branch = content.substr(16);
                current_branch.erase(std::remove(current_branch.begin(), current_branch.end(), '\n'), current_branch.end());
            } else {
                current_branch = content;
            }
        } catch (const std::exception& e) {
            if (event_bus) {
                event_bus->notify({Event::Type::ERROR_MESSAGE, "Failed to load current branch: " + std::string(e.what())});
            }
        }
    }
}

void BranchManager::saveCurrentBranch() {
    try {
        createDirectory(getBranchesDirectory());
        
        std::ofstream file(getHeadFilePath());
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open HEAD file for writing");
        }
        
        file << "ref: refs/heads/" << current_branch;
        file.close();
    } catch (const std::exception& e) {
        if (event_bus) {
            event_bus->notify({Event::Type::ERROR_MESSAGE, "Failed to save current branch: " + std::string(e.what())});
        }
    }
}

std::string BranchManager::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void BranchManager::writeFile(const std::string& path, const std::string& content) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + path);
    }
    
    file << content;
    file.close();
}

bool BranchManager::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

void BranchManager::createDirectory(const std::string& path) const {
    if (!fileExists(path)) {
        std::filesystem::create_directories(path);
    }
}

std::string BranchManager::getBranchesFilePath() const {
    return ".svcs/refs/heads/branches";
}

std::string BranchManager::getHeadFilePath() const {
    return ".svcs/HEAD";
}

std::string BranchManager::getBranchesDirectory() const {
    return ".svcs/refs/heads";
}