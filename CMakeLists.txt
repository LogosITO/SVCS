cmake_minimum_required(VERSION 3.15)

project(
    SVCS
    VERSION 0.1
    LANGUAGES CXX
)

cmake_policy(SET CMP0167 NEW)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

set(SOURCES
    src/main.cxx
    src/commit.cxx
)

set(HEADERS
    include/header.hxx
)

find_package(Boost 1.88.0 REQUIRED COMPONENTS system date_time)

find_package(OpenSSL REQUIRED)

add_executable(SVCS ${SOURCES})

target_include_directories(SVCS PRIVATE include)

target_link_libraries(SVCS PRIVATE 
    Boost::system
    Boost::date_time
    OpenSSL::SSL
    OpenSSL::Crypto
)

set_target_properties(SVCS PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_definitions(SVCS PRIVATE 
    BOOST_ALL_NO_LIB=1
)

set_target_properties(SVCS PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)