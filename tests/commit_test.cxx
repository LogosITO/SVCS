#include "../core/include/Commit.hxx"
#include <gtest/gtest.h>

TEST(AuthorValidationTest, ValidEmailIsAccepted) {
    Author a("Jane Doe", "jane.doe@company.org");
    EXPECT_EQ(a.get_email(), "jane.doe@company.org");
    EXPECT_EQ(a.get_name(), "Jane Doe"); 
}

TEST(AuthorValidationTest, InvalidEmailIsNotAccepted1) {
    Author a("Bad Sender", "anonym@ann.com@gmail.com");
    EXPECT_NE(a.get_email(), "anonym@gmail.com");
}

TEST(AuthorValidationTest, InvalidEmailIsNotAccepted2) {
    Author a("Bad Sender", "@gmail.com");
    EXPECT_NE(a.get_email(), "anonym@gmail.com");
}

TEST(AuthorValidationTest, InvalidEmailIsNotAccepted3) {
    Author a("Bad Sender", "lol");
    EXPECT_NE(a.get_email(), "anonym@gmail.com");
}

TEST(HashCalculatingTest, HCT1) {
    std::string content = "123123jakdjlasdj123123123123";
    Commit c;
    c.computeHash(content);
    std::string res1 = c.getHashId();
    c.computeHash(content);
    std::string res2 = c.getHashId();
    EXPECT_EQ(res1, res2);
}

TEST(HashCalculatingTest, HCT2) {
    std::string content = "i love my mom!";
    Commit c, b;
    c.computeHash(content);
    std::string res1 = c.getHashId();
    b.computeHash(content);
    std::string res2 = c.getHashId();
    EXPECT_EQ(res1, res2);
}

TEST(HashCalculatingTest, HCT3) {
    std::string content1 = "123123jakdjlasdj123123123123";
    std::string content2 = "123123jakdjlasdj123123123122";
    Commit c;
    c.computeHash(content1);
    std::string res1 = c.getHashId();
    c.computeHash(content2);
    std::string res2 = c.getHashId();
    EXPECT_NE(res1, res2);
}

TEST(HashCalculatingTest, HCT4) {
    std::string content1 = "i love my mom!";
    std::string content2 = "i love my mom.";
    Commit c, b;
    c.computeHash(content1);
    std::string res1 = c.getHashId();
    b.computeHash(content2);
    std::string res2 = c.getHashId();
    EXPECT_NE(res1, res2);
}

int main(int argc, char *argv[]){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}