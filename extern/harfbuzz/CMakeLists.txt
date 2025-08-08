
set(HB_HAVE_FREETYPE ON)
set(HB_BUILD_SUBSET OFF)

add_subdirectory("${harfbuzz_SOURCE_DIR}" "${harfbuzz_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_harfbuzz_PKGLIBS harfbuzz CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_harfbuzz_LOCATION "${harfbuzz_BINARY_DIR}/harfbuzz.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_harfbuzz_LOCATION "${harfbuzz_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libharfbuzz.a" CACHE INTERNAL "")
endif()
