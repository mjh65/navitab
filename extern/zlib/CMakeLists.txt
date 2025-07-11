
set(ZLIB_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_Declare(zlib
    URL "https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz"
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(ZLIB)

add_library(ZLIB::ZLIB ALIAS zlibstatic)
add_library(ZLIB::zlib ALIAS zlibstatic)

set(ZLIB_INCLUDE_DIRS "${zlib_SOURCE_DIR}" PARENT_SCOPE)
