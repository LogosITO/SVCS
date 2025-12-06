/**
 * @file IntVersionCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of integration tests for the VersionCommand class.
 * @details This file provides the concrete implementation for the VersionCommandTest fixture.
 * Since the `svcs version` command is stateless and does not interact with a repository,
 * the setup focuses on initializing the MockSubject (event bus) to capture the output
 * and the command instance itself. Helper methods are provided for verifying the exact
 * content of the command's notifications.
 *
 * @russian
 * @brief Реализация интеграционных тестов для класса VersionCommand.
 * @details Этот файл предоставляет конкретную реализацию для фикстура VersionCommandTest.
 * Поскольку команда `svcs version` не имеет состояния и не взаимодействует с репозиторием,
 * настройка сосредоточена на инициализации MockSubject (шины событий) для захвата вывода
 * и самого экземпляра команды. Предоставлены вспомогательные методы для проверки точного
 * содержимого уведомлений команды.
 */

#include "IntVersionCommandTest.hxx"

#include <algorithm>

namespace svcs::test::cli::utils {

void VersionCommandTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->command = std::make_unique<VersionCommand>(this->mockEventBus);
    this->mockEventBus->clear();
}

void VersionCommandTest::TearDown() {
    mockEventBus->clear();
}

bool VersionCommandTest::containsMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details.find(message) != std::string::npos;
        });
}

bool VersionCommandTest::containsExactMessage(const std::vector<Event>& notifications, const std::string& message) {
    return std::any_of(notifications.begin(), notifications.end(),
        [&message](const Event& event) {
            return event.details == message;
        });
}

}