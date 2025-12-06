/**
 * @file ServerCommandFactory.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the ServerCommandFactory class.
 * @details Provides concrete implementation for creating server administration
 * commands using the factory pattern with creator functions.
 *
 * @russian
 * @brief Реализация класса ServerCommandFactory.
 * @details Предоставляет конкретную реализацию для создания команд
 * администрирования сервера с использованием фабричного шаблона с функциями-создателями.
 */

#include <iostream>

#include "../include/ServerCommandFactory.hxx"
#include "../include/HubCommand.hxx"
#include "../include/RepoCommand.hxx"

namespace svcs::server::cli {

ServerCommandFactory::ServerCommandFactory(std::shared_ptr<ISubject> bus,
                                         std::shared_ptr<RepositoryManager> repoManager)
    : event_bus(std::move(bus))
    , repo_manager(std::move(repoManager))
{
    registerDefaultCommands();
}

void ServerCommandFactory::registerDefaultCommands() {
    registerCommand("hub",
        [](std::shared_ptr<ISubject> bus, std::shared_ptr<RepositoryManager> repoManager) {
            return std::make_unique<HubCommand>(std::move(bus), std::move(repoManager));
        });

    registerCommand("repo",
        [](std::shared_ptr<ISubject> bus, std::shared_ptr<RepositoryManager> repoManager) {
            return std::make_unique<RepoCommand>(std::move(bus), std::move(repoManager));
        });
}

void ServerCommandFactory::registerCommand(const std::string& name,
                                         std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                                                               std::shared_ptr<RepositoryManager>)> creator) {
    creators[name] = std::move(creator);
}

std::string ServerCommandFactory::getCommandDescription(const std::string& name) const {
    auto command = createCommand(name);
    if (command) {
        return command->getDescription();
    }
    return "Unknown server command: " + name;
}

void ServerCommandFactory::showCommandHelp(const std::string& name) const {
    auto command = createCommand(name);
    if (command) {
        command->showHelp();
    } else {
        std::cerr << "Unknown server command: " << name << std::endl;
    }
}

std::unique_ptr<ICommand> ServerCommandFactory::createCommand(const std::string& name) const {
    auto it = creators.find(name);
    if (it != creators.end()) {
        return it->second(event_bus, repo_manager);
    }
    return nullptr;
}

std::vector<std::string> ServerCommandFactory::getRegisteredCommands() const {
    std::vector<std::string> commandNames;
    for (const auto& [name, creator] : creators) {
        commandNames.push_back(name);
    }
    return commandNames;
}

bool ServerCommandFactory::commandExists(const std::string& name) const {
    return creators.find(name) != creators.end();
}

}