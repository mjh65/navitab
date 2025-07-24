

# Navitab uses Zstd's CMake scripts.

set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_LEGACY_SUPPORT OFF)
set(BUILD_TESTING OFF)
set(ZSTD_USE_STATIC_RUNTIME ON)

add_subdirectory("${zstd_SOURCE_DIR}/build/cmake" "${zstd_BINARY_DIR}" EXCLUDE_FROM_ALL)

# this aims to help find_package select this zstd library 
if(MSVC)
    set(ZSTD_LIBRARY "${zstd_BINARY_DIR}/zstd.lib" CACHE INTERNAL "")
else()
    set(ZSTD_LIBRARY "${zstd_BINARY_DIR}/libzstd.a" CACHE INTERNAL "")
endif()
set(ZSTD_INCLUDE_DIR "${zstd_SOURCE_DIR}/lib" CACHE INTERNAL "")

