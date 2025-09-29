set(CORE_SOURCES
    src/VcsObject.cxx
    src/Blob.cxx
    src/Tree.cxx
    src/Commit.cxx
    src/ObjectStorage.cxx
    src/Utils.cxx
    src/Index.cxx
    src/Repository.cxx
)

set(CORE_HEADERS
    include/VcsObject.hxx
    include/Blob.hxx
    include/Tree.hxx
    include/Commit.hxx
    include/ObjectStorage.hxx
    include/Utils.hxx
    include/Repository.hxx
)


add_library(svcs_core STATIC ${CORE_SOURCES})

set_target_properties(svcs_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(svcs_core PUBLIC include)

target_link_libraries(svcs_core PUBLIC 
    Boost::system
    Boost::date_time
    OpenSSL::Crypto
    ZLIB::ZLIB
)
