/**
 * @file ServerBaseCommand.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of ServerBaseCommand class.
 * @details Provides concrete implementation of common server command functionality
 * including event notification methods.
 *
 * @russian
 * @brief Реализация класса ServerBaseCommand.
 * @details Предоставляет конкретную реализацию общей функциональности серверных команд,
 * включая методы уведомления о событиях.
 */

#include "../include/ServerBaseCommand.hxx"

namespace svcs::server::cli {

ServerBaseCommand::ServerBaseCommand(std::shared_ptr<ISubject> event_bus,
                                     std::shared_ptr<RepositoryManager> repo_manager)
    : event_bus_(std::move(event_bus))
    , repo_manager_(std::move(repo_manager))
{
}

void ServerBaseCommand::notifyInfo(const std::string& message) const
{
    if (event_bus_) {
        event_bus_->notify(Event{Event::GENERAL_INFO, message, "ServerCommand"});
    }
}

void ServerBaseCommand::notifyError(const std::string& message) const
{
    if (event_bus_) {
        event_bus_->notify(Event{Event::ERROR_MESSAGE, message, "ServerCommand"});
    }
}

}