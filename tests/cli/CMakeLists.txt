find_package(GTest REQUIRED)

set(CLI_TEST_SOURCES
    utils/InitCommandIntegrationTest.cxx
    utils/IntVersionCommandTest.cxx
    utils/IntHelpCommandTest.cxx
    utils/ClearCommandTest.cxx
    utils/IntAddCommandTest.cxx
    utils/IntRemoveCommandTest.cxx
    utils/IntSaveCommandTest.cxx
    utils/IntStatusCommandTest.cxx
    utils/IntHistoryCommandTest.cxx
    InitCommandTest.cxx
    VersionCommandTest.cxx
    HelpCommandTest.cxx
    AddCommandTest.cxx
    ClearCommandTest.cxx
    RemoveCommandTest.cxx
    SaveCommandTest.cxx
    StatusCommandTest.cxx
    HistoryCommandTest.cxx
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