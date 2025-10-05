/**
 * @file SaveCommand.cxx
 * @brief Implementation of the SaveCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include <utility>

#include "../include/SaveCommand.hxx"
#include "../../services/ISubject.hxx"

SaveCommand::SaveCommand(std::shared_ptr<ISubject> subject,
                         std::shared_ptr<RepositoryManager> repoManager)
    : eventBus_(std::move(subject)), repoManager_(std::move(repoManager)) {
}

bool SaveCommand::execute(const std::vector<std::string>& args) {
    const std::string SOURCE = "save";
    
    // Check for help FIRST - before any other validation
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return true;
        }
    }
    
    // Check if repository is initialized
    if (!repoManager_->isRepositoryInitialized()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Not a SVCS repository. Run 'svcs init' first.", SOURCE});
        return false;
    }
    
    // Parse commit message
    std::string message = parseMessage(args);
    if (!validateMessage(message)) {
        return false;
    }
    
    // Check if there are staged changes
    if (!hasStagedChanges()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "No changes staged for save. Use 'svcs add' to stage files first.", SOURCE});
        return false;
    }
    
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Saving changes with message: " + message, SOURCE});
    
    // Create the save point (commit)
    if (createSavePoint(message)) {
        eventBus_->notify({Event::SAVE_SUCCESS, 
                          "Changes saved successfully!", SOURCE});
        return true;
    } else {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Failed to save changes", SOURCE});
        return false;
    }
}

std::string SaveCommand::getDescription() const {
    return "Save staged changes to the repository";
}

std::string SaveCommand::getUsage() const {
    return "svcs save -m \"message\"";
}

void SaveCommand::showHelp() const {
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Usage: " + getUsage(), "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Description: " + getDescription(), "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Creates a permanent snapshot of all staged changes with the given message.", "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Options:", "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  -m, --message <msg>    Message describing the changes (required)", "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Examples:", "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  svcs save -m \"Add new feature\"", "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  svcs save --message \"Fix bug in calculation\"", "save"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Note: Use 'svcs add' to stage files before saving.", "save"});
}

std::string SaveCommand::parseMessage(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        if ((args[i] == "-m" || args[i] == "--message") && i + 1 < args.size()) {
            return args[i + 1];
        }
    }
    return "";
}

bool SaveCommand::validateMessage(const std::string& message) const {
    if (message.empty()) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Save message is required. Use -m or --message option.", "save"});
        eventBus_->notify({Event::GENERAL_INFO, 
                          "Example: svcs save -m \"Your message here\"", "save"});
        return false;
    }
    
    if (message.length() < 2) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Save message is too short. Please provide a descriptive message.", "save"});
        return false;
    }
    
    return true;
}

bool SaveCommand::hasStagedChanges() const {
    auto stagedFiles = repoManager_->getStagedFiles();
    return !stagedFiles.empty();
}

bool SaveCommand::createSavePoint(const std::string& message) const {
    try {
        eventBus_->notify({Event::DEBUG_MESSAGE, 
                          "Creating save point with message: " + message, "save"});

        if (repoManager_->saveStagedChanges(message)) {
            eventBus_->notify({Event::SAVE_SUCCESS, 
                              "Save point created successfully!", "save"});
            return true;
        } else {
            eventBus_->notify({Event::ERROR_MESSAGE, 
                              "Failed to create save point", "save"});
            return false;
        }
        
    } catch (const std::exception& e) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Error creating save point: " + std::string(e.what()), "save"});
        return false;
    }
}