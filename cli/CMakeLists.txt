set(CLI_SOURCES
    src/InitCommand.cxx
    src/AddCommand.cxx
    src/CommandFactory.cxx
    src/main.cxx
)

set(CLI_HEADERS
    include/BaseCommand.hxx
    include/ICommand.hxx
    include/InitCommand.hxx
    include/AddCommand.hxx
    include/CommandFactory.hxx
)
add_executable(svcs ${CLI_SOURCES})

set_target_properties(svcs_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "svcs"
)

target_include_directories(svcs PRIVATE include)
target_include_directories(svcs PRIVATE ../core/include)

target_link_libraries(svcs
    svcs_core
    svcs_services
)

install(TARGETS svcs DESTINATION bin)