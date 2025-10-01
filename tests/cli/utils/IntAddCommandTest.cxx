#include "IntAddCommandTest.hxx"
#include "../mocks/MockSubject.hxx"
#include "../../../core/include/RepositoryManager.hxx"

#include <fstream>
#include <memory>

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

void AddCommandTest::createTestFile(const std::string& filename, const std::string& content) {
    std::ofstream file(testDir / filename);
    file << content;
    file.close();
}

void AddCommandTest::createTestDirectory(const std::string& dirname) {
    std::filesystem::create_directories(testDir / dirname);
}