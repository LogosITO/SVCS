find_package(GTest REQUIRED)

set(SERVER_TEST_SOURCES
    RemoteProtocolTest.cxx
    HubCommandTest.cxx
)

add_executable(svcs_server_unit_tests ${SERVER_TEST_SOURCES})

target_link_libraries(svcs_server_unit_tests PRIVATE
    svcs_server
    GTest::gtest
    GTest::gtest_main
)

set_target_properties(svcs_server_unit_tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

include(CTest)

add_test(NAME CoreUnitTests COMMAND svcs_server_unit_tests)