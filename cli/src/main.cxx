/**
 * @file main.cxx
 * @brief Main entry point and command-line interface (CLI) driver for the SVCS application.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/CommandFactory.hxx"
#include "../../services/logging/Logger.hxx"
#include "../../services/EventBus.hxx"

#include <memory>
#include <vector>

int main(int argc, char* argv[]) {
    // СОЗДАЕМ долгоживущие объекты
    auto eventBus = std::make_shared<EventBus>();
    auto cli_logger = std::make_shared<Logger>("VCS.CLI");
    auto repo_logger = std::make_shared<Logger>("VCS.Repository");
    
    eventBus->attach(cli_logger);
    eventBus->attach(repo_logger);

    // СОЗДАЕМ RepositoryManager и передаем ему eventBus
    auto repoManager = std::make_shared<RepositoryManager>(eventBus);
    
    // СОЗДАЕМ фабрику команд - теперь передаем ДВА аргумента
    CommandFactory factory(eventBus, repoManager);

    if (argc < 2) {
        cli_logger->info("SVCS - Simple Version Control System");
        cli_logger->info("Available commands: init, add");
        return 1;
    }

    std::string command_name = argv[1];
    
    if (command_name == "version" || command_name == "--version" || command_name == "-v") {
        cli_logger->info("SVCS version 1.0.0");
        return 0;
    }

    if (command_name == "help" || command_name == "--help" || command_name == "-h") {
        cli_logger->info("SVCS - Simple Version Control System");
        cli_logger->info("Available commands:");
        cli_logger->info("  init    - Initialize new repository");
        cli_logger->info("  add     - Add files to staging area");
        cli_logger->info("  version - Show version information");
        return 0;
    }

    auto command = factory.createCommand(command_name);

    if (!command) {
        cli_logger->error("Unknown command: " + command_name);
        cli_logger->info("Use 'svcs help' to see available commands");
        return 1;
    }

    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }

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