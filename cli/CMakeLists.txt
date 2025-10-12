set(CLI_SOURCES
    src/HelpService.cxx
    src/HelpCommand.cxx
    src/VersionCommand.cxx
    src/InitCommand.cxx
    src/ClearCommand.cxx
    src/AddCommand.cxx
    src/RemoveCommand.cxx
    src/SaveCommand.cxx
    src/StatusCommand.cxx
    src/HistoryCommand.cxx
    src/UndoCommand.cxx
    src/BranchCommand.cxx
    src/MergeCommand.cxx
    src/CommandFactory.cxx
)

set(CLI_HEADERS
    include/Utils.hxx
    include/HelpService.hxx
    include/BaseCommand.hxx
    include/ICommand.hxx
    include/HelpCommand.hxx
    include/VersionCommand.hxx
    include/InitCommand.hxx
    include/ClearCommand.hxx
    include/AddCommand.hxx
    include/RemoveCommand.hxx
    include/SaveCommand.hxx
    include/StatusCommand.hxx
    include/HistoryCommand.hxx
    include/UndoCommand.hxx
    include/BranchCommand.hxx
    include/MergeCommand.hxx
    include/CommandFactory.hxx
)

add_library(svcs_cli_commands STATIC ${CLI_SOURCES})
add_executable(svcs src/main.cxx ${CLI_SOURCES})

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
    svcs_server
)

if(UNIX)
    install(TARGETS svcs DESTINATION bin)
endif()
  
if(WIN32)
    install(TARGETS svcs DESTINATION .)
endif()