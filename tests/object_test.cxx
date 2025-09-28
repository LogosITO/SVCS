#include "../core/include/object.hxx"
#include <gtest/gtest.h>

TEST(VscObjectHashTest, CreatingHash) {
    std::string data = "Hello, Github!";
    
    TestableObject blob("blob", data);
    TestableObject commit("commit", data);
    
    EXPECT_FALSE(blob.getHashId().empty());
    EXPECT_FALSE(commit.getHashId().empty());
}

TEST(VscObjectHashTest, HashIsUniqueByType) {
    std::string data = "Hello, Github!";
    
    TestableObject blob("blob", data);
    TestableObject commit("commit", data);

    EXPECT_NE(blob.getType(), commit.getType());
}

TEST(VcsObjectHashTest, HashIsConsistent) {
    std::string data = "The same data for consistency check.";
    std::string type = "tree";

    TestableObject obj1(type, data);
    TestableObject obj2(type, data);

    EXPECT_EQ(obj1.getHashId(), obj2.getHashId());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}