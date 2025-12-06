/**
* @file IntAddCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the AddCommandTest integration test fixture.
 * @details This file defines the `SetUp` and `TearDown` routines, along with
 * helper methods essential for testing the `AddCommand`. The setup includes:
 * 1. Initializing mock components (MockSubject).
 * 2. Initializing core components (RepositoryManager, AddCommand).
 * 3. Managing a temporary, isolated SVCS repository directory.
 *
 * @russian
 * @brief Реализация интеграционного тестового фикстура AddCommandTest.
 * @details Этот файл определяет процедуры `SetUp` и `TearDown`, а также
 * вспомогательные методы, необходимые для тестирования `AddCommand`. Настройка включает:
 * 1. Инициализацию mock-компонентов (MockSubject).
 * 2. Инициализацию основных компонентов (RepositoryManager, AddCommand).
 * 3. Управление временной изолированной директорией репозитория SVCS.
 */

#include "IntAddCommandTest.hxx"
#include "../mocks/MockSubject.hxx"
#include "../../../core/include/RepositoryManager.hxx"

#include <fstream>
#include <memory>

namespace svcs::test::cli::utils {

void AddCommandTest::SetUp() {
    // ✅ CORRECT: Assign to the member variables (using 'this->' is explicit, but good practice)
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<AddCommand>(this->mockEventBus, this->repoManager);
    
    // Assign to the member testDir
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_add";
    
    std::filesystem::remove_all(this->testDir);
    std::filesystem::create_directories(this->testDir);
    std::filesystem::current_path(this->testDir);
    
    // Инициализируем репозиторий (using the member repoManager)
    this->repoManager->initializeRepository(".", true);
    this->mockEventBus->clear();
}

void AddCommandTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->clear();
}

void AddCommandTest::createTestFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void AddCommandTest::createTestDirectory(const std::string& dirname) const {
    std::filesystem::create_directories(testDir / dirname);
}

}