# This file is part of the Navitab project. See the README and LICENSE for details.

# Navitab custom CMake script since Gumbo does not provide one.

# Project name and a few useful settings. Other commands can pick up the results
project(GUMBO
    VERSION 0.10.1
    DESCRIPTION "A library for html5 parsing"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

# The GUMBO library
set(GUMBO_SOURCES
    string_buffer.c
    util.c
    vector.c
    attribute.c
    char_ref.c
    utf8.c
    error.c
    tag.c
    parser.c
    tokenizer.c
    string_piece.c
)
PrefixSources(GUMBO_SOURCES "${gumbo_SOURCE_DIR}/src")

add_library(gumbo STATIC ${GUMBO_SOURCES})
target_include_directories(gumbo
    PUBLIC "${gumbo_SOURCE_DIR}/src"
)
if(MSVC)
    target_include_directories(gumbo PRIVATE "${gumbo_SOURCE_DIR}/visualc/include")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_gumbo_PKGLIBS gumbo CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_gumbo_LOCATION "${NAVITAB_IMPORT_LIBS}/gumbo.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_gumbo_LOCATION "${NAVITAB_IMPORT_LIBS}/libgumbo.a" CACHE INTERNAL "")
endif()
