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

/**
 * @english
 * @namespace svcs::test::core
 * @brief Unit tests for core components and data structures.
 * @details Contains test cases for core VCS objects, repository management,
 * and fundamental data structures. Tests in this namespace validate
 * the basic functionality and integrity of the system core.
 *
 * @russian
 * @namespace svcs::test::core
 * @brief Модульные тесты для основных компонентов и структур данных.
 * @details Содержит тест-кейсы для основных объектов СКВ, управления репозиториями
 * и фундаментальных структур данных. Тесты в этом пространстве имен проверяют
 * базовую функциональность и целостность ядра системы.
 */
namespace svcs::test::core {

using namespace svcs::core;

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

}