#include "../core/include/ObjectStorage.hxx"
#include "../core/include/Blob.hxx"
#include "../core/include/Tree.hxx"
#include "../core/include/Commit.hxx"

#include <gtest/gtest.h>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

const std::string DUMMY_HASH_FILE_A = "a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4e5f6a1b2";
const std::string DUMMY_HASH_PARENT_1 = "p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1p1";
const std::string DUMMY_HASH_PARENT_2 = "p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2p2";
const std::string DUMMY_HASH_TREE_ROOT = "t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0";


const std::string TEST_ROOT = "test_repo_root";
const std::string OBJECTS_DIR = TEST_ROOT + "/.svcs/objects";

Blob create_blob(const std::string& content) {
    return Blob(content);
}

class ObjectStorageTest : public ::testing::Test {
protected:
    ObjectStorage* storage;

    void SetUp() override {
        fs::create_directories(OBJECTS_DIR);
        storage = new ObjectStorage(TEST_ROOT);
    }

    void TearDown() override {
        delete storage;
        fs::remove_all(TEST_ROOT);
    }
    
    bool fileExistsOnDisk(const std::string& hash) const {
        std::string path = storage->getObjectPath(hash);
        return fs::exists(path);
    }
};

TEST_F(ObjectStorageTest, SavesObjectToCorrectPath) {
    Blob blob = create_blob("Hello SVCS");
    std::string expected_hash = blob.getHashId();
    
    EXPECT_FALSE(fileExistsOnDisk(expected_hash));
    
    EXPECT_TRUE(storage->saveObject(blob)) << "Saving the object should succeed.";
    
    EXPECT_TRUE(fileExistsOnDisk(expected_hash)) << "Object file must exist after saving.";

    std::string dir_name = expected_hash.substr(0, 2);
    EXPECT_TRUE(fs::exists(fs::path(OBJECTS_DIR) / dir_name)) << "Subdirectory was not created.";
}

TEST_F(ObjectStorageTest, LoadRestoresOriginalBlob) {
    const std::string initial_content = "This is the content of the file.";
    Blob original_blob = create_blob(initial_content);
    std::string original_hash = original_blob.getHashId();

    storage->saveObject(original_blob);
    
    std::unique_ptr<VcsObject> loaded_obj = storage->loadObject(original_hash);
    
    EXPECT_NE(loaded_obj, nullptr) << "Loaded object should not be null.";
    
    EXPECT_EQ(loaded_obj->getType(), "blob");

    EXPECT_EQ(loaded_obj->getHashId(), original_hash);
    
    const Blob* loaded_blob = dynamic_cast<const Blob*>(loaded_obj.get());
    ASSERT_NE(loaded_blob, nullptr) << "Loaded object must be castable to Blob.";
    EXPECT_EQ(loaded_blob->getData(), initial_content);
}

TEST_F(ObjectStorageTest, LoadThrowsExceptionIfNotFound) {
    const std::string fake_hash = "abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789";
    
    EXPECT_THROW({
        storage->loadObject(fake_hash);
    }, std::runtime_error) << "Should throw runtime_error when object is not found.";
}

TEST_F(ObjectStorageTest, LoadRestoresOriginalTree) {
    std::vector<TreeEntry> entries = {
        {"100644", "file.txt", DUMMY_HASH_FILE_A, "blob"}
    };
    Tree original_tree(entries); 
    std::string original_hash = original_tree.getHashId();

    storage->saveObject(original_tree);
    std::unique_ptr<VcsObject> loaded_obj = storage->loadObject(original_hash);

    EXPECT_EQ(loaded_obj->getType(), "tree");
    const Tree* loaded_tree = dynamic_cast<const Tree*>(loaded_obj.get());
    
    ASSERT_NE(loaded_tree, nullptr);
    EXPECT_EQ(loaded_tree->getHashId(), original_hash);
    EXPECT_EQ(loaded_tree->getEntries().size(), 1); 
}

TEST_F(ObjectStorageTest, LoadRestoresOriginalCommit) {
    Commit original_commit(
        DUMMY_HASH_TREE_ROOT,
        {DUMMY_HASH_PARENT_1, DUMMY_HASH_PARENT_2},
        "Alice <alice@example.com>",
        "Testing commit message with\nmultiple lines.",
        1234567890 
    ); 
    std::string original_hash = original_commit.getHashId();

    storage->saveObject(original_commit);
    std::unique_ptr<VcsObject> loaded_obj = storage->loadObject(original_hash);
    
    EXPECT_EQ(loaded_obj->getType(), "commit");
    const Commit* loaded_commit = dynamic_cast<const Commit*>(loaded_obj.get());
    
    ASSERT_NE(loaded_commit, nullptr);
    EXPECT_EQ(loaded_commit->getHashId(), original_hash);
    EXPECT_EQ(loaded_commit->getMessage(), "Testing commit message with\nmultiple lines.");
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
