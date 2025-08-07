

# Navitab uses ZLib's CMake scripts.

set(ZLIB_BUILD_EXAMPLES OFF)

add_subdirectory("${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}" EXCLUDE_FROM_ALL)

# These settings influence findPackage into detecting Navitab's version of ZLib.
# The name zlibs also avoids a clash in VS's generator.
set(ZLIB_USE_STATIC_LIBS ON CACHE INTERNAL "")
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}" CACHE INTERNAL "")
if(MSVC)
    set(ZLIB_LIBRARY "${zlib_BINARY_DIR}/zlibs.lib" CACHE INTERNAL "")
else()
    set(ZLIB_LIBRARY "${zlib_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libzlibs.a" CACHE INTERNAL "")
endif()
add_library(ZLIB::ZLIB ALIAS zlibstatic)

# The ZLib CMake script creates differently named (and located) lib files
# depending on the platform and build configuration. Rewrite the target
# properties here to align with the (uniformly located) cache variables,
set_target_properties(zlibstatic PROPERTIES OUTPUT_NAME zlibs)
set_target_properties(zlibstatic PROPERTIES OUTPUT_DIRECTORY "${zlib_BINARY_DIR}")
