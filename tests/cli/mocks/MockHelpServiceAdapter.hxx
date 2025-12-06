#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "../../../services/Event.hxx"
#include "../../../cli/include/HelpService.hxx"
#include "MockHelpService.hxx"

namespace svcs::test::cli::mocks {

/**
 * @brief Адаптер для использования MockHelpService с HelpCommand
 */
class MockHelpServiceAdapter : public svcs::cli::HelpService {
private:
    std::shared_ptr<MockHelpService> mockHelpService_;

public:
    explicit MockHelpServiceAdapter(std::shared_ptr<MockHelpService> mockHelpService)
        : HelpService(
            // 1. bus - nullptr, так как MockHelpService сам отправляет события
            nullptr,
            // 2. Коллбэк для получения доступных команд
            [this]() -> std::vector<std::string> {
                return mockHelpService_->getAvailableCommands();
            },
            // 3. Коллбэк для получения описания команды
            [this](const std::string& commandName) -> std::string {
                return mockHelpService_->getCommandDescription(commandName);
            },
            // 4. Коллбэк для показа справки по команде
            [this](const std::string& commandName) {
                mockHelpService_->showCommandHelp(commandName);
            },
            // 5. Коллбэк для получения использования команды
            [this](const std::string& commandName) -> std::string {
                // Простая реализация или делегируем mock
                return "Usage: svcs " + commandName + " [options]";
            }
          ),
          mockHelpService_(mockHelpService) {
    }

    // Методы доступа к mock для тестов
    std::shared_ptr<MockHelpService> getMock() const {
        return mockHelpService_;
    }
};

} // namespace svcs::test::cli::mocks