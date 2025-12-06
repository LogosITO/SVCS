/**
* @file VersionCommand.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the VersionCommand class.
 *
 * @russian
 * @brief Реализация класса VersionCommand.
 */

#include <utility>

#include "../include/VersionCommand.hxx"
#include "../../services/ISubject.hxx"

namespace svcs::cli {

using namespace svcs::services;

VersionCommand::VersionCommand(std::shared_ptr<ISubject> subject)
    : eventBus_(std::move(subject)) {
}

bool VersionCommand::execute(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return true;
        }
    }
    
    eventBus_->notify({Event::GENERAL_INFO, getVersionString(), "version"});
    eventBus_->notify({Event::GENERAL_INFO, getBuildInfo(), "version"});
    eventBus_->notify({Event::GENERAL_INFO, getCopyright(), "version"});
    
    return true;
}

std::string VersionCommand::getDescription() const {
    return "Show version information";
}

std::string VersionCommand::getUsage() const {
    return "svcs version";
}

void VersionCommand::showHelp() const {
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Usage: " + getUsage(), "version"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Description: " + getDescription(), "version"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "Shows the current version of SVCS, build information, and copyright details.", "version"});
    eventBus_->notify({Event::HELP_MESSAGE, 
                      "This command does not accept any arguments.", "version"});
}

std::string VersionCommand::getVersionString() {
    return "SVCS (Simple Version Control System) version 1.0.0";
}

std::string VersionCommand::getBuildInfo() {
    std::string buildInfo = "Build: ";
    
    // Detect build type
#ifdef NDEBUG
    buildInfo += "Release";
#else
    buildInfo += "Debug";
#endif
    
    // Add compiler information if available
#ifdef __clang__
    buildInfo += " | Compiler: Clang";
    buildInfo += " | Version: " + std::string(__clang_version__);
#elif defined(__GNUC__)
    buildInfo += " | Compiler: GCC";
    buildInfo += " | Version: " + std::to_string(__GNUC__) + "." + 
                 std::to_string(__GNUC_MINOR__) + "." + 
                 std::to_string(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    buildInfo += " | Compiler: MSVC";
    buildInfo += " | Version: " + std::to_string(_MSC_VER);
#else
    buildInfo += " | Compiler: Unknown";
#endif
    
    // Add C++ standard version
    buildInfo += " | C++ Standard: " + std::to_string(__cplusplus);
    
    return buildInfo;
}

std::string VersionCommand::getCopyright() {
    return "Copyright (c) 2025 LogosITO. Licensed under MIT License.";
}

}