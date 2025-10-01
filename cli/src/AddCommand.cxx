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

bool AddCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "No files specified", "add"});
        return false;
    }
    
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", "add"});
        return false;
    }
    
    bool success = true;
    for (const auto& file : args) {
        if (repoManager_->addFileToStaging(file)) {
            eventBus_->notify({Event::SAVE_SUCCESS, 
                              "Added " + file + " to staging area", "add"});
        } else {
            eventBus_->notify({Event::ERROR_MESSAGE, 
                              "Failed to add " + file, "add"});
            success = false;
        }
    }
    int counter = 0;
    for (const auto& path : args) {
        if (path == ".") {
            auto files = getAllFilesInDirectory(".");
            for (const auto& file : files) {
                if (repoManager_->addFileToStaging(file)) {
                    counter++;
                }
            }
        } else if (std::filesystem::is_directory(path)) {
            auto files = getAllFilesInDirectory(path);
            for (const auto& file : files) {
                if (repoManager_->addFileToStaging(file)) {
                    counter++;
                }
            }
        } else {
            if (repoManager_->addFileToStaging(path)) {
                counter++;
            }
        }
    }
    return success;
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
    return "svcs add <file1> [file2] [file3] ...";
}

void AddCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Usage: " + getUsage(), "add"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Description: " + getDescription(), "add"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Files are added to the staging area for the next commit", "add"});
}