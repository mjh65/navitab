# This file is part of the Navitab project. See the README and LICENSE for details.

# FreeType's CMake support is quite old, so cache variables are required to override options

# These HarfBuzz definitions are required so that Freetype finds the Navitab HB libraries.
# NB, hard-coded items will need updating if a new HarfBuzz package version is adopted.
set(HarfBuzz_INCLUDE_DIR "${NAVITAB_DOWNLOAD_DIR}/harfbuzz-11.2.1/src" CACHE INTERNAL "")
if(MSVC)
    set(HarfBuzz_LIBRARY "${NAVITAB_LIBS3RD_DIR}/harfbuzz.lib" CACHE INTERNAL "")
else()
    set(HarfBuzz_LIBRARY "${NAVITAB_LIBS3RD_DIR}/libharfbuzz.a" CACHE INTERNAL "")
endif()

add_subdirectory("${freetype_SOURCE_DIR}" "${freetype_BINARY_DIR}" EXCLUDE_FROM_ALL)

target_link_libraries(freetype PRIVATE png bzip2)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_freetype_PKGLIBS freetype CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_freetype_LOCATION "${NAVITAB_LIBS3RD_DIR}/freetype.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_freetype_LOCATION "${NAVITAB_LIBS3RD_DIR}/libfreetype.a" CACHE INTERNAL "")
endif()
set(NAVITAB_freetype_LINKLIBS png bzip2 CACHE INTERNAL "")
