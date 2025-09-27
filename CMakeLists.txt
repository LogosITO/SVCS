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

set(CORE_SOURCES
    src/commit.cxx
)

set(CORE_HEADERS
    include/Commit.hxx
)

add_library(svcs_core STATIC ${CORE_SOURCES})

target_include_directories(svcs_core PUBLIC include)

target_link_libraries(svcs_core PRIVATE 
    Boost::system
    Boost::date_time
    OpenSSL::Crypto
)

set_target_properties(svcs_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

set(MAIN_SOURCES
    src/main.cxx
)

add_executable(SVCS ${MAIN_SOURCES})

target_link_libraries(SVCS PRIVATE 
    svcs_core
    OpenSSL::SSL
)

set_target_properties(SVCS PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_definitions(SVCS PRIVATE 
    BOOST_ALL_NO_LIB=1
)

find_package(Boost 1.88.0 REQUIRED COMPONENTS system date_time)
find_package(OpenSSL REQUIRED)

add_subdirectory(tests)