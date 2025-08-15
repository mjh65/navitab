# This file is part of the Navitab project. See the README and LICENSE for details.

# Navitab uses ZLib's CMake scripts.

set(ZLIB_BUILD_EXAMPLES OFF)

add_subdirectory("${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}" EXCLUDE_FROM_ALL)
add_library(ZLIB::ZLIB ALIAS zlibstatic)
add_library(zlibs ALIAS zlibstatic)

# These settings influence findPackage into detecting Navitab's version of ZLib.
# The name zlibs also avoids a clash in VS's generator.
set(ZLIB_USE_STATIC_LIBS ON CACHE INTERNAL "")
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}" CACHE INTERNAL "")
if(MSVC)
    set(ZLIB_LIBRARY "${NAVITAB_IMPORT_LIBS}/zlibs.lib" CACHE INTERNAL "")
else()
    set(ZLIB_LIBRARY "${NAVITAB_IMPORT_LIBS}/libzlibs.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_zlib_PKGLIBS zlibs CACHE INTERNAL "")
set(NAVITAB_zlibs_LOCATION "${ZLIB_LIBRARY}" CACHE INTERNAL "")
