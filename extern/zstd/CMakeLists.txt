# This file is part of the Navitab project. See the README and LICENSE for details.

# Navitab uses Zstd's CMake scripts.

set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_LEGACY_SUPPORT OFF)
set(ZSTD_BUILD_SHARED OFF)
set(ZSTD_BUILD_COMPRESSION OFF)
set(ZSTD_BUILD_DICTBUILDER OFF)
set(ZSTD_USE_STATIC_RUNTIME ON)

add_subdirectory("${zstd_SOURCE_DIR}/build/cmake" "${zstd_BINARY_DIR}" EXCLUDE_FROM_ALL)
add_library(zstd ALIAS libzstd_static)

# Ensure that find_package selects Navitab's downloaded zstd static library 
# CMake target is called 'libzstd'
set(ZSTD_INCLUDE_DIR "${zstd_SOURCE_DIR}/lib" CACHE INTERNAL "")
if(MSVC)
    set(ZSTD_LIBRARY "${NAVITAB_LIBS3RD_DIR}/zstd.lib" CACHE INTERNAL "")
else()
    set(ZSTD_LIBRARY "${NAVITAB_LIBS3RD_DIR}/libzstd.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_zstd_PKGLIBS zstd CACHE INTERNAL "")
set(NAVITAB_zstd_LOCATION "${ZSTD_LIBRARY}" CACHE INTERNAL "")
