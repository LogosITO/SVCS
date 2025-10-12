set(SERVER_SOURCES
    src/HubCommand.cxx
)

set(SERVER_HEADERS
    include/HubCommand.hxx
)

add_library(svcs_server STATIC ${SERVER_SOURCES})

target_include_directories(svcs PRIVATE include)
target_include_directories(svcs PRIVATE ../core/include)