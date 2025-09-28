#include "../core/include/Tree.hxx"
#include <gtest/gtest.h>

TEST(TreeTest, HashIsCalculatedAndTypeIsCorrect) {
    std::vector<TreeEntry> entries = {
        createEntry("file_a.txt", "hash_a"),
        createEntry("file_b.txt", "hash_b")
    };
    Tree tree(entries);

    EXPECT_FALSE(tree.getHashId().empty()) << "Tree hash should be calculated on creation.";
    EXPECT_EQ(tree.getType(), "tree") << "Tree type must be 'tree'.";
}

TEST(TreeTest, HashIsConsistentRegardlessOfInitialOrder) {
    TreeEntry entry1 = createEntry("file_a.txt", "hash_1");
    TreeEntry entry2 = createEntry("file_b.txt", "hash_2");

    std::vector<TreeEntry> entries_order1 = {entry1, entry2}; 
    Tree tree1(entries_order1);

    std::vector<TreeEntry> entries_order2 = {entry2, entry1}; 
    Tree tree2(entries_order2);

    EXPECT_EQ(tree1.getHashId(), tree2.getHashId())
        << "Tree hash must be stable regardless of the input order of entries.";
}

TEST(TreeTest, HashChangesWhenChildHashChanges) {
    TreeEntry entry_v1 = createEntry("config.json", "old_config_hash_123");
    TreeEntry entry_v2 = createEntry("config.json", "new_config_hash_789");

    std::vector<TreeEntry> entries1 = {entry_v1}; 
    Tree tree1(entries1);

    std::vector<TreeEntry> entries2 = {entry_v2}; 
    Tree tree2(entries2);

    EXPECT_NE(tree1.getHashId(), tree2.getHashId())
        << "Changing a child object's hash must change the parent Tree hash.";
}

TEST(TreeTest, AddAndUpdateEntry) {
    Tree tree({});
    
    tree.addEntry(createEntry("readme.md", "hash_v1"));
    EXPECT_EQ(tree.getEntries().size(), 1);
    
    tree.addEntry(createEntry("readme.md", "hash_v2"));
    
    EXPECT_EQ(tree.getEntries().size(), 1) << "Adding an entry with the same name should update, not duplicate.";
    
    auto entry = tree.findEntry("readme.md");
    EXPECT_TRUE(entry.has_value());
    if (entry.has_value()) {
        EXPECT_EQ(entry->hash_id, "hash_v2") << "The entry hash was not updated correctly.";
    }

    tree.addEntry(createEntry("src/", "hash_src"));
    EXPECT_EQ(tree.getEntries().size(), 2);
}

TEST(TreeTest, RemoveEntry) {
    std::vector<TreeEntry> entries = {
        createEntry("a.txt", "h1"),
        createEntry("b.txt", "h2")
    };
    Tree tree(entries);
    EXPECT_EQ(tree.getEntries().size(), 2);

    bool removed = tree.removeEntry("a.txt");
    EXPECT_TRUE(removed);
    EXPECT_EQ(tree.getEntries().size(), 1);
    EXPECT_FALSE(tree.findEntry("a.txt").has_value()) << "Entry a.txt should be gone.";

    removed = tree.removeEntry("c.txt");
    EXPECT_FALSE(removed);
    EXPECT_EQ(tree.getEntries().size(), 1) << "Size shouldn't change when trying to remove non-existent entry.";
}

TEST(TreeTest, FindEntry) {
    TreeEntry target = createEntry("target.log", "h_target");
    std::vector<TreeEntry> entries = {
        createEntry("other.txt", "h_other"),
        target
    };
    Tree tree(entries);

    auto found = tree.findEntry("target.log");
    EXPECT_TRUE(found.has_value());
    if (found.has_value()) {
        EXPECT_EQ(found->hash_id, "h_target");
    }
    
    auto not_found = tree.findEntry("missing.file");
    EXPECT_FALSE(not_found.has_value());
}

TEST(TreeTest, SerializationAndDeserializationRoundTrip) {
    std::vector<TreeEntry> original_entries = {
        createEntry("z_last.dat", "hash_z"), 
        createEntry("a_first.dat", "hash_a")
    };
    Tree original_tree(original_entries);
    
    std::string serialized_data = original_tree.serialize();
    
    Tree restored_tree = Tree::deserialize(serialized_data);
    
    EXPECT_EQ(original_tree.getHashId(), restored_tree.getHashId())
        << "Restored tree must have the same hash ID as the original.";
        
    EXPECT_EQ(restored_tree.getEntries().size(), 2);
    if (restored_tree.getEntries().size() == 2) {
        EXPECT_EQ(restored_tree.getEntries()[0].name, "a_first.dat");
        EXPECT_EQ(restored_tree.getEntries()[1].name, "z_last.dat");
    }
}
