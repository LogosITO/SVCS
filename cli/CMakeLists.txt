set(CLI_SOURCES
    src/HelpService.cxx
    src/HelpCommand.cxx
    src/VersionCommand.cxx
    src/InitCommand.cxx
    src/AddCommand.cxx
    src/SaveCommand.cxx
    src/StatusCommand.cxx
    src/CommandFactory.cxx
    src/main.cxx
)

set(CLI_HEADERS
    include/HelpService.hxx
    include/BaseCommand.hxx
    include/ICommand.hxx
    include/HelpCommand.hxx
    include/VersionCommand.hxx
    include/InitCommand.hxx
    include/AddCommand.hxx
    include/SaveCommand.hxx
    include/StatusCommand.hxx
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