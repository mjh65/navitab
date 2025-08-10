# This file is part of the Navitab project. See the README and LICENSE for details.

# The openjpeg downloaded library has CMake support, but the supported versions
# don't overlap with the minimum version Navitab requires. There may be quite a
# few warnings!

# Need to use cache variables due to policy CMP0077
set(BUILD_CODEC OFF CACHE INTERNAL "")

add_subdirectory("${openjpeg_SOURCE_DIR}" "${openjpeg_BINARY_DIR}" EXCLUDE_FROM_ALL)

target_include_directories(openjp2
    INTERFACE "$<BUILD_INTERFACE:${openjpeg_SOURCE_DIR}/src/lib/openjp2>"
    INTERFACE "$<BUILD_INTERFACE:${openjpeg_BINARY_DIR}/src/lib/openjp2>"
)

# Unset these after configuring openjpeg to avoid polluting other modules
unset(BUILD_CODEC CACHE)
unset(LIBRARY_OUTPUT_PATH CACHE)
unset(EXECUTABLE_OUTPUT_PATH CACHE)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_openjpeg_PKGLIBS openjp2 CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_openjp2_LOCATION "${openjpeg_BINARY_DIR}/src/lib/openjp2/openjp2.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_openjp2_LOCATION "${openjpeg_BINARY_DIR}/src/lib/openjp2/${XCODE_LIB_PATH_PREFIX}libopenjp2.a" CACHE INTERNAL "")
endif()
