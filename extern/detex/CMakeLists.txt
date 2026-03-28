# This file is part of the Navitab project. See the README and LICENSE for details.

# This file is a basic CMake wrapper for the detex external library, which does not
# provide CMake support natively.

project(detex
    VERSION 0.1.2
    DESCRIPTION "A library for manipulating texture blocks"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

# The detex library
set(detex_SOURCES
    bits.c
    bits.h
    bptc-tables.c
    bptc-tables.h
    clamp.c
    convert.c
    dds.c
    decompress-bc.c
    decompress-bptc-float.c
    decompress-bptc.c
    decompress-eac.c
    decompress-etc.c
    decompress-rgtc.c
    detex.h
    division-tables.c
    file-info.c
    file-info.h
    half-float.c
    half-float.h
    hdr.c
    hdr.h
    ktx.c
    misc.c
    misc.h
    raw.c
    texture.c
)
PrefixSources(detex_SOURCES "${NAVITAB_detex_SOURCE_DIR}")

add_library(detex STATIC ${detex_SOURCES})
target_include_directories(detex
    PUBLIC "${NAVITAB_detex_SOURCE_DIR}"
)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_detex_PKGLIBS detex CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_detex_LOCATION "${NAVITAB_LIBS3RD_DIR}/detex.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_detex_LOCATION "${NAVITAB_LIBS3RD_DIR}/libdetex.a" CACHE INTERNAL "")
endif()
set(NAVITAB_detex_INCDIRS "${NAVITAB_detex_SOURCE_DIR}" CACHE INTERNAL "")
