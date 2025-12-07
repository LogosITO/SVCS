/**
 * @file BlobTest.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Unit tests for the Blob class, verifying object construction, hash calculation, and data integrity.
 *
 * @russian
 * @brief Модульные тесты для класса Blob, проверяющие построение объектов, вычисление хэшей и целостность данных.
 */

#include "../../core/include/Blob.hxx"

#include <gtest/gtest.h>
/**
 * @brief Comprehensive testing framework for SVCS version control system
 * @details Organized testing infrastructure covering all aspects of SVCS
 * from core functionality to user interface and network services.
 *
 * @english
 * Comprehensive testing framework for SVCS version control system
 * @details Provides structured testing approach with clear separation of
 * concerns between different system layers. Supports test-driven development,
 * continuous integration, and quality assurance processes. Includes utilities
 * for mocking, test data generation, and performance benchmarking.
 *
 * @russian
 * Комплексная тестовая среда для системы контроля версий SVCS
 * @details Предоставляет структурированный подход к тестированию с четким
 * разделением ответственности между различными слоями системы. Поддерживает
 * разработку через тестирование (TDD), непрерывную интеграцию и процессы
 * обеспечения качества. Включает утилиты для мокинга, генерации тестовых
 * данных и бенчмаркинга производительности.
 */

namespace svcs::test {}

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

}