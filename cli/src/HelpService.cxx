/**
 * @file HelpService.cxx
 * @brief Implementation of the HelpService class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/HelpService.hxx"
#include "../../services/ISubject.hxx"

#include <algorithm>

HelpService::HelpService(std::shared_ptr<ISubject> bus,
                         std::function<std::vector<std::string>()> getCommands,
                         std::function<std::string(const std::string&)> getDescription,
                         std::function<void(const std::string&)> showHelp,
                         std::function<std::string(const std::string&)> getUsage)
    : eventBus_(bus), 
      getCommandsCallback_(getCommands),
      getDescriptionCallback_(getDescription),
      showHelpCallback_(showHelp),
      getUsageCallback_(getUsage) {
}

std::vector<std::string> HelpService::getAvailableCommands() const {
    if (getCommandsCallback_) {
        return getCommandsCallback_();
    }
    return {};
}

std::string HelpService::getCommandDescription(const std::string& commandName) const {
    if (getDescriptionCallback_) {
        return getDescriptionCallback_(commandName);
    }
    return "Help service not properly initialized";
}

void HelpService::showCommandHelp(const std::string& commandName) const {
    if (showHelpCallback_) {
        showHelpCallback_(commandName);
    } else if (eventBus_) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Help service not properly initialized", "HelpService"});
    }
}

bool HelpService::commandExists(const std::string& commandName) const {
    auto commands = getAvailableCommands();
    return std::find(commands.begin(), commands.end(), commandName) != commands.end();
}

std::string HelpService::getCommandUsage(const std::string& commandName) const {
    if (getUsageCallback_) {
        return getUsageCallback_(commandName);
    }
    
    // Fallback: try to get usage from description if available
    std::string description = getCommandDescription(commandName);
    if (description != "Unknown command" && description != "Help service not properly initialized") {
        return "svcs " + commandName + " [arguments]";
    }
    
    return "";
}