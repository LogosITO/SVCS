set(CORE_SOURCES
    src/Commit.cxx
    src/VcsObject.cxx
    src/Blob.cxx
)

set(CORE_HEADERS
    include/Commit.hxx
    include/VcsObject.hxx
    include/Blob.hxx
)


add_library(svcs_core STATIC ${CORE_SOURCES})

set_target_properties(svcs_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(svcs_core PUBLIC include)

target_link_libraries(svcs_core PRIVATE 
    Boost::system
    Boost::date_time
    OpenSSL::Crypto
)