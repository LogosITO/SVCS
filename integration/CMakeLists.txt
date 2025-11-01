add_library(svcs_integration
        src/CompositeCommandFactory.cxx
)

target_link_libraries(svcs_integration
        PRIVATE
        svcs_core
        svcs_cli_commands
        svcs_server
        svcs_services
)

target_include_directories(svcs_integration
        PUBLIC
        include
        PRIVATE
        src
)

target_include_directories(svcs_integration
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(svcs
        src/main.cxx
)

target_link_libraries(svcs
        PRIVATE
        svcs_integration
        svcs_core
        svcs_cli_commands
        svcs_server
        svcs_services
)

if(UNIX)
    install(TARGETS svcs DESTINATION bin)
endif()

if(WIN32)
    install(TARGETS svcs DESTINATION .)
endif()