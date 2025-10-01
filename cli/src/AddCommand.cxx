/**
 * @file AddCommand.cxx
 * @brief Implementation of the AddCommand class, responsible for processing the 'svcs add' command and staging files for the next commit.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/AddCommand.hxx"

AddCommand::AddCommand(std::shared_ptr<ISubject> subject, 
                       std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(subject), repoManager_(repoManager) {
}

AddCommand::AddOptions AddCommand::parseArguments(const std::vector<std::string>& args) const {
    AddOptions options;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const auto& arg = args[i];
        
        if (arg == "--dry-run" || arg == "-n") {
            options.dryRun = true;
        } else if (arg == "--interactive" || arg == "-i") {
            options.interactive = true;
        } else if (arg == "--patch" || arg == "-p") {
            options.patch = true;
        } else if (arg == "--update" || arg == "-u") {
            options.update = true;
        } else if (arg == "--force" || arg == "-f") {
            options.force = true;
        } else if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--exclude" && i + 1 < args.size()) {
            options.excludePatterns.push_back(args[++i]);
        } else if (arg.find("-") == 0) {
            // Unknown option - treat as error or ignore?
            eventBus_->notify({Event::WARNING_MESSAGE, 
                              "Unknown option: " + arg, "add"});
        } else {
            // Regular file/directory
            options.files.push_back(arg);
        }
    }
    
    return options;
}

bool AddCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "add";
    
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", SOURCE});
        return false;
    }

    // Парсим аргументы
    AddOptions options = parseArguments(args);
    
    if (options.showHelp) {
        showHelp();
        return true;
    }
    
    if (options.files.empty() && !options.interactive) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "No files specified", SOURCE});
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Use 'svcs add --help' for usage information", SOURCE});
        return false;
    }
    
    // Dry-run режим
    if (options.dryRun) {
        return showDryRun(options.files);
    }
    
    // Интерактивный режим
    if (options.interactive) {
        return addInteractive();
    }
    
    // Обычное добавление файлов
    bool success = true;
    int addedCount = 0;
    
    for (const auto& file : options.files) {
        if (file == ".") {
            // Добавление всей директории
            auto files = getAllFilesInDirectory(".");
            for (const auto& f : files) {
                if (shouldAddFile(f, options.force)) {
                    if (repoManager_->addFileToStaging(f)) {
                        addedCount++;
                        eventBus_->notify({Event::DEBUG_MESSAGE, 
                                          "Added: " + f, SOURCE});
                    } else {
                        success = false;
                        eventBus_->notify({Event::ERROR_MESSAGE, 
                                          "Failed to add: " + f, SOURCE});
                    }
                }
            }
        } else if (std::filesystem::is_directory(file)) {
            // Добавление директории
            auto files = getAllFilesInDirectory(file);
            for (const auto& f : files) {
                if (shouldAddFile(f, options.force)) {
                    if (repoManager_->addFileToStaging(f)) {
                        addedCount++;
                        eventBus_->notify({Event::DEBUG_MESSAGE, 
                                          "Added: " + f, SOURCE});
                    } else {
                        success = false;
                        eventBus_->notify({Event::ERROR_MESSAGE, 
                                          "Failed to add: " + f, SOURCE});
                    }
                }
            }
        } else {
            // Добавление отдельного файла
            if (shouldAddFile(file, options.force)) {
                if (repoManager_->addFileToStaging(file)) {
                    addedCount++;
                    eventBus_->notify({Event::SAVE_SUCCESS, 
                                      "Added: " + file, SOURCE});
                } else {
                    success = false;
                    eventBus_->notify({Event::ERROR_MESSAGE, 
                                      "Failed to add: " + file, SOURCE});
                }
            }
        }
    }
    
    if (addedCount > 0) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Successfully added " + std::to_string(addedCount) + " file(s)", SOURCE});
    }
    
    return success;
}

bool AddCommand::addInteractive() const {
    const std::string SOURCE = "add";
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Interactive add mode", SOURCE});
    
    // Get all files in current directory (excluding .svcs directory)
    auto allFiles = getAllFilesInDirectory(".");
    if (allFiles.empty()) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "No files found to add", SOURCE});
        return true;
    }
    
    // Display files for selection
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Available files:", SOURCE});
    
    int index = 1;
    for (const auto& file : allFiles) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "[" + std::to_string(index++) + "] " + file, SOURCE});
    }
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Adding all available files...", SOURCE});
    
    bool success = true;
    int addedCount = 0;
    
    for (const auto& file : allFiles) {
        if (shouldAddFile(file, false)) {
            if (repoManager_->addFileToStaging(file)) {
                addedCount++;
                eventBus_->notify({Event::SAVE_SUCCESS, 
                                  "Added: " + file, SOURCE});
            } else {
                success = false;
                eventBus_->notify({Event::ERROR_MESSAGE, 
                                  "Failed to add: " + file, SOURCE});
            }
        }
    }
    
    if (addedCount > 0) {
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Successfully added " + std::to_string(addedCount) + " file(s)", SOURCE});
    }
    
    return success;
}

bool AddCommand::addPatch(const std::string& filePath) const {
    const std::string SOURCE = "add";
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Patch mode not yet implemented for file: " + filePath, SOURCE});
    
    // For now, return false to indicate this feature isn't available
    return false;
}

bool AddCommand::showDryRun(const std::vector<std::string>& files) const {
    const std::string SOURCE = "add";
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Dry run - following files would be added:", SOURCE});
    
    int fileCount = 0;
    
    for (const auto& file : files) {
        if (file == ".") {
            auto allFiles = getAllFilesInDirectory(".");
            for (const auto& f : allFiles) {
                if (shouldAddFile(f, false)) {
                    eventBus_->notify({Event::GENERAL_INFO, "  " + f, SOURCE});
                    fileCount++;
                }
            }
        } else if (std::filesystem::is_directory(file)) {
            auto dirFiles = getAllFilesInDirectory(file);
            for (const auto& f : dirFiles) {
                if (shouldAddFile(f, false)) {
                    eventBus_->notify({Event::GENERAL_INFO, "  " + f, SOURCE});
                    fileCount++;
                }
            }
        } else {
            if (shouldAddFile(file, false)) {
                eventBus_->notify({Event::GENERAL_INFO, "  " + file, SOURCE});
                fileCount++;
            }
        }
    }
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Total: " + std::to_string(fileCount) + " file(s) would be added", SOURCE});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Run without --dry-run to actually add files", SOURCE});
    
    return true;
}

bool AddCommand::shouldAddFile(const std::string& file, bool force) const {
    if (!std::filesystem::exists(file)) {
        eventBus_->notify({Event::WARNING_MESSAGE, 
                          "File does not exist: " + file, "add"});
        return false;
    }
    
    if (std::filesystem::is_directory(file)) {
        return false;
    }
    return true;
}

std::vector<std::string> AddCommand::getAllFilesInDirectory(const std::string& directory) const {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (filePath.find("/.svcs/") == std::string::npos && 
                    filePath.find("\\.svcs\\") == std::string::npos) {
                    files.push_back(filePath);
                }
            }
        }
    } catch (const std::exception& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error reading directory: " + std::string(e.what()), "add"});
    }
    return files;
}

std::string AddCommand::getDescription() const {
    return "Add files to the staging area";
}

std::string AddCommand::getUsage() const {
    return "svcs add [options] <file1> [file2] [file3] ...";
}

void AddCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, "Usage: " + getUsage(), "add"});
    eventBus_->notify({Event::GENERAL_INFO, "Description: " + getDescription(), "add"});
    eventBus_->notify({Event::GENERAL_INFO, "Files are added to the staging area for the next commit", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "Options:", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --dry-run, -n       Show what would be added without actually adding", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --interactive, -i   Interactive mode", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --patch, -p         Patch mode (select parts of files to add)", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --update, -u        Only add tracked files", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --force, -f         Force add ignored files", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --exclude <pattern> Exclude files matching pattern", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  --help, -h          Show this help", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "Examples:", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs add file.txt              Add single file", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs add .                     Add all files in current directory", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs add --dry-run .           Show what would be added", "add"});
    eventBus_->notify({Event::GENERAL_INFO, "  svcs add src/ include/         Add directories", "add"});
}