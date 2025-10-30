/**
 * @file TreeTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Unit tests for the Tree class, verifying object construction, canonical sorting, entry management, and serialization/deserialization integrity.
 *
 * @russian
 * @brief Модульные тесты для класса Tree, проверяющие построение объектов, каноническую сортировку, управление записями и целостность сериализации/десериализации.
 */
#include "../../core/include/Tree.hxx"

#include <gtest/gtest.h>

const std::string HASH_A = "a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
const std::string HASH_B = "b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1b1";
const std::string HASH_C = "c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2c2";
const std::string HASH_D = "d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3d3";


TEST(TreeTest, HashIsCalculatedAndTypeIsCorrect) {
    std::vector<TreeEntry> entries = {
        createEntry("file_a.txt", HASH_A),
        createEntry("file_b.txt", HASH_B)
    };
    Tree tree(entries);

    EXPECT_FALSE(tree.getHashId().empty()) << "Tree hash should be calculated on creation.";
    EXPECT_EQ(tree.getType(), "tree") << "Tree type must be 'tree'.";
    EXPECT_EQ(tree.getHashId().length(), 64); 
}

TEST(TreeTest, HashIsConsistentRegardlessOfInitialOrder) {
    TreeEntry entry1 = createEntry("file_a.txt", HASH_A);
    TreeEntry entry2 = createEntry("file_b.txt", HASH_B);

    std::vector<TreeEntry> entries_order1 = {entry1, entry2}; 
    Tree tree1(entries_order1);

    std::vector<TreeEntry> entries_order2 = {entry2, entry1}; 
    Tree tree2(entries_order2);

    EXPECT_EQ(tree1.getHashId(), tree2.getHashId())
        << "Tree hash must be stable regardless of the input order of entries (due to internal sorting).";
}

TEST(TreeTest, HashChangesWhenChildHashChanges) {
    TreeEntry entry_v1 = createEntry("config.json", HASH_A);
    TreeEntry entry_v2 = createEntry("config.json", HASH_B); 

    std::vector<TreeEntry> entries1 = {entry_v1}; 
    Tree tree1(entries1);

    std::vector<TreeEntry> entries2 = {entry_v2}; 
    Tree tree2(entries2);

    EXPECT_NE(tree1.getHashId(), tree2.getHashId())
        << "Changing a child object's hash must change the parent Tree hash.";
}

TEST(TreeTest, AddAndUpdateEntry) {
    Tree tree({});
    
    tree.addEntry(createEntry("readme.md", HASH_A));
    EXPECT_EQ(tree.getEntries().size(), 1);
    
    tree.addEntry(createEntry("readme.md", HASH_B));
    
    EXPECT_EQ(tree.getEntries().size(), 1) << "Adding an entry with the same name should update, not duplicate.";
    
    auto entry = tree.findEntry("readme.md");
    EXPECT_TRUE(entry.has_value());
    if (entry.has_value()) {
        EXPECT_EQ(entry->hash_id, HASH_B) << "The entry hash was not updated correctly.";
    }

    tree.addEntry(createEntry("src/", HASH_C));
    EXPECT_EQ(tree.getEntries().size(), 2);
}

TEST(TreeTest, RemoveEntry) {
    std::vector<TreeEntry> entries = {
        createEntry("a.txt", HASH_A),
        createEntry("b.txt", HASH_B)
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
    TreeEntry target = createEntry("target.log", HASH_C);
    std::vector<TreeEntry> entries = {
        createEntry("other.txt", HASH_D),
        target
    };
    Tree tree(entries);

    auto found = tree.findEntry("target.log");
    EXPECT_TRUE(found.has_value());
    if (found.has_value()) {
        EXPECT_EQ(found->hash_id, HASH_C);
    }
    
    auto not_found = tree.findEntry("missing.file");
    EXPECT_FALSE(not_found.has_value());
}

TEST(TreeTest, SerializationAndDeserializationRoundTrip) {
    std::vector<TreeEntry> original_entries = {
        createEntry("z_last.dat", HASH_A), 
        createEntry("a_first.dat", HASH_B) 
    };
    
    std::cout << "Original entries order:\n";
    for (const auto& e : original_entries) {
        std::cout << "  " << e.name << "\n";
    }
    
    Tree original_tree(original_entries);
    
    std::cout << "After Tree constructor:\n";
    for (const auto& e : original_tree.getEntries()) {
        std::cout << "  " << e.name << "\n";
    }
    
    std::string serialized_data = original_tree.serialize();
    std::cout << "Serialized size: " << serialized_data.size() << "\n";
    
    Tree restored_tree = Tree::deserialize(serialized_data);
    
    std::cout << "After deserialization:\n";
    for (const auto& e : restored_tree.getEntries()) {
        std::cout << "  " << e.name << "\n";
    }
    
    EXPECT_EQ(original_tree.getHashId(), restored_tree.getHashId())
        << "Restored tree must have the same hash ID as the original.";
        
    EXPECT_EQ(restored_tree.getEntries().size(), 2);
    if (restored_tree.getEntries().size() == 2) {
        EXPECT_EQ(restored_tree.getEntries()[0].name, "a_first.dat");
        EXPECT_EQ(restored_tree.getEntries()[1].name, "z_last.dat");
        
        EXPECT_EQ(restored_tree.getEntries()[0].hash_id, HASH_B);
        EXPECT_EQ(restored_tree.getEntries()[1].hash_id, HASH_A);
    }
}