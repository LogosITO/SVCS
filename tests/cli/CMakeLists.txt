find_package(GTest REQUIRED)

set(CLI_TEST_SOURCES
    utils/InitCommandIntegrationTest.cxx
    utils/ClearCommandTest.cxx
    utils/IntAddCommandTest.cxx
    InitCommandTest.cxx
    AddCommandTest.cxx
    ClearCommandTest.cxx
)

add_executable(svcs_cli_unit_tests ${CLI_TEST_SOURCES})

target_include_directories(svcs_cli_unit_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(svcs_cli_unit_tests PRIVATE
    svcs_cli_commands
    svcs_core
    GTest::gtest
    GTest::gtest_main
)

set_target_properties(svcs_cli_unit_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

include(CTest)

add_test(NAME CLIUnitTests COMMAND svcs_cli_unit_tests)