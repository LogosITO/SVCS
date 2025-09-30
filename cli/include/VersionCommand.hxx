/**
 * @file VersionCommand.hxx
 * @brief Declaration of the VersionCommand class for displaying version information.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include "ICommand.hxx"
#include "../../services/ISubject.hxx"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Command for displaying version information about SVCS.
 *
 * The VersionCommand shows the current version, build information,
 * and copyright details.
 */
class VersionCommand : public ICommand {
private:
    std::shared_ptr<ISubject> eventBus_;

public:
    /**
     * @brief Constructs the VersionCommand.
     *
     * @param subject Shared pointer to the event bus for notifications.
     */
    explicit VersionCommand(std::shared_ptr<ISubject> subject);
    
    /**
     * @brief Executes the version command.
     *
     * @param args Command arguments (ignored for version command).
     * @return true always, as version command cannot fail.
     */
    bool execute(const std::vector<std::string>& args) override;
    
    /**
     * @brief Gets the name of the command.
     * @return "version"
     */
    std::string getName() const override { return "version"; }
    
    /**
     * @brief Gets the description of the command.
     * @return "Show version information"
     */
    std::string getDescription() const override;
    
    /**
     * @brief Gets the usage syntax of the command.
     * @return "svcs version"
     */
    std::string getUsage() const override;
    
    /**
     * @brief Shows detailed help information for this command.
     */
    void showHelp() const override;
    
private:
    /**
     * @brief Gets the full version string.
     * @return Formatted version information.
     */
    std::string getVersionString() const;
    
    /**
     * @brief Gets build information (debug/release, compiler, etc.).
     * @return Build information string.
     */
    std::string getBuildInfo() const;
    
    /**
     * @brief Gets copyright information.
     * @return Copyright string.
     */
    std::string getCopyright() const;
};