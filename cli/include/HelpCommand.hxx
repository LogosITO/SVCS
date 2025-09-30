/**
 * @file HelpCommand.hxx
 * @brief Declaration of the HelpCommand class for displaying command help information.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"
#include "../include/HelpService.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Command for displaying help information about other commands.
 *
 * The HelpCommand can show general help with all available commands
 * or detailed help for a specific command. It uses HelpService to
 * retrieve command information without direct dependency on CommandFactory.
 */
class HelpCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;
    std::shared_ptr<HelpService> helpService_;

public:
    /**
     * @brief Constructs the HelpCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     * @param helpService Shared pointer to the help service for retrieving command information.
     */
    HelpCommand(std::shared_ptr<ISubject> subject,
                std::shared_ptr<HelpService> helpService);
    
    /**
     * @brief Executes the help command.
     *
     * @param args Command arguments. If empty, shows general help.
     *              If contains a command name, shows help for that command.
     * @return true always, as help command cannot fail.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "help"
     */
    std::string getName() const override { return "help"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Show help information for commands"
     */
    std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs help [command]"
     */
    std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Shows general help with all available commands.
     */
    void showGeneralHelp();
    
    /**
     * @brief Shows detailed help for a specific command.
     * @param commandName The name of the command to show help for.
     */
    void showCommandHelp(const std::string& commandName);
};