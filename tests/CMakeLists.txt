find_package(GTest REQUIRED)

add_executable(commit_tests commit_test.cxx)
add_executable(object_tests object_test.cxx)
add_executable(blob_tests blob_test.cxx)
add_executable(tree_tests tree_test.cxx)

target_link_libraries(commit_tests PRIVATE
    GTest::gtest_main
    svcs_core
)

target_link_libraries(object_tests PRIVATE 
    GTest::gtest_main
    svcs_core
)

target_link_libraries(blob_tests PRIVATE
    GTest::gtest_main
    svcs_core
)

target_link_libraries(tree_tests PRIVATE
    GTest::gtest_main
    svcs_core
)

include(CTest)

add_test(NAME CommitTests COMMAND commit_tests)
add_test(NAME ObjectTests COMMAND object_tests)
add_test(NAME BlobTests COMMAND blob_tests)
add_test(NAME TreeTests COMMAND tree_tets)