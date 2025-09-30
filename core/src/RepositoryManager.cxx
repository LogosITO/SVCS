/**
 * @file RepositoryManager.cxx
 * @brief Implementation of the RepositoryManager class, handling high-level repository creation, initialization, and cleanup utilities.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/RepositoryManager.hxx"
#include "../../services/ISubject.hxx"

#include <fstream>

RepositoryManager::RepositoryManager(std::shared_ptr<ISubject> bus) 
    : eventBus_(bus) {
}

void RepositoryManager::logError(const std::string& message) {
    if (eventBus_) {
        eventBus_->notify({Event::ERROR_MESSAGE, message, "RepositoryManager"});
    }
}

void RepositoryManager::logDebug(const std::string& message) {
    if (eventBus_) {
        eventBus_->notify({Event::DEBUG_MESSAGE, message, "RepositoryManager"});
    }
}

void RepositoryManager::logInfo(const std::string& message) {
    if (eventBus_) {
        eventBus_->notify({Event::GENERAL_INFO, message, "RepositoryManager"});
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

bool RepositoryManager::initializeRepository(const std::string& path, bool force) {
    std::filesystem::path repoPath;
    
    // Если путь не указан или указан как ".", используем текущую рабочую директорию
    if (path.empty() || path == ".") {
        repoPath = std::filesystem::current_path();
    } else {
        repoPath = std::filesystem::absolute(path);
    }
    
    std::filesystem::path svcsDir = repoPath / ".svcs";
    
    logInfo("Initializing repository in: " + repoPath.string());
    
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
    
    // Создаем структуру директорий
    if (!createDirectory(svcsDir)) {
        logError("Failed to create .svcs directory");
        return false;
    }
    if (!createDirectory(svcsDir / "objects")) {
        logError("Failed to create objects directory");
        return false;
    }
    if (!createDirectory(svcsDir / "refs")) {
        logError("Failed to create refs directory");
        return false;
    }
    if (!createDirectory(svcsDir / "refs" / "heads")) {
        logError("Failed to create refs/heads directory");
        return false;
    }
    
    // Создаем необходимые файлы
    if (!createFile(svcsDir / "HEAD", "ref: refs/heads/main\n")) {
        logError("Failed to create HEAD file");
        return false;
    }
    if (!createFile(svcsDir / "index", "")) {
        logError("Failed to create index file");
        return false;
    }
    if (!createFile(svcsDir / "config", 
        "[core]\n    repositoryformatversion = 0\n    filemode = true\n    bare = false\n")) {
        logError("Failed to create config file");
        return false;
    }
    
    currentRepoPath_ = repoPath.string();
    logInfo("Repository initialized successfully: " + currentRepoPath_);
    
    return true;
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
        currentRepoPath_ = checkPath.string();
        logDebug("Repository found at: " + currentRepoPath_);
    }
    
    return exists;
}

bool RepositoryManager::addFileToStaging(const std::string& filePath) {
    if (!isRepositoryInitialized()) {
        logError("No repository found for add operation");
        return false;
    }
    
    std::filesystem::path svcsDir = std::filesystem::path(currentRepoPath_) / ".svcs";
    std::filesystem::path indexFile = svcsDir / "index";
    
    logDebug("Adding file to staging: " + filePath);
    logDebug("Repository path: " + currentRepoPath_);
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
    
    std::filesystem::path svcsDir = std::filesystem::path(currentRepoPath_) / ".svcs";
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