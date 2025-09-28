#include "../core/include/Commit.hxx"
#include <gtest/gtest.h>

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

const std::string EMPTY_TREE_HASH = "4b825dc642cb6eb9a060e54bf8d69288fbee4904";

TEST(CommitTest, BasicInitialCommit) {
    Commit commit = createTestCommit(
        EMPTY_TREE_HASH, 
        {}, 
        "Initial commit message."
    );

    EXPECT_FALSE(commit.getHashId().empty()) << "Commit hash must be calculated.";
    EXPECT_EQ(commit.getType(), "commit");
    EXPECT_EQ(commit.getTreeHash(), EMPTY_TREE_HASH);
    EXPECT_TRUE(commit.getParentHashes().empty());
}

TEST(CommitTest, HashIsStableRegardlessOfParentOrder) {
    std::string parent_a = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    std::string parent_b = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";

    Commit commit1 = createTestCommit(
        EMPTY_TREE_HASH, 
        {parent_a, parent_b},
        "Merge commit."
    );

    Commit commit2 = createTestCommit(
        EMPTY_TREE_HASH, 
        {parent_b, parent_a},
        "Merge commit."
    );

    EXPECT_EQ(commit1.getHashId(), commit2.getHashId())
        << "Commit hash must be stable when parent list is reordered.";
}

TEST(CommitTest, HashChangesWithDifferentMessage) {
    Commit commit1 = createTestCommit(EMPTY_TREE_HASH, {}, "Message V1");
    Commit commit2 = createTestCommit(EMPTY_TREE_HASH, {}, "Message V2");

    EXPECT_NE(commit1.getHashId(), commit2.getHashId())
        << "Changing commit message must change the commit hash.";
}

TEST(CommitTest, SerializationDeserializationRoundTrip) {
    std::string complex_message = "Feature commit.\n\n- Line 2\n- Line 3";
    std::time_t custom_time = 1700000000;
    
    Commit original = createTestCommit(
        "new_tree_hash_12345", 
        {"p1_hash", "p2_hash"}, 
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
}