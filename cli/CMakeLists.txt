set(CLI_SOURCES
    src/InitCommand.cxx
    src/CommandFactory.cxx
    src/main.cxx
)

set(CLI_HEADERS
    include/BaseCommand.hxx
    include/ICommand.hxx
    include/InitCommand.hxx
    include/CommandFactory.hxx
)
add_executable(svcs_cli ${CLI_SOURCES})

set_target_properties(svcs_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "svcs"
)

target_include_directories(svcs_cli PRIVATE include)
target_include_directories(svcs_cli PRIVATE ../core/include)

target_link_libraries(svcs_cli 
    svcs_core
    svcs_services
)

install(TARGETS svcs_cli DESTINATION bin)