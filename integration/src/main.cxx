/**
 * @file main.cxx
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "../include/CompositeCommandFactory.hxx"
#include "../../services/EventBus.hxx"
#include "../../core/include/RepositoryManager.hxx"

using namespace svcs::services;
using namespace svcs::core;
using namespace svcs::integration;

void showGeneralHelp(const CompositeCommandFactory& factory) {
    std::cout << "SVCS - Simple Version Control System\n";
    std::cout << "Version 1.0\n\n";

    std::cout << "Usage: svcs <command> [arguments]\n\n";

    std::cout << "Available Commands:\n";

    auto all_commands = factory.getAllCommandNames();
    if (all_commands.empty()) {
        std::cout << "  No commands available.\n";
    } else {
        std::cout << "  Basic Commands:\n";
        std::vector<std::string> basic_commands = {"init", "add", "commit", "status", "log"};
        for (const auto& cmd : basic_commands) {
            if (factory.commandExists(cmd)) {
                std::cout << "    " << cmd << " - " << factory.getCommandDescription(cmd) << "\n";
            }
        }

        std::cout << "  Server Commands:\n";
        std::vector<std::string> server_commands = {"hub", "repo"};
        for (const auto& cmd : server_commands) {
            if (factory.commandExists(cmd)) {
                std::cout << "    " << cmd << " - " << factory.getCommandDescription(cmd) << "\n";
            }
        }

        std::cout << "  Other Commands:\n";
        for (const auto& cmd : all_commands) {
            if (std::find(basic_commands.begin(), basic_commands.end(), cmd) == basic_commands.end() &&
                std::find(server_commands.begin(), server_commands.end(), cmd) == server_commands.end()) {
                std::cout << "    " << cmd << " - " << factory.getCommandDescription(cmd) << "\n";
            }
        }
    }

    std::cout << "\nFor help on a specific command, use: svcs <command> --help\n";
    std::cout << "Example: svcs init --help\n";
}

int main(int argc, char* argv[]) {
    auto event_bus = std::make_shared<EventBus>();
    auto repo_manager = std::make_shared<RepositoryManager>(event_bus);

    CompositeCommandFactory command_factory(event_bus, repo_manager);

    if (argc < 2) {
        showGeneralHelp(command_factory);
        return 1;
    }

    std::string command_name = argv[1];
    std::vector<std::string> args;

    if (command_name == "--help" || command_name == "-h" || command_name == "help") {
        showGeneralHelp(command_factory);
        return 0;
    }

    if (command_name == "--version" || command_name == "-v") {
        std::cout << "SVCS version 1.0\n";
        return 0;
    }

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            command_factory.showCommandHelp(command_name);
            return 0;
        }

        args.push_back(arg);
    }

    auto command = command_factory.createCommand(command_name);
    if (!command) {
        std::cerr << "Error: Unknown command '" << command_name << "'\n";
        std::cerr << "Use 'svcs help' to see all available commands.\n";
        return 1;
    }

    bool success = command->execute(args);
    return success ? 0 : 1;
}

