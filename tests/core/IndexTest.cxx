/**
 * @file IndexTest.cxx
 * @brief Unit tests for the Index class, verifying staging area management, file modification tracking, and index persistence.
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../../core/include/Index.hxx"
#include "../../core/include/Blob.hxx"
#include "../../core/include/ObjectStorage.hxx" 

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <thread>

namespace fs = std::filesystem;

/**
 * @brief Mock implementation of ISubject (Event Bus).
 * * This mock silences all notifications, ensuring that Index logic is tested
 * without side effects from the event system.
 */
class MockSubject : public ISubject {
public:
    void notify(const Event& event) const override { } 
    void attach(std::shared_ptr<IObserver> observer) override { }
    void detach(std::shared_ptr<IObserver> observer) override { } 
    ~MockSubject() override = default;
};


// Глобальный mock subject для использования
static MockSubject g_mock_subject;

/**
 * @brief Mock implementation of ObjectStorage.
 * * This mock substitutes the actual filesystem I/O for object saving and loading 
 * with an in-memory map, allowing for fast, isolated testing of the Index's interaction 
 * with the storage layer.
 */
class MockObjectStorage : public ObjectStorage {
public:
    mutable std::map<std::string, std::string> saved_blobs;
    
    // Используем реальный временный путь вместо /mock/path
    explicit MockObjectStorage(const fs::path& temp_dir)
        : ObjectStorage((temp_dir / ".svcs" / "objects").string(), std::make_shared<MockSubject>(g_mock_subject))  
    {}  
    
    bool saveObject(const VcsObject& object) const override {
        if (auto blob = dynamic_cast<const Blob*>(&object)) {
            saved_blobs[blob->getHashId()] = blob->serialize();
            return true;
        }
        return true;
    }
    
    std::unique_ptr<VcsObject> loadObject(const std::string& hash) const override {
        auto it = saved_blobs.find(hash);
        if (it != saved_blobs.end()) {
            return std::make_unique<Blob>(it->second);
        }
        return nullptr;
    }
    
    void clear() {
        saved_blobs.clear();
    }
    
    bool is_blob_saved(const std::string& hash) const {
        return saved_blobs.find(hash) != saved_blobs.end();
    }
    
    size_t get_saved_blobs_count() const {
        return saved_blobs.size();
    }
};

void create_test_file(const fs::path& path, const std::string& content) {
    std::ofstream out(path);
    if (out.is_open()) {
        out << content;
    }
}

class IndexTest : public ::testing::Test {
protected:
    fs::path temp_dir; 
    fs::path repo_root; 
    fs::path vcs_root;
    std::unique_ptr<MockObjectStorage> mock_storage;
    std::unique_ptr<Index> index;

    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "svcs_index_test";
        repo_root = temp_dir / "repo";
        vcs_root = repo_root / ".svcs";
        
        std::cout << "Setting up test in: " << temp_dir << std::endl;
        
        // Убедимся, что директории созданы
        fs::create_directories(repo_root);
        fs::create_directories(vcs_root);
        
        ASSERT_TRUE(fs::exists(repo_root)) << "Repo root was not created!";
        ASSERT_TRUE(fs::exists(vcs_root)) << "VCS root was not created!";
        
        // ПЕРЕДАЕМ temp_dir в MockObjectStorage вместо фиктивного пути
        mock_storage = std::make_unique<MockObjectStorage>(temp_dir);
        index = std::make_unique<Index>(vcs_root, repo_root, *mock_storage);
        
        std::cout << "Test setup complete" << std::endl;
    }

    void TearDown() override {
        std::cout << "Tearing down test..." << std::endl;
        index.reset();
        mock_storage.reset();
        if (fs::exists(temp_dir)) {
            fs::remove_all(temp_dir);
        }
    }
    
    static void create_file_with_delay(const fs::path& path, const std::string& content, int ms_delay = 10) {
        if (fs::exists(path)) {
            fs::remove(path);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_delay));
        
        std::ofstream out(path);
        if (!out.is_open()) {
            throw std::runtime_error("Failed to create file: " + path.string());
        }
        out << content;
        out.close();
        
        // Убедимся, что файл создан
        if (!fs::exists(path)) {
            throw std::runtime_error("File was not created: " + path.string());
        }
    }
    
    // Вспомогательный метод для вывода содержимого индекса
    static void print_index_contents() {
        std::cout << "Index contents:" << std::endl;
        // Если у Index есть метод для получения всех записей, используйте его
        // Иначе этот метод нужно адаптировать под ваш интерфейс Index
    }
};

TEST_F(IndexTest, BasicFileOperations) {
    // Простейший тест для проверки базовой функциональности
    fs::path rel_path = "simple.txt";
    fs::path full_path = repo_root / rel_path;
    
    std::cout << "=== BasicFileOperations Test ===" << std::endl;
    std::cout << "Repo root: " << repo_root << std::endl;
    std::cout << "Full path: " << full_path << std::endl;
    
    // Создаем файл
    create_file_with_delay(full_path, "simple content", 20);
    ASSERT_TRUE(fs::exists(full_path));
    std::cout << "File created successfully" << std::endl;
    
    // Пробуем прочитать файл
    std::ifstream test_in(full_path);
    ASSERT_TRUE(test_in.is_open());
    std::string test_content;
    test_in >> test_content;
    test_in.close();
    std::cout << "File read successfully" << std::endl;
    
    // Пробуем staged
    try {
        index->stage_file(rel_path);
        std::cout << "File staged successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "ERROR staging file: " << e.what() << std::endl;
        FAIL() << "Staging failed: " << e.what();
    }
    
    // Проверяем, что запись появилась в индексе
    const IndexEntry* entry = index->getEntry(rel_path);
    if (entry == nullptr) {
        std::cout << "ERROR: Entry not found in index after staging!" << std::endl;
        FAIL() << "Entry not found in index";
    } else {
        std::cout << "Entry found in index: " << entry->file_path << std::endl;
        std::cout << "Blob hash: " << entry->blob_hash << std::endl;
    }
    
    SUCCEED();
}

