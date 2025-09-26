cmake_minimum_required(VERSION 3.15)

project(
    SVCS
    VERSION 0.1
    LANGUAGES CXX
)

cmake_policy(SET CMP0167 NEW)

file(GLOB SOURCES "src/*.cxx")
file(GLOB HEADERS "include/*.hxx")

find_package(Boost 1.88.0 REQUIRED COMPONENTS system date_time)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(SVCS ${SOURCES})
include_directories(SVCS ${HEADERS})

target_link_libraries(SVCS PRIVATE ${Boost_LIBRARIES})

