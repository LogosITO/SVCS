set(SERVER_SOURCES
    src/ServerBaseCommand.cxx
    src/HubCommand.cxx
    src/RepoCommand.cxx
    src/ServerCommandFactory.cxx
    src/RemoteManager.cxx
    src/RemoteProtocol.cxx
    src/Server.cxx
    src/SSHConfig.cxx
    src/SSHSession.cxx
    src/SSHServer.cxx
)

set(SERVER_HEADERS
    include/ServerBaseCommand.hxx
    include/HubCommand.hxx
    include/RepoCommand.hxx
    include/ServerCommandFactory.hxx
    include/RemoteManager.hxx
    include/RemoteProtocol.hxx
    include/Server.hxx
    include/SSHConfig.hxx
    include/SSHSession.hxx
    include/SSHServer.hxx
)
add_library(svcs_server STATIC ${SERVER_SOURCES})

target_include_directories(svcs_server PRIVATE include)
target_include_directories(svcs_server PRIVATE ../core/include)

target_link_libraries(svcs_server PRIVATE svcs_core)

find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
    target_link_libraries(svcs_server PRIVATE Boost::system)
else()
    message(FATAL_ERROR "Boost not found!")
endif()

find_package(libssh REQUIRED)
if(libssh_FOUND)
    message(STATUS "Found libssh: ${libssh_INCLUDE_DIRS}")
    include_directories(${libssh_INCLUDE_DIRS})
    target_link_libraries(svcs_server PRIVATE ${libssh_LIBRARIES})
else()
    message(FATAL_ERROR "libssh not found! Install with: sudo apt-get install libssh-dev")
endif()