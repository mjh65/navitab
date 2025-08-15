# This file is part of the Navitab project. See the README and LICENSE for details.

# Navitab custom CMake for the MuPDF library, since it doesn't provide one.

project(MuPDF
    VERSION 1.26.3
    DESCRIPTION "A library for rendering PDF and other formats"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

file(GLOB MUPDF_SOURCES
    "${mupdf_SOURCE_DIR}/source/fitz/*.c"
    "${mupdf_SOURCE_DIR}/source/html/*.c"
    "${mupdf_SOURCE_DIR}/source/pdf/*.c"
    "${mupdf_SOURCE_DIR}/source/svg/*.c"
    "${mupdf_SOURCE_DIR}/generated/resources/fonts/urw/*.cff.c"
    "${mupdf_SOURCE_DIR}/source/cbz/muimg.c"
)

add_library(mupdf STATIC ${MUPDF_SOURCES})
target_include_directories(mupdf
    PUBLIC "${mupdf_SOURCE_DIR}/include"
)
target_link_libraries(mupdf
    PRIVATE zlibs
    PRIVATE brotlidec
    PRIVATE brotlienc
    PRIVATE lcms2
    PRIVATE jbig2dec
    PRIVATE jpeg
    PRIVATE openjp2
    PRIVATE freetype
    PRIVATE harfbuzz
    PRIVATE gumbo
)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_mupdf_PKGLIBS mupdf CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_mupdf_LOCATION "${NAVITAB_IMPORT_LIBS}/mupdf.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_mupdf_LOCATION "${NAVITAB_IMPORT_LIBS}/libmupdf.a" CACHE INTERNAL "")
endif()
set(NAVITAB_mupdf_INCDIRS "${mupdf_SOURCE_DIR}/include" CACHE INTERNAL "")
set(NAVITAB_mupdf_LINKLIBS zlibs brotlidec brotlienc brotlicommon lcms2 jbig2dec jpeg openjp2 freetype harfbuzz gumbo CACHE INTERNAL "")
