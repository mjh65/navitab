

# Navitab uses Zstd's CMake scripts.

set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_LEGACY_SUPPORT OFF)
set(ZSTD_BUILD_SHARED OFF)
set(ZSTD_BUILD_COMPRESSION OFF)
set(ZSTD_BUILD_DICTBUILDER OFF)
set(ZSTD_USE_STATIC_RUNTIME ON)

add_subdirectory("${zstd_SOURCE_DIR}/build/cmake" "${zstd_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Ensure that find_package selects Navitab's downloaded zstd static library 
# CMake target is called 'libzstd'
set(ZSTD_INCLUDE_DIR "${zstd_SOURCE_DIR}/lib" CACHE INTERNAL "")
if(MSVC)
    set(ZSTD_LIBRARY "${zstd_BINARY_DIR}/lib/zstd_static.lib" CACHE INTERNAL "")
else()
    set(ZSTD_LIBRARY "${zstd_BINARY_DIR}/lib/${XCODE_LIB_PATH_PREFIX}libzstd.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_zstd_PKGLIBS libzstd CACHE INTERNAL "")
set(NAVITAB_libzstd_LOCATION "${ZSTD_LIBRARY}" CACHE INTERNAL "")
