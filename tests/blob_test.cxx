/**
 * @file blob_test.cxx
 * @brief Unit tests for the Blob class, verifying object construction, hash calculation, and data integrity.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../core/include/Blob.hxx"

#include <gtest/gtest.h>

TEST(BlobTest, HashIsCalculatedOnCreation) {
    std::string content = "This is the content of file A.";
    Blob blob(content);

    EXPECT_FALSE(blob.getHashId().empty());

    EXPECT_EQ(blob.getData(), content);
    EXPECT_EQ(blob.getType(), "blob") << "Blob type must be 'blob'.";
}

TEST(BlobTest, HashIsConsistent) {
    std::string content = "The data must produce an identical hash.";

    Blob blob1(content);
    Blob blob2(content);

    EXPECT_EQ(blob1.getHashId(), blob2.getHashId());
}

TEST(BlobTest, HashChangesWithContentModification) {
    std::string content1 = "Version 1.0";
    std::string content2 = "Version 1.1";

    Blob blob1(content1);
    Blob blob2(content2);

    EXPECT_NE(blob1.getHashId(), blob2.getHashId());
}

TEST(BlobTest, HandlingEmptyContent) {
    Blob empty_blob("");
    EXPECT_FALSE(empty_blob.getHashId().empty()) << "Hash should be calculated even for empty content.";

    Blob non_empty_blob("A");
    EXPECT_NE(empty_blob.getHashId(), non_empty_blob.getHashId()) << "Empty content hash must be unique";
}