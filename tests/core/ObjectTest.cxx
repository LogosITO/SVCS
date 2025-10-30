/**
 * @file ObjectTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Unit tests for the core hashing logic of the VcsObject abstract base class.
 *
 * @russian
 * @brief Модульные тесты для основной логики хэширования абстрактного базового класса VcsObject.
 */

#include "../../core/include/VcsObject.hxx"

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