# This file is part of the Navitab project. See the README and LICENSE for details.

set(HB_HAVE_FREETYPE ON)
set(HB_BUILD_SUBSET OFF)

add_subdirectory("${harfbuzz_SOURCE_DIR}" "${harfbuzz_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_harfbuzz_PKGLIBS harfbuzz CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_harfbuzz_LOCATION "${NAVITAB_LIBS3RD_DIR}/harfbuzz.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_harfbuzz_LOCATION "${NAVITAB_LIBS3RD_DIR}/libharfbuzz.a" CACHE INTERNAL "")
endif()
