#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "../../../services/Event.hxx"
#include "../../../cli/include/HelpCommand.hxx"
#include "../mocks/MockSubject.hxx"
#include "../mocks/MockHelpService.hxx"

namespace svcs::test::cli::utils {

using Event = svcs::services::Event;
using svcs::cli::HelpCommand;
using svcs::test::cli::mocks::MockSubject;
using svcs::test::cli::mocks::MockHelpService;

class HelpCommandTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    static bool containsMessage(const std::vector<Event>& notifications, const std::string& message);
    int countMessages(const std::vector<Event>& notifications, const std::string& message);

    std::shared_ptr<MockSubject> mockEventBus;
    std::shared_ptr<MockHelpService> mockHelpService;
    std::unique_ptr<HelpCommand> command;
};

}