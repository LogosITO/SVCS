/**
 * @file commit_test.cxx
 * @brief Unit tests for the Commit class, verifying object construction, hash stability (parent order), and serialization/deserialization integrity.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../../core/include/Commit.hxx"

#include <gtest/gtest.h>

const std::string HASH_TREE_EMPTY = "4b825dc642cb6eb9a060e54bf8d69288fbee4904a0a0a0a0a0a0a0a0a0a0a0a";
const std::string HASH_PARENT_A = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
const std::string HASH_PARENT_B = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
const std::string HASH_TREE_NEW = "c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1c1";
const std::string HASH_PARENT_P1 = "d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2";
const std::string HASH_PARENT_P2 = "e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3e3";

Commit createTestCommit(
    const std::string& tree_hash,
    const std::vector<std::string>& parents,
    const std::string& message,
    std::time_t timestamp = 1600000000
) {
    return Commit(
        tree_hash,
        parents,
        "Test Author <test@example.com>",
        message,
        timestamp
    );
}

TEST(CommitTest, BasicInitialCommit) {
    Commit commit = createTestCommit(
        HASH_TREE_EMPTY, 
        {}, 
        "Initial commit message."
    );

    EXPECT_FALSE(commit.getHashId().empty()) << "Commit hash must be calculated.";
    EXPECT_EQ(commit.getType(), "commit");
    EXPECT_EQ(commit.getTreeHash(), HASH_TREE_EMPTY);
    EXPECT_TRUE(commit.getParentHashes().empty());
    EXPECT_EQ(commit.getHashId().length(), 64);
}

TEST(CommitTest, HashIsStableRegardlessOfParentOrder) {
    Commit commit1 = createTestCommit(
        HASH_TREE_EMPTY, 
        {HASH_PARENT_A, HASH_PARENT_B},
        "Merge commit."
    );

    Commit commit2 = createTestCommit(
        HASH_TREE_EMPTY, 
        {HASH_PARENT_B, HASH_PARENT_A},
        "Merge commit."
    );

    EXPECT_EQ(commit1.getHashId(), commit2.getHashId())
        << "Commit hash must be stable when parent list is reordered.";
}

TEST(CommitTest, HashChangesWithDifferentMessage) {
    Commit commit1 = createTestCommit(HASH_TREE_EMPTY, {}, "Message V1");
    Commit commit2 = createTestCommit(HASH_TREE_EMPTY, {}, "Message V2");

    EXPECT_NE(commit1.getHashId(), commit2.getHashId())
        << "Changing commit message must change the commit hash.";
}

TEST(CommitTest, SerializationDeserializationRoundTrip) {
    std::string complex_message = "Feature commit.\n\n- Line 2\n- Line 3";
    std::time_t custom_time = 1700000000;
    
    Commit original = createTestCommit(
        HASH_TREE_NEW, 
        {HASH_PARENT_P1, HASH_PARENT_P2}, 
        complex_message, 
        custom_time
    );

    std::string serialized = original.serialize();
    
    Commit restored = Commit::deserialize(serialized);
    
    EXPECT_EQ(original.getHashId(), restored.getHashId()) << "Restored commit hash must match original hash.";
    EXPECT_EQ(original.getTreeHash(), restored.getTreeHash());
    EXPECT_EQ(original.getMessage(), restored.getMessage());
    EXPECT_EQ(original.getAuthor(), restored.getAuthor());
    EXPECT_EQ(original.getTimestamp(), restored.getTimestamp());
    
    EXPECT_EQ(original.getParentHashes().size(), 2);
    EXPECT_EQ(restored.getParentHashes().size(), 2);
    EXPECT_EQ(original.getParentHashes()[0], restored.getParentHashes()[0]);
    EXPECT_EQ(original.getParentHashes()[1], restored.getParentHashes()[1]);
}