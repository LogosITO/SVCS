/**
 * @file MockHelpService.hxx
 * @brief Mock implementation of the HelpService interface for unit testing purposes.
 *
 * @details This class inherits from HelpService and overrides its abstract methods 
 * (`getAvailableCommands`, `getCommandDescription`, `showCommandHelp`) with controlled 
 * implementations. It includes utility methods (`setAvailableCommands`, `wasCommandHelpCalled`, etc.) 
 * to allow tests to configure the mock's behavior and inspect its interactions. 
 * This mock is essential for unit testing the **HelpCommand** without depending 
 * on the actual implementation of the CLI command registration system.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include "../../../cli/include/HelpService.hxx"
#include "MockSubject.hxx"

// Forward declaration if necessary, but included headers should suffice
// struct Event; 

/**
 * @class MockHelpService
 * @brief A mock implementation of the HelpService for testing the HelpCommand.
 *
 * @details This mock allows test cases to:
 * 1. **Configure** the list of available commands.
 * 2. **Configure** the descriptions for specific commands.
 * 3. **Verify** which command's help was requested by the `HelpCommand`.
 * 4. Use the provided MockSubject for emitting debug information during testing.
 */
class MockHelpService : public HelpService {
private:
    /// @brief Storage for the list of available commands to be returned by the mock.
    std::vector<std::string> availableCommands_;
    
    /// @brief Storage for pre-configured command descriptions.
    std::map<std::string, std::string> commandDescriptions_;
    
    /// @brief Records the names of commands whose help was requested via showCommandHelp.
    std::set<std::string> calledCommandHelp_;
    
    /// @brief The mock event bus used for optional debug output.
    std::shared_ptr<MockSubject> mockEventBus_;

public:
    /**
     * @brief Constructor for MockHelpService.
     * @details Initializes the base HelpService with lambda functions that delegate 
     * to the mock's own control methods, and stores the mock event bus.
     * @param mockEventBus The shared pointer to the MockSubject used for notifications.
     */
    explicit MockHelpService(const std::shared_ptr<MockSubject>& mockEventBus)
        : HelpService(
            mockEventBus, // bus
            [this]() { return this->getAvailableCommands(); }, // commandsFunc
            [this](const std::string& cmd) { return this->getCommandDescription(cmd); }, // descriptionFunc  
            [this](const std::string& cmd) { this->showCommandHelp(cmd); }, // helpFunc
            [](const std::string& cmd) { return "Usage for " + cmd; } // usageFunc - simplified usage for mock
          ),
          mockEventBus_(mockEventBus) {
    }

    // --- Implementations of HelpService Abstract Methods ---

    /**
     * @brief Returns the pre-configured list of available commands.
     * @return A vector of command names.
     */
    [[nodiscard]] std::vector<std::string> getAvailableCommands() const {
        return availableCommands_;
    }
    
    /**
     * @brief Returns the pre-configured description for a given command.
     * @param commandName The name of the command.
     * @return The configured description, or "No description available" if not set.
     */
    [[nodiscard]] std::string getCommandDescription(const std::string& commandName) const {
        auto it = commandDescriptions_.find(commandName);
        if (it != commandDescriptions_.end()) {
            return it->second;
        }
        return "No description available";
    }
    
    /**
     * @brief Records that help was requested for the given command.
     * @details Also emits a debug message to the mock event bus.
     * @param commandName The name of the command whose help is requested.
     */
    void showCommandHelp(const std::string& commandName) {
        calledCommandHelp_.insert(commandName);
        if (mockEventBus_) {
            mockEventBus_->notify({Event::DEBUG_MESSAGE, 
                                  "MockHelpService: Showing help for " + commandName, "help"});
        }
    }
    
    // --- Test Control Methods ---
    
    /**
     * @brief Sets the list of command names to be returned by getAvailableCommands().
     * @param commands The vector of command names.
     */
    void setAvailableCommands(const std::vector<std::string>& commands) {
        availableCommands_ = commands;
    }
    
    /**
     * @brief Configures a description for a specific command name.
     * @param commandName The name of the command.
     * @param description The description to return for that command.
     */
    void setCommandDescription(const std::string& commandName, const std::string& description) {
        commandDescriptions_[commandName] = description;
    }
    
    /**
     * @brief Checks if showCommandHelp() was called for a specific command.
     * @param commandName The name of the command to check.
     * @return true if showCommandHelp() was called with this name, false otherwise.
     */
    [[nodiscard]] bool wasCommandHelpCalled(const std::string& commandName) const {
        return calledCommandHelp_.find(commandName) != calledCommandHelp_.end();
    }
    
    /**
     * @brief Resets the mock's internal state (called commands, lists, and descriptions).
     */
    void clear() {
        calledCommandHelp_.clear();
        availableCommands_.clear();
        commandDescriptions_.clear();
    }
};