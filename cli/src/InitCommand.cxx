#include "../include/InitCommand.hxx"

bool InitCommand::execute(const std::vector<std::string>& args) {
    logInfo("Initializing new repository");
    
    std::string path = ".";
    if (!args.empty()) {
        path = args[0];
        logDebug("Custom path specified: " + path);
    }
    
    try {
        auto repo = createRepository(path);
        bool success = repo->init();
        
        if (success) {
            logSuccess("Repository initialized successfully at " + path + "/.svcs");
        } else {
            logError("Failed to initialize repository");
        }
        return success;
        
    } catch (const std::exception& e) {
        logError("Exception during init: " + std::string(e.what()));
        return false;
    }
}