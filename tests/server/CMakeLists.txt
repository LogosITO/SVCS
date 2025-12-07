find_package(GTest REQUIRED)

if(NOT LIBSSH_FOUND)
    find_package(LibSSH QUIET)
    if(LibSSH_FOUND)
        set(LIBSSH_INCLUDE_DIRS ${LibSSH_INCLUDE_DIRS})
        set(LIBSSH_LIBRARIES ${LibSSH_LIBRARIES})
    else()
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(LIBSSH QUIET libssh)
        endif()
    endif()
endif()

if(NOT LIBSSH_FOUND)
    find_path(LIBSSH_INCLUDE_DIR libssh/libssh.h
            PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
    )

    find_library(LIBSSH_LIBRARY
            NAMES ssh libssh
            PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
    )

    if(LIBSSH_INCLUDE_DIR AND LIBSSH_LIBRARY)
        set(LIBSSH_FOUND TRUE)
        set(LIBSSH_INCLUDE_DIRS ${LIBSSH_INCLUDE_DIR})
        set(LIBSSH_LIBRARIES ${LIBSSH_LIBRARY})
        message(STATUS "Found libssh manually: ${LIBSSH_LIBRARY}")
    endif()
endif()

if(NOT LIBSSH_FOUND)
    message(WARNING "libssh not found. SSH tests may not compile.")
endif()

set(SERVER_TEST_SOURCES
        RemoteProtocolTest.cxx
        HubCommandTest.cxx
        RepoCommandTest.cxx
        RemoteManagerTest.cxx
        ServerTest.cxx
        SSHConfigTest.cxx
        SSHSessionTest.cxx
)

add_executable(svcs_server_unit_tests ${SERVER_TEST_SOURCES})

target_include_directories(svcs_server_unit_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../server/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../services/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../cli/include
        ${LIBSSH_INCLUDE_DIRS}
)

target_link_libraries(svcs_server_unit_tests PRIVATE
        svcs_server
        svcs_platform
        svcs_services
        svcs_core
        svcs_cli_commands
        ${LIBSSH_LIBRARIES}
        GTest::gtest
        GTest::gtest_main
)

if(NOT LIBSSH_FOUND)
    target_compile_definitions(svcs_server_unit_tests PRIVATE
            NO_LIBSSH=1
    )
endif()

set_target_properties(svcs_server_unit_tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

include(CTest)
add_test(NAME ServerUnitTests COMMAND svcs_server_unit_tests)