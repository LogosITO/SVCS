/**
 * @file CommandFactory.cxx
 * @brief Implementation of the CommandFactory class, which is responsible for dynamically creating ICommand objects and managing their lifecycle and dependencies.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/CommandFactory.hxx"
#include "../include/InitCommand.hxx"
#include "../include/AddCommand.hxx"
#include "../include/HelpService.hxx"
#include "../include/HelpCommand.hxx"
#include "../include/VersionCommand.hxx"
#include "../include/SaveCommand.hxx"

#include <iostream>
#include <memory>

static bool isInitializing = false;

CommandFactory::CommandFactory(std::shared_ptr<ISubject> bus, 
                             std::shared_ptr<RepositoryManager> repoManager) 
    : event_bus(bus), repo_manager(repoManager) {
    
    if (isInitializing) {
        std::cerr << "ERROR: Recursive CommandFactory construction detected!" << std::endl;
        return;
    }
    
    isInitializing = true;
    std::cout << "DEBUG: CommandFactory constructor called" << std::endl;
    
    registerDefaultCommands();
    
    isInitializing = false;
    std::cout << "DEBUG: CommandFactory initialization completed" << std::endl;
}
void CommandFactory::registerDefaultCommands() {
    std::cout << "DEBUG: CommandFactory initializing..." << std::endl;

    registerCommand("version", [](std::shared_ptr<ISubject> bus, 
                                 std::shared_ptr<RepositoryManager> repoManager) -> std::unique_ptr<ICommand> {
        return std::make_unique<VersionCommand>(bus);
    });
    
    registerCommand("init", [](std::shared_ptr<ISubject> bus, 
                              std::shared_ptr<RepositoryManager> repoManager) -> std::unique_ptr<ICommand> {
        std::cout << "DEBUG: Creating InitCommand instance" << std::endl;
        return std::make_unique<InitCommand>(bus, repoManager);
    });
    
    registerCommand("add", [](std::shared_ptr<ISubject> bus, 
                             std::shared_ptr<RepositoryManager> repoManager) -> std::unique_ptr<ICommand> {
        std::cout << "DEBUG: Creating AddCommand instance" << std::endl;
        return std::make_unique<AddCommand>(bus, repoManager);
    });

    registerCommand("save", [](std::shared_ptr<ISubject> bus, 
                          std::shared_ptr<RepositoryManager> repoManager) -> std::unique_ptr<ICommand> {
        std::cout << "DEBUG: Creating SaveCommand instance" << std::endl;
        return std::make_unique<SaveCommand>(bus, repoManager);
    });
    
    registerCommand("help", [this](std::shared_ptr<ISubject> bus, 
                                  std::shared_ptr<RepositoryManager> repoManager) -> std::unique_ptr<ICommand> {
        auto helpService = std::make_shared<HelpService>(
            bus,
            [this]() { return this->getRegisteredCommands(); },
            [this](const std::string& cmd) { return this->getCommandDescription(cmd); },
            [this](const std::string& cmd) { this->showCommandHelp(cmd); },
            [this](const std::string& cmd) { 
                auto command = this->createCommand(cmd);
                return command ? command->getUsage() : "";
            }
        );
        
        return std::make_unique<HelpCommand>(bus, helpService);
    });

    std::cout << "DEBUG: CommandFactory registered " << creators.size() << " commands" << std::endl;
}

std::string CommandFactory::getCommandDescription(const std::string& name) const {
    auto command = createCommand(name);
    if (command) {
        return command->getDescription();
    }
    return "Unknown command";
}

void CommandFactory::showCommandHelp(const std::string& name) const {
    auto command = createCommand(name);
    if (command) {
        command->showHelp();
    } else {
        // Используем прямой вывод для избежания рекурсии через eventBus
        std::cout << "Unknown command: " << name << std::endl;
        std::cout << "Use 'svcs help' to see available commands" << std::endl;
    }
}

void CommandFactory::registerCommand(const std::string& name,
                                   std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                                                         std::shared_ptr<RepositoryManager>)> creator) {
    creators[name] = std::move(creator);
    std::cout << "DEBUG: Registered command: " << name << std::endl;
}

std::unique_ptr<ICommand> CommandFactory::createCommand(const std::string& name) const {
    auto it = creators.find(name);
    if (it == creators.end()) {
        std::cout << "DEBUG: Command not found: " << name << std::endl;
        return nullptr;
    }
    
    std::cout << "DEBUG: Creating command: " << name << std::endl;
    return it->second(event_bus, repo_manager);
}

std::vector<std::string> CommandFactory::getRegisteredCommands() const {
    std::vector<std::string> commands;
    for (const auto& pair : creators) {
        commands.push_back(pair.first);
    }
    return commands;
}