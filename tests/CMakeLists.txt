find_package(GTest REQUIRED)

add_executable(commit_tests commit_test.cxx)

target_link_libraries(commit_tests PRIVATE
    GTest::gtest_main
    svcs_core
)

include(CTest)
add_test(NAME CommitTests COMMAND commit_tests)
