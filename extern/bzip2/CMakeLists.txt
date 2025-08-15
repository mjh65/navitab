# This file is part of the Navitab project. See the README and LICENSE for details.

# BZip2 does not provide a CMake script, so this is a basic wrapper for Navitab's needs.

# Project name and a few useful settings. Other commands can pick up the results
project(BZIP2
    VERSION 1.0.8
    DESCRIPTION "A library for compression"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

# The BZIP2 library
set(BZIP2_SOURCES blocksort.c bzlib.c compress.c crctable.c decompress.c huffman.c randtable.c)
PrefixSources(BZIP2_SOURCES "${bzip2_SOURCE_DIR}")

add_library(bzip2 STATIC ${BZIP2_SOURCES})
target_include_directories(bzip2
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}"
)

# This is to influence findPackage into detecting Navitab's version of BZip2
set(BZIP2_INCLUDE_DIR "${bzip2_SOURCE_DIR}" CACHE INTERNAL "")
if(MSVC)
    set(BZIP2_LIBRARIES "${NAVITAB_IMPORT_LIBS}/bzip2.lib" CACHE INTERNAL "")
else()
    set(BZIP2_LIBRARIES "${NAVITAB_IMPORT_LIBS}/libbzip2.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_bzip2_PKGLIBS bzip2 CACHE INTERNAL "")
set(NAVITAB_bzip2_LOCATION "${BZIP2_LIBRARIES}" CACHE INTERNAL "")
