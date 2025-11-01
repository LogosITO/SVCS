set(SERVER_SOURCES
    src/ServerBaseCommand.cxx
    src/HubCommand.cxx
    src/RepoCommand.cxx
    src/RemoteManager.cxx
    src/RemoteProtocol.cxx
    src/Server.cxx
)

set(SERVER_HEADERS
    include/ServerBaseCommand.hxx
    include/HubCommand.hxx
    include/RepoCommand.hxx
    include/RemoteManager.hxx
    include/RemoteProtocol.hxx
    include/Server.hxx
)

find_package(Boost REQUIRED COMPONENTS system)

add_library(svcs_server STATIC ${SERVER_SOURCES})

target_include_directories(svcs_server PRIVATE include)
target_include_directories(svcs_server PRIVATE ../core/include)

target_link_libraries(svcs_server PRIVATE svcs_core)
target_link_libraries(svcs_server PRIVATE Boost::system)