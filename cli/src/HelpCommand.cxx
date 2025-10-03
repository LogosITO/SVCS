/**
 * @file HelpCommand.cxx
 * @brief Implementation of the HelpCommand class.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/HelpCommand.hxx"
#include "../../services/ISubject.hxx"

HelpCommand::HelpCommand(std::shared_ptr<ISubject> subject,
                         std::shared_ptr<HelpService> helpService)
    : eventBus_(subject), helpService_(helpService) {
}

bool HelpCommand::execute(const std::vector<std::string>& args) {   
    if (args.empty()) {
        showGeneralHelp();
        return true;
    }
    
    std::string commandName = args[0];
    showCommandHelp(commandName);
    return true;
}
std::string HelpCommand::getDescription() const {
    return "Show help information for commands";
}

std::string HelpCommand::getUsage() const {
    return "svcs help [command]";
}

void HelpCommand::showHelp() const {
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Usage: " + getUsage(), "help"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Description: " + getDescription(), "help"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "If no command is specified, shows general help", "help"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "Examples:", "help"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs help          # Show all commands", "help"});
    eventBus_->notify({Event::GENERAL_INFO, 
                      "  svcs help init     # Show help for init command", "help"});
}

void HelpCommand::showGeneralHelp() {
    if (!helpService_) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Help service not available", "help"});
        return;
    }
    
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "SVCS - Simple Version Control System", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Version 1.0.0", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Usage: svcs <command> [options] [arguments]", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Available commands:", "help"});
    
    // Get available commands using HelpService
    auto commands = helpService_->getAvailableCommands();
    
    for (const auto& commandName : commands) {
        // Get command description using HelpService
        std::string description = helpService_->getCommandDescription(commandName);
        eventBus_->notify({Event::HELP_MESSAGE, 
                          "  " + commandName + " - " + description, "help"});
    }
    
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Global options:", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  --version, -v    Show version information", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  --help, -h       Show this help message", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "For more information about a specific command, use:", "help"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "  svcs help <command>", "help"});
}

void HelpCommand::showCommandHelp(const std::string& commandName) {
    if (!helpService_) {
        eventBus_->notify({Event::ERROR_MESSAGE, 
                          "Help service not available", "help"});
        return;
    }
    
    // Use HelpService to show help for specific command
    helpService_->showCommandHelp(commandName);
}