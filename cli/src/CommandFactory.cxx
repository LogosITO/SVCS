#include "../include/CommandFactory.hxx"
#include "../include/InitCommand.hxx"
#include "../include/AddCommand.hxx"

#include <iostream>
#include <memory>

CommandFactory::CommandFactory(std::shared_ptr<ISubject> bus, 
                             std::shared_ptr<RepositoryManager> repoManager) 
    : eventBus_(bus), repoManager_(repoManager) {
    registerDefaultCommands();
}

void CommandFactory::registerDefaultCommands() {
    std::cout << "DEBUG: CommandFactory initializing..." << std::endl;
    
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
    
    std::cout << "DEBUG: CommandFactory registered " << creators_.size() << " commands" << std::endl;
}

void CommandFactory::registerCommand(const std::string& name,
                                   std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>,
                                                                         std::shared_ptr<RepositoryManager>)> creator) {
    creators_[name] = std::move(creator);
    std::cout << "DEBUG: Registered command: " << name << std::endl;
}

std::unique_ptr<ICommand> CommandFactory::createCommand(const std::string& name) {
    auto it = creators_.find(name);
    if (it == creators_.end()) {
        std::cout << "DEBUG: Command not found: " << name << std::endl;
        return nullptr;
    }
    
    std::cout << "DEBUG: Creating command: " << name << std::endl;
    return it->second(eventBus_, repoManager_);
}

std::vector<std::string> CommandFactory::getRegisteredCommands() const {
    std::vector<std::string> commands;
    for (const auto& pair : creators_) {
        commands.push_back(pair.first);
    }
    return commands;
}