TEST_F(IndexTest, StageNewFile_AddsCorrectEntry) {
    fs::path rel_path = "test.txt";
    fs::path full_path = repo_root / rel_path;
    std::string content = "This is the content for staging.";

    std::cout << "Creating test file: " << full_path << std::endl;
    create_file_with_delay(full_path, content);
    
    // Проверим, что файл создался
    ASSERT_TRUE(fs::exists(full_path)) << "Test file was not created!";
    ASSERT_TRUE(fs::is_regular_file(full_path)) << "Test file is not a regular file!";
    
    std::cout << "Staging file..." << std::endl;
    // Stage the file
    EXPECT_NO_THROW(index->stage_file(rel_path));

    std::cout << "Checking entry..." << std::endl;
    // Check entry was created
    const IndexEntry* entry = index->getEntry(rel_path);
    ASSERT_NE(entry, nullptr) << "Entry was not created in index!";
    EXPECT_EQ(entry->file_path, rel_path) << "Entry path mismatch!";
    
    std::cout << "Checking blob storage..." << std::endl;
    // Verify blob was saved
    EXPECT_TRUE(mock_storage->is_blob_saved(entry->blob_hash)) 
        << "Blob with hash " << entry->blob_hash << " was not saved in storage!";
    EXPECT_GT(mock_storage->get_saved_blobs_count(), 0) 
        << "No blobs were saved in storage!";
        
    std::cout << "Test completed successfully!" << std::endl;
}

TEST_F(IndexTest, StageFile_ThrowsOnNonExistentFile) {
    fs::path rel_path = "missing.txt";
    fs::path full_path = repo_root / rel_path;
    
    // Убедимся, что файла действительно нет
    ASSERT_FALSE(fs::exists(full_path)) << "File should not exist but it does!";
    
    try {
        index->stage_file(rel_path);
        FAIL() << "Expected std::runtime_error but no exception was thrown!";
    } catch (const std::runtime_error& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
        // Проверяем, что сообщение об ошибке содержит ожидаемый текст
        EXPECT_TRUE(std::string(e.what()).find("Cannot stage") != std::string::npos ||
                   std::string(e.what()).find("invalid") != std::string::npos ||
                   std::string(e.what()).find("not exist") != std::string::npos)
            << "Unexpected error message: " << e.what();
    } catch (const std::exception& e) {
        FAIL() << "Unexpected exception type: " << typeid(e).name() << " - " << e.what();
    } catch (...) {
        FAIL() << "Unknown exception type thrown!";
    }
}

TEST_F(IndexTest, IsFileModified_False_WhenNoChange) {
    fs::path rel_path = "stable.txt";
    fs::path full_path = repo_root / rel_path;
    std::string content = "Same content.";
    
    create_file_with_delay(full_path, content, 20);
    index->stage_file(rel_path);

    EXPECT_FALSE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, IsFileModified_True_WhenSizeChanges) {
    fs::path rel_path = "size_change.txt";
    fs::path full_path = repo_root / rel_path;
    
    create_file_with_delay(full_path, "Start", 20);
    index->stage_file(rel_path);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    create_file_with_delay(full_path, "Start and end.", 0);
    
    EXPECT_TRUE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, IsFileModified_True_WhenFileDeleted) {
    fs::path rel_path = "to_delete.txt";
    fs::path full_path = repo_root / rel_path;
    
    create_file_with_delay(full_path, "Will be deleted.", 20);
    index->stage_file(rel_path);

    fs::remove(full_path);
    
    EXPECT_TRUE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, IsFileModified_True_WhenUntrackedExists) {
    fs::path rel_path = "untracked.txt";
    fs::path full_path = repo_root / rel_path;
    
    create_file_with_delay(full_path, "This is new.", 20);
    
    EXPECT_TRUE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, Persistence_SaveAndLoad) {
    fs::path rel_path_1 = "file1.txt";
    fs::path rel_path_2 = "sub/file2.txt";
    fs::create_directories(repo_root / "sub");

    create_file_with_delay(repo_root / rel_path_1, "AAA", 20);
    create_file_with_delay(repo_root / rel_path_2, "BBB", 20);

    index->stage_file(rel_path_1);
    index->stage_file(rel_path_2);

    // Save current state
    index->save(); 

    // Get references to original data
    const IndexEntry* orig_entry1 = index->getEntry(rel_path_1);
    const IndexEntry* orig_entry2 = index->getEntry(rel_path_2);
    ASSERT_NE(orig_entry1, nullptr);
    ASSERT_NE(orig_entry2, nullptr);

    std::string hash1 = orig_entry1->blob_hash;
    long long size2 = orig_entry2->file_size;

    // Create new index and load
    Index loaded_index(vcs_root, repo_root, *mock_storage);

    const IndexEntry* loaded_entry1 = loaded_index.getEntry(rel_path_1);
    const IndexEntry* loaded_entry2 = loaded_index.getEntry(rel_path_2);

    ASSERT_NE(loaded_entry1, nullptr);
    ASSERT_NE(loaded_entry2, nullptr);

    EXPECT_EQ(loaded_entry1->blob_hash, hash1);
    EXPECT_EQ(loaded_entry2->file_size, size2);
}