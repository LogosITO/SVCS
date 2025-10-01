/**
 * @file ClearCommandTest.cxx
 * @brief Тесты для команды 'clear'. Реализация методов тестового фикстура ClearCommandTest.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "ClearCommandTest.hxx" 

#include <iostream>
#include <stdexcept>
#include <fstream>

// ----------------------------------------------------------------------
// РЕАЛИЗАЦИЯ МЕТОДОВ ТЕСТОВОГО ФИКСТУРА ClearCommandTest
// ----------------------------------------------------------------------

/**
 * @brief Sets up the test environment before each test case.
 * * 1. Initializes the `RepositoryManager` and `ClearCommand` using the `MockSubject`.
 * * 2. Creates a unique temporary directory, changes the current path to it, and initializes a new SVCS repository.
 * * 3. Saves the original `std::cin` buffer for user input simulation and clears initial events.
 * @throw std::runtime_error if repository initialization fails.
 */
void ClearCommandTest::SetUp() {
    mockEventBus = std::make_shared<MockSubject>();
    // Использование реального RepositoryManager с MockSubject
    repoManager = std::make_shared<RepositoryManager>(mockEventBus); 
    command = std::make_unique<ClearCommand>(mockEventBus, repoManager);
    
    // Создаем временную директорию для тестов
    testDir = std::filesystem::temp_directory_path() / "svcs_test_clear";
    std::filesystem::remove_all(testDir);
    std::filesystem::create_directories(testDir);
    std::filesystem::current_path(testDir); // Переходим в тестовую директорию
    
    if (!repoManager->initializeRepository(".", true)) {
        throw std::runtime_error("Failed to initialize repository in SetUp!");
    }
    // Очищаем события, сгенерированные при инициализации, чтобы начать тест с чистого листа
    mockEventBus->clear();
    
    // Сохраняем оригинальный cin для восстановления
    originalCin = std::cin.rdbuf();
}

/**
 * @brief Tears down the test environment after each test case.
 * * Restores the original `std::cin` buffer, reverts the current working directory, 
 * recursively deletes the test directory (`testDir`), and clears captured events.
 */
void ClearCommandTest::TearDown() {
    // Восстанавливаем оригинальный cin
    if (originalCin) {
        std::cin.rdbuf(originalCin);
    }
    
    // Возвращаемся в исходную временную директорию перед удалением
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    
    // Удаляем тестовую директорию
    if (std::filesystem::exists(testDir)) {
        std::filesystem::remove_all(testDir);
    }
    
    mockEventBus->clear();
}

/**
 * @brief Creates a test file inside the test repository directory.
 * @param filename The name of the file to create (relative to testDir).
 * @param content The content to write into the file.
 * @throw std::runtime_error if the file cannot be opened for writing.
 */
void ClearCommandTest::createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(testDir / filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open test file for writing: " + filename);
    }
    file << content;
    file.close();
}

/**
 * @brief Creates a test directory inside the test repository directory.
 * @param dirname The name of the directory to create (relative to testDir).
 */
void ClearCommandTest::createTestDirectory(const std::string& dirname) {
    std::filesystem::create_directories(testDir / dirname);
}

/**
 * @brief Simulates user input by redirecting std::cin to read from an internal string stream.
 * @details This is crucial for testing commands (like 'clear') that require confirmation.
 * @param input The string (e.g., "y\n" or "N\n") that will be read as user input.
 */
void ClearCommandTest::simulateUserInput(const std::string& input) {
    inputStream.str(input);
    // Очищаем флаги потока, чтобы он был готов к чтению
    inputStream.clear(); 
    // Перенаправляем std::cin
    std::cin.rdbuf(inputStream.rdbuf());
}

/**
 * @brief Checks if the .svcs repository directory exists in the test directory.
 * @return true if the .svcs directory exists, false otherwise.
 */
bool ClearCommandTest::repositoryExists() const {
    return std::filesystem::exists(testDir / ".svcs");
}