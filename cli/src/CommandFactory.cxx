#include "../include/CommandFactory.hxx"
#include "../include/ICommand.hxx"
#include "../include/InitCommand.hxx" 
#include <stdexcept>
#include <iostream>
#include <utility> 

std::once_flag CommandFactory::initInstanceFlag_;

CommandFactory::CommandFactory(std::shared_ptr<ISubject> eventBus) 
    : eventBus_(std::move(eventBus)) 
{
    registerDefaultCommands();
}

// --- getInstance (Потокобезопасный Singleton) ---
CommandFactory& CommandFactory::getInstance(std::shared_ptr<ISubject> eventBus) {
    // Используем лямбда-функцию для потокобезопасной ленивой инициализации
    static CommandFactory* instance = nullptr;
    
    // Если eventBus передан, но instance еще не создан (первый вызов)
    if (eventBus && !instance) {
        std::call_once(initInstanceFlag_, [&]() {
            instance = new CommandFactory(std::move(eventBus));
        });
    }
    
    if (!instance) {
        throw std::runtime_error("CommandFactory not initialized! Must call getInstance with EventBus first.");
    }
    return *instance;
}

// --- registerDefaultCommands (ИЗМЕНЕНИЕ) ---
void CommandFactory::registerDefaultCommands() {
    std::cout << "DEBUG: CommandFactory constructor" << std::endl;
    
    // ИЗМЕНЕНИЕ: Теперь лямбда-функция ПРИНИМАЕТ EventBus
    registerCommand("init", [](std::shared_ptr<ISubject> bus) -> std::unique_ptr<ICommand> {
        std::cout << "DEBUG: Creating InitCommand instance" << std::endl;
        return std::make_unique<InitCommand>(bus); // Используем конструктор InitCommand(ISubject)
    });
    
    registerCommand("add", [](std::shared_ptr<ISubject> bus) -> std::unique_ptr<ICommand> {
        std::cout << "DEBUG: Creating AddCommand instance" << std::endl;
        // return std::make_unique<AddCommand>(bus);
        return nullptr;
    });
    
    // ... другие команды ...
    
    std::cout << "DEBUG: CommandFactory registered " << creators_.size() << " commands" << std::endl;
}

// --- registerCommand (ИЗМЕНЕНИЕ) ---
void CommandFactory::registerCommand(const std::string& name,
                                   std::function<std::unique_ptr<ICommand>(std::shared_ptr<ISubject>)> creator) {
    creators_[name] = std::move(creator);
}

// --- createCommand (ИЗМЕНЕНИЕ) ---
std::unique_ptr<ICommand> CommandFactory::createCommand(const std::string& name) {
    std::cout << "DEBUG: Creating command: " << name << std::endl;
    
    auto it = creators_.find(name);
    if (it != creators_.end()) {
        // ИЗМЕНЕНИЕ: Вызываем хранитель, ПЕРЕДАВАЯ сохраненный EventBus
        std::unique_ptr<ICommand> command = it->second(eventBus_); 
        
        std::cout << "DEBUG: Command created successfully: " << name << std::endl;
        std::cout << "DEBUG: Command pointer: " << command.get() << std::endl;
        return command;
    }
    
    std::cout << "DEBUG: Command not found: " << name << std::endl;
    return nullptr;
}