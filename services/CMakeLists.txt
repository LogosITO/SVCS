add_library(svcs_services STATIC
    logging/Logger.cxx    
)

target_include_directories(svcs_services PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)