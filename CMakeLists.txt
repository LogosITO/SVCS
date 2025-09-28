cmake_minimum_required(VERSION 3.15)
project(SVCS VERSION 0.1 LANGUAGES CXX)

cmake_policy(SET CMP0167 NEW)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()


find_package(Boost 1.88.0 REQUIRED COMPONENTS system date_time)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_subdirectory(core)

add_subdirectory(tests)