find_package(GTest REQUIRED)

set(CORE_TEST_SOURCES
    ObjectTest.cxx
    BlobTest.cxx
    TreeTest.cxx
    CommitTest.cxx
    StorageTest.cxx
    IndexTest.cxx
    RepositoryTest.cxx
    RepoManagerTest.cxx
    BranchManagerTest.cxx
)

add_executable(svcs_core_unit_tests ${CORE_TEST_SOURCES})

target_link_libraries(svcs_core_unit_tests PRIVATE
    svcs_core
    GTest::gtest
    GTest::gtest_main
)

set_target_properties(svcs_core_unit_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

include(CTest)

add_test(NAME CoreUnitTests COMMAND svcs_core_unit_tests)