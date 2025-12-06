#include "IntHelpCommandTest.hxx"
#include <algorithm>
#include "MockHelpServiceAdapter.hxx"

namespace svcs::test::cli::utils {

void HelpCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->mockHelpService = std::make_shared<MockHelpService>(mockEventBus);

    // Создаем адаптер
    auto mockHelpServiceAdapter = std::make_shared<svcs::test::cli::mocks::MockHelpServiceAdapter>(mockHelpService);

    // Используем адаптер для создания команды
    this->command = std::make_unique<HelpCommand>(this->mockEventBus, mockHelpServiceAdapter);

    this->mockEventBus->clear();
}

void HelpCommandTest::TearDown() {
    if (mockEventBus) {
        mockEventBus->clear();
    }
    if (mockHelpService) {
        mockHelpService->clear();
    }
}

bool HelpCommandTest::containsMessage(const std::vector<svcs::services::Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const svcs::services::Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

int HelpCommandTest::countMessages(const std::vector<svcs::services::Event>& notifications, const std::string& message) {
    return std::count_if(notifications.begin(), notifications.end(),
        [&message](const svcs::services::Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

}