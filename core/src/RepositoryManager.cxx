/**
 * @file RepositoryManager.cxx
 * @brief Implementation of the RepositoryManager class, handling high-level repository creation, initialization, and cleanup utilities.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <algorithm>
#include <filesystem>
#include <optional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <set>

RepositoryManager::RepositoryManager(std::shared_ptr<ISubject> bus) 
    : eventBus(bus) {
}

void RepositoryManager::logError(const std::string& message) {
    if (eventBus) {
        eventBus->notify({Event::ERROR_MESSAGE, message, "RepositoryManager"});
    }
}

void RepositoryManager::logDebug(const std::string& message) {
    if (eventBus) {
        eventBus->notify({Event::DEBUG_MESSAGE, message, "RepositoryManager"});
    }
}

void RepositoryManager::logInfo(const std::string& message) {
    if (eventBus) {
        eventBus->notify({Event::GENERAL_INFO, message, "RepositoryManager"});
    }
}

bool RepositoryManager::removeRepository(const std::filesystem::path& path) {
    try {
        if (!std::filesystem::exists(path)) {
            logDebug("Repository doesn't exist, nothing to remove: " + path.string());
            return true;
        }
        
        // Проверяем, что это действительно директория .svcs
        if (path.filename() != ".svcs") {
            logError("Refusing to remove non-repository directory: " + path.string());
            return false;
        }
        
        // Подсчитываем количество файлов и директорий для удаления
        size_t fileCount = 0;
        size_t dirCount = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                fileCount++;
            } else if (entry.is_directory()) {
                dirCount++;
            }
        }
        
        logDebug("Removing repository: " + path.string() + 
                 " (" + std::to_string(fileCount) + " files, " + 
                 std::to_string(dirCount) + " directories)");
        
        // Удаляем репозиторий
        std::filesystem::remove_all(path);
        
        // Проверяем, что удаление прошло успешно
        if (!std::filesystem::exists(path)) {
            logInfo("Successfully removed repository: " + path.string());
            return true;
        } else {
            logError("Failed to remove repository: " + path.string());
            return false;
        }
    } catch (const std::exception& e) {
        logError("Failed to remove repository '" + path.string() + "': " + e.what());
        return false;
    }
}

bool RepositoryManager::createDirectory(const std::filesystem::path& path) {
    try {
        if (std::filesystem::exists(path)) {
            logDebug("Directory already exists: " + path.string());
            return true;
        }
        
        bool created = std::filesystem::create_directories(path);
        if (created) {
            logDebug("Created directory: " + path.string());
        } else {
            logError("Failed to create directory: " + path.string());
        }
        return created;
    } catch (const std::exception& e) {
        logError("Failed to create directory '" + path.string() + "': " + e.what());
        return false;
    }
}

bool RepositoryManager::createFile(const std::filesystem::path& path, const std::string& content) {
    try {
        // Создаем родительские директории если нужно
        if (!createDirectory(path.parent_path())) {
            return false;
        }
        
        std::ofstream file(path);
        if (!file) {
            logError("Failed to open file for writing: " + path.string());
            return false;
        }
        file << content;
        file.close();
        
        bool exists = std::filesystem::exists(path);
        if (exists) {
            logDebug("Created file: " + path.string());
        } else {
            logError("File creation failed - file doesn't exist: " + path.string());
        }
        return exists;
    } catch (const std::exception& e) {
        logError("Failed to create file '" + path.string() + "': " + e.what());
        return false;
    }
}

void RepositoryManager::updateBranchReference(const std::string& branchName, const std::string& commitHash) {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        std::filesystem::path repoPath = getRepositoryPath();
        std::filesystem::path branchFile = repoPath / ".svcs" / "refs" / "heads" / branchName;
        
        // Создать директории если не существуют
        std::filesystem::create_directories(branchFile.parent_path());
        
        if (commitHash.empty()) {
            // Очистить файл ветки (оставить пустым)
            std::ofstream file(branchFile, std::ios::trunc);
            logDebug("Cleared branch reference: " + branchName);
        } else {
            // Записать хеш коммита в файл ветки
            std::ofstream file(branchFile);
            file << commitHash << "\n";
            logDebug("Updated branch " + branchName + " to commit: " + commitHash.substr(0, 8));
        }
        
    } catch (const std::exception& e) {
        logError("Error updating branch reference: " + std::string(e.what()));
    }
}

void RepositoryManager::updateHead(const std::string& commitHash) {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        std::filesystem::path repoPath = getRepositoryPath();
        std::filesystem::path headFile = repoPath / ".svcs" / "HEAD";
        
        std::filesystem::create_directories(headFile.parent_path());
        
        if (commitHash.empty()) {
            std::ofstream file(headFile);
            file << "ref: refs/heads/main\n";
            
            std::filesystem::path refsDir = repoPath / ".svcs" / "refs" / "heads";
            std::filesystem::create_directories(refsDir);
            std::filesystem::path mainRef = refsDir / "main";
            
            if (!std::filesystem::exists(mainRef)) {
                std::ofstream refFile(mainRef);
            }
            
            logDebug("Reset HEAD to default branch");
        } else {
            std::ofstream file(headFile);
            file << commitHash << "\n";
            logDebug("Updated HEAD to commit: " + commitHash.substr(0, 8));
        }
        
    } catch (const std::exception& e) {
        logError("Error updating HEAD: " + std::string(e.what()));
    }
}

bool RepositoryManager::initializeRepository(const std::string& path, bool force) {
    std::filesystem::path repoPath;
    
    if (path.empty() || path == ".") {
        repoPath = std::filesystem::current_path();
    } else {
        repoPath = std::filesystem::absolute(path);
    }
    
    std::filesystem::path svcsDir = repoPath / ".svcs";
    
    logInfo("=== START INITIALIZATION ===");
    logInfo("Target path: " + path);
    logInfo("Absolute repo path: " + repoPath.string());
    logInfo("SVCS dir will be: " + svcsDir.string());
    logInfo("Current working dir: " + std::filesystem::current_path().string());
    
    // Проверяем существование .svcs ДО удаления
    if (std::filesystem::exists(svcsDir)) {
        logInfo(".svcs exists before removal: YES");
        logInfo(".svcs contents before removal:");
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(svcsDir)) {
                logInfo("  " + entry.path().string());
            }
        } catch (const std::exception& e) {
            logError("Error listing .svcs: " + std::string(e.what()));
        }
    } else {
        logInfo(".svcs exists before removal: NO");
    }
    
    // Проверяем, не существует ли уже репозиторий
    if (std::filesystem::exists(svcsDir)) {
        if (force) {
            logInfo("Repository already exists, forcing reinitialization");
            if (!removeRepository(svcsDir)) {
                logError("Failed to remove existing repository");
                return false;
            }
        } else {
            logError("Repository already exists at: " + svcsDir.string());
            logInfo("Use --force to reinitialize");
            return false;
        }
    }
    
    logInfo("Creating directory structure...");
    
    if (!createDirectory(svcsDir)) {
        logError("FAILED to create .svcs directory");
        return false;
    }
    logInfo("CREATED: " + svcsDir.string());
    
    if (!createDirectory(svcsDir / "objects")) {
        logError("FAILED to create objects directory");
        return false;
    }
    logInfo("CREATED: " + (svcsDir / "objects").string());
    
    if (!createDirectory(svcsDir / "refs")) {
        logError("FAILED to create refs directory");
        return false;
    }
    logInfo("CREATED: " + (svcsDir / "refs").string());
    
    if (!createDirectory(svcsDir / "refs" / "heads")) {
        logError("FAILED to create refs/heads directory");
        return false;
    }
    logInfo("CREATED: " + (svcsDir / "refs" / "heads").string());
    
    // Создаем файлы с детальным логированием
    logInfo("Creating files...");
    
    if (!createFile(svcsDir / "HEAD", "ref: refs/heads/main\n")) {
        logError("FAILED to create HEAD file");
        return false;
    }
    logInfo("CREATED: HEAD file");
    
    if (!createFile(svcsDir / "index", "")) {
        logError("FAILED to create index file");
        return false;
    }
    logInfo("CREATED: index file");
    
    if (!createFile(svcsDir / "config", 
        "[core]\n    repositoryformatversion = 0\n    filemode = true\n    bare = false\n")) {
        logError("FAILED to create config file");
        return false;
    }
    logInfo("CREATED: config file");
    
    currentRepoPath = repoPath.string();
    
    // ФИНАЛЬНАЯ ПРОВЕРКА - что реально создалось
    logInfo("=== FINAL VERIFICATION ===");
    logInfo("Checking created structure...");
    
    bool allOk = true;
    for (const auto& checkPath : {
        svcsDir,
        svcsDir / "objects", 
        svcsDir / "refs",
        svcsDir / "refs" / "heads",
        svcsDir / "HEAD",
        svcsDir / "index",
        svcsDir / "config"
    }) {
        if (std::filesystem::exists(checkPath)) {
            logInfo("VERIFIED: " + checkPath.string());
        } else {
            logError("MISSING: " + checkPath.string());
            allOk = false;
        }
    }
    
    if (allOk) {
        logInfo("=== INITIALIZATION SUCCESSFUL ===");
        logInfo("Repository initialized at: " + currentRepoPath);
    } else {
        logError("=== INITIALIZATION FAILED ===");
    }
    
    return allOk;
}


bool RepositoryManager::isRepositoryInitialized(const std::string& path) {
    std::filesystem::path checkPath;
    
    if (path.empty() || path == ".") {
        checkPath = std::filesystem::current_path();
    } else {
        checkPath = std::filesystem::absolute(path);
    }
    
    std::filesystem::path svcsDir = checkPath / ".svcs";
    std::filesystem::path headFile = svcsDir / "HEAD";
    
    bool exists = std::filesystem::exists(svcsDir) && 
                  std::filesystem::exists(headFile);
    
    logDebug("Checking repository at: " + checkPath.string() + " - " + (exists ? "exists" : "not found"));
    
    if (exists) {
        currentRepoPath = checkPath.string();
        logDebug("Repository found at: " + currentRepoPath);
    }
    
    return exists;
}

bool RepositoryManager::addFileToStaging(const std::string& filePath) {
    if (!isRepositoryInitialized()) {
        logError("No repository found for add operation");
        return false;
    }
    
    std::filesystem::path svcsDir = std::filesystem::path(currentRepoPath) / ".svcs";
    std::filesystem::path indexFile = svcsDir / "index";
    
    logDebug("Adding file to staging: " + filePath);
    logDebug("Repository path: " + currentRepoPath);
    logDebug("Index file: " + indexFile.string());
    
    try {
        std::ofstream index(indexFile, std::ios::app);
        if (!index) {
            logError("Failed to open index file: " + indexFile.string());
            return false;
        }
        
        index << filePath << "\n";
        logInfo("Successfully added to index: " + filePath);
        return true;
    } catch (const std::exception& e) {
        logError("Exception adding file '" + filePath + "': " + e.what());
        return false;
    }
}

std::vector<std::string> RepositoryManager::getStagedFiles() {
    std::vector<std::string> stagedFiles;
    
    if (!isRepositoryInitialized()) {
        return stagedFiles;
    }
    
    std::filesystem::path svcsDir = std::filesystem::path(currentRepoPath) / ".svcs";
    std::filesystem::path indexFile = svcsDir / "index";
    
    try {
        std::ifstream index(indexFile);
        std::string line;
        while (std::getline(index, line)) {
            if (!line.empty()) {
                stagedFiles.push_back(line);
            }
        }
        logDebug("Read " + std::to_string(stagedFiles.size()) + " staged files from index");
    } catch (const std::exception& e) {
        logError("Error reading index file '" + indexFile.string() + "': " + e.what());
    }
    
    return stagedFiles;
}

bool RepositoryManager::clearStagingArea() {
    if (!isRepositoryInitialized()) {
        return false;
    }

    std::filesystem::path indexFile = std::filesystem::path(currentRepoPath) / ".svcs" / "index";
    
    try {
        // Очищаем index файл
        std::ofstream index(indexFile, std::ios::trunc);
        if (!index) {
            logError("Failed to clear staging area: cannot open index file");
            return false;
        }
        
        logDebug("Cleared staging area");
        return true;
    } catch (const std::exception& e) {
        logError("Error clearing staging area: " + std::string(e.what()));
        return false;
    }
}

std::string RepositoryManager::createCommit(const std::string& message) {
    if (!isRepositoryInitialized()) {
        logError("No repository found for commit operation");
        return "";
    }

    auto stagedFiles = getStagedFiles();
    if (stagedFiles.empty()) {
        logError("No files staged for commit");
        return "";
    }

    try {
        std::string parentCommit = getHeadCommit();
        logDebug("Creating new commit with parent: '" + parentCommit + "'");
        
        // ВАЖНО: Проверить что родительский коммит существует
        if (!parentCommit.empty()) {
            std::filesystem::path objectsDir = std::filesystem::path(getRepositoryPath()) / ".svcs" / "objects";
            std::filesystem::path commitDir = objectsDir / parentCommit.substr(0, 2);
            std::filesystem::path commitFile = commitDir / parentCommit.substr(2);
            
            if (!std::filesystem::exists(commitFile)) {
                logDebug("HEAD points to non-existent commit, resetting: " + parentCommit);
                parentCommit = "none"; // Установить как первый коммит
                // ОБНОВИТЬ ФАЙЛ ВЕТКИ
                updateBranchReference("main", ""); // Очистить ссылку ветки
            }
        } else {
            parentCommit = "none";
            logDebug("First commit - no parent");
        }
        
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()).count();
        
        std::stringstream commitContent;
        commitContent << "parent " << parentCommit << "\n";
        commitContent << "author User <user@example.com>\n";
        commitContent << "timestamp " << timestamp << "\n";
        commitContent << "message " << message << "\n";
        commitContent << "files " << stagedFiles.size() << "\n";
        for (const auto& file : stagedFiles) {
            commitContent << file << "\n";
        }
        
        std::string content = commitContent.str();
        
        std::hash<std::string> hasher;
        size_t hashValue = hasher(content);
        std::stringstream hashStream;
        hashStream << std::hex << hashValue;
        std::string commitHash = hashStream.str();
        
        logDebug("Commit content:\n" + content);
        logDebug("Generated commit hash: " + commitHash);
        
        std::filesystem::path commitDir = std::filesystem::path(currentRepoPath) / ".svcs" / "objects" / commitHash.substr(0, 2);
        std::filesystem::path commitFile = commitDir / commitHash.substr(2);
        
        if (!createFile(commitFile, content)) {
            logError("Failed to store commit object");
            return "";
        }
        
        // ВАЖНО: Обновить и HEAD и файл ветки
        updateHead(commitHash);
        updateBranchReference("main", commitHash);
        
        logInfo("Created commit: " + commitHash.substr(0, 8) + " - " + message);
        return commitHash;
        
    } catch (const std::exception& e) {
        logError("Error creating commit: " + std::string(e.what()));
        return "";
    }
}


std::string RepositoryManager::getParentCommitHash(const std::string& commitHash) const {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        std::filesystem::path repoPath = getRepositoryPath();
        std::filesystem::path objectsDir = repoPath / ".svcs" / "objects";
        std::filesystem::path commitDir = objectsDir / commitHash.substr(0, 2);
        std::filesystem::path commitFile = commitDir / commitHash.substr(2);
        
        if (!std::filesystem::exists(commitFile)) {
            eventBus->notify({Event::DEBUG_MESSAGE, 
                              "Commit file not found: " + commitFile.string(), SOURCE});
            return "";
        }
        
        std::ifstream file(commitFile);
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.find("parent ") == 0) {
                std::string parentHash = line.substr(7); // Remove "parent "
                if (parentHash != "none") {
                    return parentHash;
                }
            }
        }
        
        return ""; // No parent found (initial commit)
        
    } catch (const std::exception& e) {
        eventBus->notify({Event::DEBUG_MESSAGE, 
                          "Error reading parent commit: " + std::string(e.what()), SOURCE});
        return "";
    }
}

void RepositoryManager::updateCommitReferences(const std::string& removedCommitHash, const std::string& newParentHash) {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        // Найти все коммиты, которые ссылаются на удаляемый коммит как на родителя
        auto commits = getCommitHistory();
        
        for (const auto& commit : commits) {
            std::filesystem::path repoPath = getRepositoryPath();
            std::filesystem::path objectsDir = repoPath / ".svcs" / "objects";
            std::filesystem::path commitDir = objectsDir / commit.hash.substr(0, 2);
            std::filesystem::path commitFile = commitDir / commit.hash.substr(2);
            
            if (!std::filesystem::exists(commitFile)) {
                continue;
            }
            
            // Прочитать файл коммита
            std::ifstream inFile(commitFile);
            std::vector<std::string> lines;
            std::string line;
            bool modified = false;
            
            while (std::getline(inFile, line)) {
                // Если строка ссылается на удаляемый коммит как на родителя, заменить ссылку
                if (line.find("parent " + removedCommitHash) == 0) {
                    line = "parent " + newParentHash;
                    modified = true;
                    logDebug("Updated parent reference in commit: " + commit.hash.substr(0, 8));
                }
                lines.push_back(line);
            }
            inFile.close();
            
            // Если были изменения, перезаписать файл коммита
            if (modified) {
                std::ofstream outFile(commitFile);
                for (const auto& l : lines) {
                    outFile << l << "\n";
                }
            }
        }
        
    } catch (const std::exception& e) {
        logError("Error updating commit references: " + std::string(e.what()));
    }
}

bool RepositoryManager::revertCommit(const std::string& commitHash) {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        logDebug("Attempting to remove commit from history: " + commitHash);
        
        auto commits = getCommitHistory();
        
        if (commits.empty()) {
            logError("No commits in history");
            return false;
        }
        
        auto commitIt = std::find_if(commits.begin(), commits.end(),
            [&commitHash](const CommitInfo& commit) {
                return commit.hash.find(commitHash) == 0;
            });
        
        if (commitIt == commits.end()) {
            logError("Commit not found in current history: " + commitHash);
            return false;
        }
        
        CommitInfo commitToRemove = *commitIt;
        
        if (commits.size() == 1) {
            logError("Cannot revert the initial commit");
            return false;
        }
        
        std::filesystem::path repoPath = getRepositoryPath();
        std::filesystem::path objectsDir = repoPath / ".svcs" / "objects";
        std::filesystem::path commitDir = objectsDir / commitToRemove.hash.substr(0, 2);
        std::filesystem::path commitFile = commitDir / commitToRemove.hash.substr(2);
        
        if (!std::filesystem::exists(commitFile)) {
            logError("Commit file already doesn't exist: " + commitFile.string());
            return false;
        }
        
        std::filesystem::remove(commitFile);
        logDebug("Removed commit file: " + commitFile.string());
        
        std::string newHeadHash = "";
        for (auto it = commits.begin(); it != commits.end(); ++it) {
            if (it->hash == commitToRemove.hash) continue; 
            
            std::filesystem::path checkDir = objectsDir / it->hash.substr(0, 2);
            std::filesystem::path checkFile = checkDir / it->hash.substr(2);
            
            if (std::filesystem::exists(checkFile)) {
                newHeadHash = it->hash;
                break;
            }
        }
        
        if (!newHeadHash.empty()) {
            updateHead(newHeadHash);
            logInfo("Updated HEAD to: " + newHeadHash.substr(0, 8));
        } else {
            updateHead("");
            logInfo("Reset HEAD - no valid commits found");
        }

        clearStagingArea();
        
        logInfo("Successfully removed commit from history: " + commitToRemove.hash.substr(0, 8));
        return true;
        
    } catch (const std::exception& e) {
        logError("Error removing commit: " + std::string(e.what()));
        return false;
    }
}

std::optional<CommitInfo> RepositoryManager::getCommitByHash(const std::string& commitHash) {
    auto commits = getCommitHistory();
    
    for (const auto& commit : commits) {
        if (commit.hash.find(commitHash) == 0) {
            return commit;
        }
    }
    
    return std::nullopt;
}

bool RepositoryManager::restoreFilesFromCommit(const CommitInfo& commit) {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        std::filesystem::path repoPath = getRepositoryPath();
        std::filesystem::path objectsDir = repoPath / ".svcs" / "objects";

        std::filesystem::path commitDir = objectsDir / commit.hash.substr(0, 2);
        std::filesystem::path commitFile = commitDir / commit.hash.substr(2);
        
        if (!std::filesystem::exists(commitFile)) {
            eventBus->notify({Event::ERROR_MESSAGE, 
                              "Commit file not found: " + commitFile.string(), SOURCE});
            return false;
        }
        
        std::ifstream file(commitFile);
        std::string line;
        bool inFilesSection = false;
        
        while (std::getline(file, line)) {
            if (line == "files " + std::to_string(commit.files_count)) {
                inFilesSection = true;
                continue;
            }
            
            if (inFilesSection) {
                if (line.empty() || line.find("file") == 0) {
                    break;
                }
                
                std::string filename = line;
                
                std::filesystem::path filePath = repoPath / filename;
                
                if (std::filesystem::exists(filePath)) {
                    std::filesystem::remove(filePath);
                }
            }
        }
        
        return true;
        
    } catch (const std::filesystem::filesystem_error& e) {
        eventBus->notify({Event::ERROR_MESSAGE, 
                          "Filesystem error restoring files: " + std::string(e.what()), SOURCE});
        return false;
    }
}


bool RepositoryManager::saveStagedChanges(const std::string& message) {
    if (!isRepositoryInitialized()) {
        logError("No repository found for save operation");
        return false;
    }

    auto stagedFiles = getStagedFiles();
    if (stagedFiles.empty()) {
        logError("No files staged for commit");
        return false;
    }

    logInfo("Creating commit with " + std::to_string(stagedFiles.size()) + " staged files");

    std::string commitHash = createCommit(message);
    if (commitHash.empty()) {
        logError("Failed to create commit");
        return false;
    }

    if (!clearStagingArea()) {
        logError("Commit created but failed to clear staging area");
    }

    logInfo("Successfully saved changes with commit: " + commitHash);
    return true;
}

std::string RepositoryManager::getHeadCommit() {
    const std::string SOURCE = "RepositoryManager";
    
    try {
        std::filesystem::path repoPath = getRepositoryPath();
        std::filesystem::path headFile = repoPath / ".svcs" / "HEAD";
        
        if (!std::filesystem::exists(headFile)) {
            logDebug("HEAD file not found");
            return "";
        }
        
        std::ifstream file(headFile);
        std::string line;
        
        if (std::getline(file, line)) {
            if (line.find("ref: ") == 0) {
                std::string refPath = line.substr(5);
                std::filesystem::path refFile = repoPath / ".svcs" / refPath;
                
                if (std::filesystem::exists(refFile)) {
                    std::ifstream refStream(refFile);
                    std::string commitHash;
                    if (std::getline(refStream, commitHash)) {
                        return commitHash;
                    }
                } else {
                    logDebug("Reference file not found: " + refFile.string());
                    return "";
                }
            } else {
                return line;
            }
        }
        
        return "";
        
    } catch (const std::exception& e) {
        logError("Error reading HEAD: " + std::string(e.what()));
        return "";
    }
}

std::vector<CommitInfo> RepositoryManager::getCommitHistory() {
    const std::string SOURCE = "RepositoryManager";
    
    std::vector<CommitInfo> commits;
    
    try {
        std::filesystem::path repoPath = getRepositoryPath();
        std::string currentHash = getHeadCommit();
        std::set<std::string> visited;
        
        while (!currentHash.empty() && visited.find(currentHash) == visited.end()) {
            visited.insert(currentHash);
            
            std::filesystem::path objectsDir = repoPath / ".svcs" / "objects";
            std::filesystem::path commitDir = objectsDir / currentHash.substr(0, 2);
            std::filesystem::path commitFile = commitDir / currentHash.substr(2);
            
            if (!std::filesystem::exists(commitFile)) {
                logDebug("Skipping non-existent commit in history: " + currentHash);
                break;
            }
            
            CommitInfo commit;
            commit.hash = currentHash;
            
            std::ifstream file(commitFile);
            std::string line;
            std::string parentHash = "";
            
            while (std::getline(file, line)) {
                if (line.find("parent ") == 0) {
                    parentHash = line.substr(7);
                } else if (line.find("message ") == 0) {
                    commit.message = line.substr(8);
                } else if (line.find("files ") == 0) {
                    commit.files_count = std::stoi(line.substr(6));
                } else if (line.find("author ") == 0) {
                    commit.author = line.substr(7);
                } else if (line.find("timestamp ") == 0) {
                    commit.timestamp = line.substr(10);
                }
            }
            
            commits.push_back(commit);
            
            if (parentHash != "none" && !parentHash.empty()) {
                std::filesystem::path parentDir = objectsDir / parentHash.substr(0, 2);
                std::filesystem::path parentFile = parentDir / parentHash.substr(2);
                
                if (std::filesystem::exists(parentFile)) {
                    currentHash = parentHash;
                } else {
                    logDebug("Parent commit not found, stopping: " + parentHash);
                    break;
                }
            } else {
                currentHash = "";
            }
        }
        
        logDebug("Retrieved " + std::to_string(commits.size()) + " valid commits from history");
        return commits;
        
    } catch (const std::exception& e) {
        logError("Error reading commit history: " + std::string(e.what()));
        return commits;
    }
}