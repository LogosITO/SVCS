/**
 * @file CompositeCommandFactory.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the CompositeCommandFactory class.
 * @details Provides concrete implementation for combining multiple command factories
 * into a unified interface. Handles command creation, help display, and command
 * discovery across all registered factories.
 *
 * @russian
 * @brief Реализация класса CompositeCommandFactory.
 * @details Предоставляет конкретную реализацию для объединения нескольких фабрик команд
 * в единый интерфейс. Обрабатывает создание команд, отображение справки и обнаружение
 * команд во всех зарегистрированных фабриках.
 */

#include "../include/CompositeCommandFactory.hxx"
#include "../../cli/include/CommandFactory.hxx"
#include "../../server/include/ServerCommandFactory.hxx"

#include <iostream>
#include <unordered_set>
#include <algorithm>

namespace svcs::integration {

using namespace svcs::server::cli;

CompositeCommandFactory::CompositeCommandFactory(std::shared_ptr<ISubject> event_bus,
                                               std::shared_ptr<RepositoryManager> repo_manager)
    : event_bus_(std::move(event_bus))
    , repo_manager_(std::move(repo_manager))
{
    registerFactory([this](const std::string& name) {
        CommandFactory standard_factory(event_bus_, repo_manager_);
        return standard_factory.createCommand(name);
    });

    registerFactory([this](const std::string& name) {
        ServerCommandFactory server_factory(event_bus_, repo_manager_);
        return server_factory.createCommand(name);
    });
}

void CompositeCommandFactory::registerFactory(std::function<std::unique_ptr<ICommand>(const std::string&)> factory) {
    factories_.push_back(std::move(factory));
}

std::unique_ptr<ICommand> CompositeCommandFactory::createCommand(const std::string& name) const {
    for (const auto& factory : factories_) {
        auto command = factory(name);
        if (command) {
            return command;
        }
    }
    return nullptr;
}

std::vector<std::string> CompositeCommandFactory::getAllCommandNames() const {
    std::unordered_set<std::string> unique_names;
    std::vector<std::string> all_names;

    CommandFactory standard_factory(event_bus_, repo_manager_);
    auto standard_commands = standard_factory.getRegisteredCommands();
    for (const auto& cmd : standard_commands) {
        if (unique_names.insert(cmd).second) {
            all_names.push_back(cmd);
        }
    }

    ServerCommandFactory server_factory(event_bus_, repo_manager_);
    auto server_commands = server_factory.getRegisteredCommands();
    for (const auto& cmd : server_commands) {
        if (unique_names.insert(cmd).second) {
            all_names.push_back(cmd);
        }
    }

    for (size_t i = 2; i < factories_.size(); ++i) {
        std::vector<std::string> common_commands = {"help", "version", "init", "add", "commit", "status"};
        for (const auto& cmd : common_commands) {
            auto test_command = factories_[i](cmd);
            if (test_command && unique_names.insert(cmd).second) {
                all_names.push_back(cmd);
            }
        }
    }

    return all_names;
}

std::string CompositeCommandFactory::getCommandDescription(const std::string& name) const {
    auto command = createCommand(name);
    if (command) {
        return command->getDescription();
    }
    return "Unknown command: " + name;
}

void CompositeCommandFactory::showCommandHelp(const std::string& name) const {
    auto command = createCommand(name);
    if (command) {
        command->showHelp();
    } else {
        std::cerr << "Error: Unknown command '" << name << "'" << std::endl;
        std::cerr << "Use 'svcs help' to see all available commands." << std::endl;
    }
}

bool CompositeCommandFactory::commandExists(const std::string& name) const {
    return createCommand(name) != nullptr;
}

size_t CompositeCommandFactory::getFactoryCount() const {
    return factories_.size();
}

}