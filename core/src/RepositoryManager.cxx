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
        // Получаем родительский коммит ДО создания нового
        std::string parentCommit = getHeadCommit();
        logDebug("Creating new commit with parent: '" + parentCommit + "'");
        
        // Если это первый коммит, parent будет пустым
        if (parentCommit.empty()) {
            parentCommit = "none";
            logDebug("First commit - no parent");
        }
        
        // Получаем текущее время
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()).count();
        
        // Создаем содержимое коммита в правильном формате
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
        
        // Генерируем хеш
        std::hash<std::string> hasher;
        size_t hashValue = hasher(content);
        std::stringstream hashStream;
        hashStream << std::hex << hashValue;
        std::string commitHash = hashStream.str();
        
        logDebug("Commit content:\n" + content);
        logDebug("Generated commit hash: " + commitHash);
        
        // Сохраняем коммит
        std::filesystem::path commitDir = std::filesystem::path(currentRepoPath) / ".svcs" / "objects" / commitHash.substr(0, 2);
        std::filesystem::path commitFile = commitDir / commitHash.substr(2);
        
        if (!createFile(commitFile, content)) {
            logError("Failed to store commit object");
            return "";
        }
        
        // Обновляем refs/heads/main
        std::filesystem::path mainBranch = std::filesystem::path(currentRepoPath) / ".svcs" / "refs" / "heads" / "main";
        if (!createFile(mainBranch, commitHash)) {
            logError("Failed to update branch reference");
            return "";
        }
        
        logInfo("Created commit: " + commitHash.substr(0, 8) + " - " + message);
        return commitHash;
        
    } catch (const std::exception& e) {
        logError("Error creating commit: " + std::string(e.what()));
        return "";
    }
}

bool RepositoryManager::saveStagedChanges(const std::string& message) {
    if (!isRepositoryInitialized()) {
        logError("No repository found for save operation");
        return false;
    }

    // Проверяем есть ли staged файлы
    auto stagedFiles = getStagedFiles();
    if (stagedFiles.empty()) {
        logError("No files staged for commit");
        return false;
    }

    logInfo("Creating commit with " + std::to_string(stagedFiles.size()) + " staged files");

    // Создаем коммит
    std::string commitHash = createCommit(message);
    if (commitHash.empty()) {
        logError("Failed to create commit");
        return false;
    }

    // Очищаем staging area
    if (!clearStagingArea()) {
        logError("Commit created but failed to clear staging area");
        // Все равно возвращаем true, так как коммит создан успешно
    }

    logInfo("Successfully saved changes with commit: " + commitHash);
    return true;
}

std::string RepositoryManager::getHeadCommit() {
    if (!isRepositoryInitialized()) {
        return "";
    }

    std::filesystem::path headFile = std::filesystem::path(currentRepoPath) / ".svcs" / "HEAD";
    
    try {
        if (!std::filesystem::exists(headFile)) {
            logDebug("HEAD file does not exist");
            return "";
        }
        
        std::ifstream file(headFile);
        std::string line;
        if (std::getline(file, line)) {
            logDebug("HEAD file content: '" + line + "'");
            
            // Если это reference (начинается с "ref: ")
            if (line.find("ref: ") == 0) {
                std::string refPath = line.substr(5);
                std::filesystem::path refFile = std::filesystem::path(currentRepoPath) / ".svcs" / refPath;
                logDebug("Following reference to: " + refFile.string());
                
                if (std::filesystem::exists(refFile)) {
                    std::ifstream ref(refFile);
                    std::string commitHash;
                    if (std::getline(ref, commitHash)) {
                        logDebug("Found commit hash in reference: " + commitHash);
                        return commitHash;
                    }
                } else {
                    logError("Reference file not found: " + refFile.string());
                }
                return "";
            }
            // Если это прямой хеш коммита
            logDebug("HEAD contains direct commit: " + line);
            return line;
        }
    } catch (const std::exception& e) {
        logError("Error reading HEAD: " + std::string(e.what()));
    }
    
    return "";
}

std::vector<CommitInfo> RepositoryManager::getCommitHistory() {
    std::vector<CommitInfo> commits;
    
    if (!isRepositoryInitialized()) {
        logError("No repository initialized for history");
        return commits;
    }
    
    // Начинаем с текущего HEAD
    std::string currentCommitHash = getHeadCommit();
    logDebug("=== START COMMIT HISTORY ===");
    logDebug("Starting from HEAD: " + currentCommitHash);
    
    if (currentCommitHash.empty()) {
        logDebug("No HEAD commit found");
        return commits;
    }
    
    int maxIterations = 10; // Защита от бесконечного цикла
    int iteration = 0;
    
    while (!currentCommitHash.empty() && currentCommitHash != "none" && iteration < maxIterations) {
        iteration++;
        
        CommitInfo commitInfo;
        commitInfo.hash = currentCommitHash;
        
        // Читаем информацию о коммите
        std::filesystem::path commitFile = std::filesystem::path(currentRepoPath) / ".svcs" / "objects" / 
                                          currentCommitHash.substr(0, 2) / currentCommitHash.substr(2);
        
        logDebug("Reading commit: " + currentCommitHash);
        logDebug("Commit file: " + commitFile.string());
        
        if (std::filesystem::exists(commitFile)) {
            std::ifstream file(commitFile);
            std::string line;
            std::string nextParent = "";
            bool foundParent = false;
            
            while (std::getline(file, line)) {
                logDebug("Commit line: " + line);
                
                if (line.find("parent ") == 0) {
                    nextParent = line.substr(7);
                    foundParent = true;
                    logDebug("Found parent: " + nextParent);
                } else if (line.find("message ") == 0) {
                    commitInfo.message = line.substr(8);
                    logDebug("Found message: " + commitInfo.message);
                } else if (line.find("timestamp ") == 0) {
                    commitInfo.timestamp = line.substr(10);
                } else if (line.find("files ") == 0) {
                    try {
                        commitInfo.files_count = std::stoi(line.substr(6));
                    } catch (...) {
                        commitInfo.files_count = 0;
                    }
                }
            }
            
            commits.push_back(commitInfo);
            logDebug("Added commit to history: " + commitInfo.hash.substr(0, 8) + " - " + commitInfo.message);
            
            // Переходим к родительскому коммиту
            if (foundParent && !nextParent.empty() && nextParent != "none") {
                currentCommitHash = nextParent;
                logDebug("Moving to parent: " + currentCommitHash);
            } else {
                logDebug("No parent found, stopping history");
                currentCommitHash = "";
            }
            
        } else {
            logError("Commit file not found: " + commitFile.string());
            break;
        }
    }
    
    logDebug("=== END COMMIT HISTORY ===");
    logDebug("Total commits found: " + std::to_string(commits.size()));
    
    return commits;
}