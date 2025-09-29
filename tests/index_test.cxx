#include "gtest/gtest.h"
#include "../core/include/Index.hxx"
#include "../core/include/Blob.hxx"
#include "../core/include/ObjectStorage.hxx" 

#include <filesystem>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <thread>

namespace fs = std::filesystem;

class MockObjectStorage : public ObjectStorage {
public:
    mutable std::map<std::string, std::string> saved_objects;
    
    MockObjectStorage() : ObjectStorage("/mock/path/vcs_root") {} 
    
    bool saveObject(const VcsObject& object) const override {
        saved_objects[object.getHashId()] = object.serialize();
        return true;
    }
    
    std::unique_ptr<VcsObject> loadObject(const std::string& hash) const {
        auto it = saved_objects.find(hash);
        if (it != saved_objects.end()) {
            return std::make_unique<Blob>(it->second);
        }
        return nullptr;
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
    fs::path vcs_root;
    fs::path repo_root; 
    MockObjectStorage mock_storage;
    std::unique_ptr<Index> index;

    IndexTest() 
      : temp_dir(fs::temp_directory_path() / "svcs_index_test"),
        repo_root(temp_dir / "repo"),
        vcs_root(repo_root / ".svcs")
    {
    }

    void SetUp() override {
        fs::create_directories(repo_root);
        fs::create_directories(vcs_root);
        index = std::make_unique<Index>(vcs_root, repo_root, mock_storage);
    }

    void TearDown() override {
        index.reset();
        fs::remove_all(temp_dir);
    }
};

TEST_F(IndexTest, StageNewFile_AddsCorrectEntry) {
    fs::path rel_path = "test.txt";
    fs::path full_path = repo_root / rel_path;
    std::string content = "This is the content for staging.";

    create_test_file(full_path, content);
    
    // Создаем blob для расчета ожидаемого хеша
    Blob expected_blob(content);
    std::string expected_hash = expected_blob.getHashId();
    
    index->stage_file(rel_path);

    const IndexEntry* entry = index->getEntry(rel_path);
    ASSERT_NE(entry, nullptr);

    EXPECT_EQ(entry->blob_hash, expected_hash);
    EXPECT_EQ(entry->file_size, content.size());
    
    // Проверяем, что blob был сохранен в storage
    EXPECT_TRUE(mock_storage.saved_objects.find(expected_hash) != mock_storage.saved_objects.end());
}

TEST_F(IndexTest, StageFile_ThrowsOnNonExistentFile) {
    fs::path rel_path = "missing.txt";
    EXPECT_THROW(index->stage_file(rel_path), std::runtime_error);
}

TEST_F(IndexTest, IsFileModified_False_WhenNoChange) {
    fs::path rel_path = "stable.txt";
    fs::path full_path = repo_root / rel_path;
    std::string content = "Same content.";
    
    create_test_file(full_path, content);
    index->stage_file(rel_path);

    // Даем файловой системе время обновиться
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    EXPECT_FALSE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, IsFileModified_True_WhenSizeChanges) {
    fs::path rel_path = "size_change.txt";
    fs::path full_path = repo_root / rel_path;
    
    create_test_file(full_path, "Start");
    index->stage_file(rel_path);

    // Ждем немного чтобы время модификации точно изменилось
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    create_test_file(full_path, "Start and end.");
    
    EXPECT_TRUE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, IsFileModified_True_WhenFileDeleted) {
    fs::path rel_path = "to_delete.txt";
    fs::path full_path = repo_root / rel_path;
    
    create_test_file(full_path, "Will be deleted.");
    index->stage_file(rel_path); 

    fs::remove(full_path);

    EXPECT_TRUE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, IsFileModified_True_WhenUntrackedExists) {
    fs::path rel_path = "untracked.txt";
    create_test_file(repo_root / rel_path, "This is new.");
    
    EXPECT_TRUE(index->isFileModified(rel_path));
}

TEST_F(IndexTest, Persistence_SaveAndLoad) {
    fs::path rel_path_1 = "file1.txt";
    fs::path rel_path_2 = "sub/file2.txt";
    fs::create_directories(repo_root / "sub");

    create_test_file(repo_root / rel_path_1, "AAA");
    create_test_file(repo_root / rel_path_2, "BBB");

    index->stage_file(rel_path_1);
    index->stage_file(rel_path_2);

    std::string hash1 = index->getEntry(rel_path_1)->blob_hash;
    long long size2 = index->getEntry(rel_path_2)->file_size;

    // Создаем новый индекс с тем же storage
    Index loaded_index(vcs_root, repo_root, mock_storage);

    const IndexEntry* entry1 = loaded_index.getEntry(rel_path_1);
    const IndexEntry* entry2 = loaded_index.getEntry(rel_path_2);

    ASSERT_NE(entry1, nullptr);
    ASSERT_NE(entry2, nullptr);

    EXPECT_EQ(entry1->blob_hash, hash1);
    EXPECT_EQ(entry2->file_size, size2);
}