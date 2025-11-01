cmake_minimum_required(VERSION 3.15)
project(SVCS VERSION 0.1 LANGUAGES CXX)

cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()


find_package(Boost 1.88.0 REQUIRED COMPONENTS system date_time)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_subdirectory(core)
add_subdirectory(cli)
add_subdirectory(server)
add_subdirectory(services)
add_subdirectory(integration)

target_include_directories(svcs_core PUBLIC
    ${CMAKE_SOURCE_DIR}/services
)

target_link_libraries(svcs_core PUBLIC
    svcs_services
)

add_custom_target(docs_en
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile.en
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating English documentation"
)

add_custom_target(docs_ru
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile.ru
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Russian documentation"
)

enable_testing()

add_subdirectory(tests)