/**
 * @file ClearCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Tests for the 'clear' command. Implementation of the ClearCommandTest test fixture methods.
 *
 * @russian
 * @brief Тесты для команды 'clear'. Реализация методов тестового фикстура ClearCommandTest.
 */

#include "ClearCommandTest.hxx"

#include <iostream>
#include <stdexcept>
#include <fstream>

namespace svcs::test::cli::utils {

void ClearCommandTest::SetUp() {
    mockEventBus = std::make_shared<MockSubject>();
    repoManager = std::make_shared<RepositoryManager>(mockEventBus);
    command = std::make_unique<ClearCommand>(mockEventBus, repoManager);

    testDir = std::filesystem::temp_directory_path() / "svcs_test_clear";
    std::filesystem::remove_all(testDir);
    std::filesystem::create_directories(testDir);
    std::filesystem::current_path(testDir);

    if (!repoManager->initializeRepository(".", true)) {
        throw std::runtime_error("Failed to initialize repository in SetUp!");
    }

    mockEventBus->clear();
    originalCin = std::cin.rdbuf();
}

void ClearCommandTest::TearDown() {
    if (originalCin) {
        std::cin.rdbuf(originalCin);
    }

    std::filesystem::current_path(std::filesystem::temp_directory_path());

    if (std::filesystem::exists(testDir)) {
        std::filesystem::remove_all(testDir);
    }

    mockEventBus->clear();
}

void ClearCommandTest::createTestFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(testDir / filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open test file for writing: " + filename);
    }
    file << content;
    file.close();
}

void ClearCommandTest::createTestDirectory(const std::string& dirname) const {
    std::filesystem::create_directories(testDir / dirname);
}

void ClearCommandTest::simulateUserInput(const std::string& input) {
    inputStream.str(input);
    inputStream.clear();
    std::cin.rdbuf(inputStream.rdbuf());
}

bool ClearCommandTest::repositoryExists() const {
    return std::filesystem::exists(testDir / ".svcs");
}

}