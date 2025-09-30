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
            eventBus_->notify({Event::GENERAL_INFO, 
                              "Added " + file + " to staging area", "add"});
        } else {
            eventBus_->notify({Event::ERROR_MESSAGE, 
                              "Failed to add " + file, "add"});
            success = false;
        }
    }
    
    return success;
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