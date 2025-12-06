add_library(svcs_platform STATIC src/NetworkUtils.cxx)

target_include_directories(svcs_platform PUBLIC include)

if(WIN32)
    target_compile_definitions(svcs_platform PRIVATE _WIN32_WINNT=0x0601)
    target_link_libraries(svcs_platform PUBLIC ws2_32 mswsock)
endif()

set_target_properties(svcs_platform PROPERTIES
        PUBLIC_HEADER "include/NetworkUtils.hxx"
        VERSION 1.0.0
        SOVERSION 1
)