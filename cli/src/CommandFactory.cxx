/**
 * @file CommandFactory.cxx
 * @brief Implementation of the CommandFactory class, which is responsible for dynamically creating ICommand objects and managing their lifecycle and dependencies.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/Utils.hxx"
#include "../include/CommandFactory.hxx"
#include "../include/InitCommand.hxx"
#include "../include/ClearCommand.hxx"
#include "../include/AddCommand.hxx"
#include "../include/RemoveCommand.hxx"
#include "../include/HelpService.hxx"
#include "../include/HelpCommand.hxx"
#include "../include/VersionCommand.hxx"
#include "../include/SaveCommand.hxx"
#include "../include/StatusCommand.hxx"
#include "../include/HistoryCommand.hxx"
#include "../include/UndoCommand.hxx"
#include "../include/BranchCommand.hxx"
#include "../include/MergeCommand.hxx"

#include "../../server/include/HubCommand.hxx"

#include <iostream>
#include <memory>
#include <utility>

static bool isInitializing = false;

CommandFactory::CommandFactory(std::shared_ptr<ISubject> bus, 
                             std::shared_ptr<RepositoryManager> repoManager) 
    : event_bus(std::move(bus)), repo_manager(std::move(repoManager)) {
    
    if (isInitializing) {
        std::cerr << "ERROR: Recursive CommandFactory construction detected!" << std::endl;
        return;
    }
    
    isInitializing = true;
    printDebug("CommandFactory constructor called");
    
    registerDefaultCommands();
    
    isInitializing = false;
    printDebug("CommandFactory initialization completed");
}
void CommandFactory::registerDefaultCommands() {
    printDebug("CommandFactory initializing...");

    registerCommand("version", [](const std::shared_ptr<ISubject>& bus,
                                 const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating VersionCommand instance");
        return std::make_unique<VersionCommand>(bus);
    });
    
    registerCommand("init", [](const std::shared_ptr<ISubject>& bus,
                              const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating InitCommand instance");
        return std::make_unique<InitCommand>(bus, repoManager);
    });

    registerCommand("hub", [](const std::shared_ptr<ISubject>& bus,
                                const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating HubCommand instance");
        return std::make_unique<HubCommand>(bus, repoManager);
    });

    registerCommand("clear", [](const std::shared_ptr<ISubject>& bus,
                            const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating ClearCommand instance");
        return std::make_unique<ClearCommand>(bus, repoManager);
    });
    
    registerCommand("add", [](const std::shared_ptr<ISubject>& bus,
                             const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating AddCommand instance");
        return std::make_unique<AddCommand>(bus, repoManager);
    });

    registerCommand("remove", [](const std::shared_ptr<ISubject>& bus,
                             const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating AddCommand instance");
        return std::make_unique<RemoveCommand>(bus, repoManager);
    });

    registerCommand("save", [](const std::shared_ptr<ISubject>& bus,
                          const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating SaveCommand instance");
        return std::make_unique<SaveCommand>(bus, repoManager);
    });

    registerCommand("status", [](const std::shared_ptr<ISubject>& bus,
                          const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating StatusCommand instance");
        return std::make_unique<StatusCommand>(bus, repoManager);
    });

    registerCommand("history", [](const std::shared_ptr<ISubject>& bus,
                             const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating HistoryCommand instance");
        return std::make_unique<HistoryCommand>(bus, repoManager);
    });

    registerCommand("undo", [](const std::shared_ptr<ISubject>& bus,
                             const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating UndoCommand instance");
        return std::make_unique<UndoCommand>(bus, repoManager);
    });

    registerCommand("branch", [](const std::shared_ptr<ISubject>& bus,
                             const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating BranchCommand instance");
        auto branch_manager = std::make_shared<BranchManager>(bus);
        return std::make_unique<BranchCommand>(bus, branch_manager);
    });

    registerCommand("merge", [](const std::shared_ptr<ISubject>& bus,
                             const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
        printDebug("Creating MergeCommand instance");
        return std::make_unique<MergeCommand>(bus, repoManager);
    });
    
    registerCommand("help", [this](const std::shared_ptr<ISubject>& bus,
                                  const std::shared_ptr<RepositoryManager>& repoManager) -> std::unique_ptr<ICommand> {
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
    if (auto command = createCommand(name)) {
        return command->getDescription();
    }
    return "Unknown command";
}

void CommandFactory::showCommandHelp(const std::string& name) const {
    if (auto command = createCommand(name)) {
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
    printDebug("Registered command" + name);
}

std::unique_ptr<ICommand> CommandFactory::createCommand(const std::string& name) const {
    auto it = creators.find(name);
    if (it == creators.end()) {
        printDebug("Command not found: " + name);
        return nullptr;
    }
    
    printDebug("Creating command: " + name);
    return it->second(event_bus, repo_manager);
}

std::vector<std::string> CommandFactory::getRegisteredCommands() const {
    std::vector<std::string> commands;
    for (const auto& pair : creators) {
        commands.push_back(pair.first);
    }
    return commands;
}