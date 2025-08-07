

# FreeType's CMake support is quite old, so cache variables are required to override options

set(DISABLE_FORCE_DEBUG_POSTFIX ON CACHE INTERNAL "")

# These HarfBuzz definitions are required so that Freetype finds the Navitab HB libraries.
# NB, hard-coded items will need updating if a new HarfBuzz package version is adopted.
set(HarfBuzz_INCLUDE_DIR "${NAVITAB_DOWNLOAD_PREFIX}/harfbuzz-11.2.1/src" CACHE INTERNAL "")
if(MSVC)
    set(HarfBuzz_LIBRARY "${PROJECT_BINARY_DIR}/download/harfbuzz/harfbuzz.lib" CACHE INTERNAL "")
else()
    set(HarfBuzz_LIBRARY "${PROJECT_BINARY_DIR}/download/harfbuzz/${XCODE_LIB_PATH_PREFIX}libharfbuzz.a" CACHE INTERNAL "")
endif()

add_subdirectory("${freetype_SOURCE_DIR}" "${freetype_BINARY_DIR}" EXCLUDE_FROM_ALL)

target_link_libraries(freetype PRIVATE png_static bzip2)

unset(DISABLE_FORCE_DEBUG_POSTFIX CACHE)
