/**
 * @file InitCommandTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the InitCommandIntegrationTest fixture methods.
 *
 * @russian
 * @brief Реализация методов фикстура InitCommandIntegrationTest.
 */


#include "InitCommandIntegrationTest.hxx"

#include <memory>

void InitCommandIntegrationTest::TearDown() {
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::remove_all(testDir);
    mockEventBus->notifications.clear();
}

void InitCommandIntegrationTest::SetUp() {
    this->mockEventBus = std::make_shared<MockSubject>();
    this->repoManager = std::make_shared<RepositoryManager>(this->mockEventBus);
    this->command = std::make_unique<InitCommand>(this->mockEventBus, this->repoManager);
    this->testDir = std::filesystem::temp_directory_path() / "svcs_test_init";
    std::filesystem::remove_all(this->testDir);
    std::filesystem::create_directories(this->testDir);
    std::filesystem::current_path(this->testDir);
}