set(SERVER_SOURCES
    src/HubCommand.cxx
    src/RemoteProtocol.cxx
)

set(SERVER_HEADERS
    include/HubCommand.hxx
    include/RemoteProtocol.hxx
)

add_library(svcs_server STATIC ${SERVER_SOURCES})

target_include_directories(svcs_server PRIVATE include)
target_include_directories(svcs_server PRIVATE ../core/include)

target_link_libraries(svcs_server PRIVATE svcs_core)