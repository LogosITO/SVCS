find_package(GTest REQUIRED)

set(ALL_TEST_SOURCES
    object_test.cxx
    blob_test.cxx
    tree_test.cxx
    commit_test.cxx
    storage_test.cxx
    index_test.cxx
    repository_test.cxx
)

add_executable(svcs_unit_tests ${ALL_TEST_SOURCES})

target_link_libraries(svcs_unit_tests PRIVATE
    svcs_core
    GTest::gtest
)

set_target_properties(svcs_unit_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

include(CTest)

add_test(NAME AllUnitTests COMMAND svcs_unit_tests)