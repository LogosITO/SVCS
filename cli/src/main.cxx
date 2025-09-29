#include "../include/CommandFactory.hxx"
#include "../../services/logging/Logger.hxx"
#include "../../services/EventBus.hxx"

#include <memory>
#include <vector>



int main(int argc, char* argv[]) {
    // 1. Создаем центральный Event Bus
    auto eventBus = std::make_shared<EventBus>();

    // 2. Создаем Logger (Observer) и подключаем его к Event Bus
    // Logger теперь сам форматирует вывод, поэтому имя "svcs_cli" должно быть его именем.
    auto cli_logger = std::make_shared<Logger>("VCS.CLI");
    eventBus->attach(cli_logger);

    // 3. CommandFactory получает ссылку на Event Bus
    // Предполагается, что CommandFactory::getInstance был изменен для приема subject.
    auto& factory = CommandFactory::getInstance(eventBus); 

    // --- Логика парсинга команд ---
    if (argc < 2) {
        cli_logger->info("SVCS - Simple Version Control System");
        // ... (остальной вывод help)
        return 1;
    }

    std::string command_name = argv[1];
    
    // Специальные команды (help, version) остаются, но используют логгер
    if (command_name == "version" || command_name == "--version" || command_name == "-v") {
        cli_logger->info("SVCS version 1.0.0");
        return 0;
    }

    auto command = factory.createCommand(command_name);
    
    if (!command) {
        cli_logger->error("Unknown command: " + command_name);
        // ...
        return 1;
    }

    // Подготовка аргументов
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // Запускаем команду
    eventBus->notify({Event::DEBUG_MESSAGE, 
                      "Executing command: " + command_name, "main"});

    try {
        bool success = command->execute(args);
        
        if (success) {
            eventBus->notify({Event::DEBUG_MESSAGE, 
                              "Command " + command_name + " completed successfully", "main"});
            return 0;
        } else {
            eventBus->notify({Event::ERROR_MESSAGE, 
                              "Command " + command_name + " failed", "main"});
            return 1;
        }
        
    } catch (const std::exception& e) {
        eventBus->notify({Event::FATAL_ERROR, 
                          "Exception in command '" + command_name + "': " + e.what(), "main"});
        return 1;
    } catch (...) {
        eventBus->notify({Event::FATAL_ERROR, 
                          "Unknown exception in command '" + command_name + "'", "main"});
        return 1;
    }
}