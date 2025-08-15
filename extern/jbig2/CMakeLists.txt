# This file is part of the Navitab project. See the README and LICENSE for details.

# This file is a basic CMake wrapper for the JBIG2 external library, which does not
# provide CMake support natively.

project(JBIG2
    VERSION 0.20.0
    DESCRIPTION "A library for decoding JBIG2 compressed images"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

# The JBIG2 library
set(JBIG2_SOURCES
    jbig2_arith_iaid.c
    jbig2_arith_int.c
    jbig2_arith.c
    jbig2_generic.c
    jbig2_halftone.c
    jbig2_huffman.c
    jbig2_hufftab.c
    jbig2_image_pbm.c
    jbig2_image_png.c
    jbig2_image.c
    jbig2_mmr.c
    jbig2_page.c
    jbig2_refinement.c
    jbig2_segment.c
    jbig2_symbol_dict.c
    jbig2_text.c
    jbig2.c
)
PrefixSources(JBIG2_SOURCES "${jbig2_SOURCE_DIR}")

add_library(jbig2dec STATIC ${JBIG2_SOURCES})
target_include_directories(jbig2dec
    PUBLIC "${jbig2_SOURCE_DIR}"
)
target_link_libraries(jbig2dec
    PRIVATE png_static
)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_jbig2_PKGLIBS jbig2dec CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_jbig2dec_LOCATION "${NAVITAB_IMPORT_LIBS}/jbig2dec.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_jbig2dec_LOCATION "${NAVITAB_IMPORT_LIBS}/libjbig2dec.a" CACHE INTERNAL "")
endif()
