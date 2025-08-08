
set(PNG_STATIC ON)
set(PNG_SHARED OFF)
if(APPLE)
    set(PNG_FRAMEWORK OFF)
    # libpng doesn't support h/w optimization for universal binaries in one pass
    # turn it off until we figure out a fix or workaround, if ever.
    set(PNG_HARDWARE_OPTIMIZATIONS OFF)
    add_definitions(-DPNG_ARM_NEON_OPT=0)
endif()
set(PNG_TOOLS OFF)
set(PNG_TESTS OFF)
set(SKIP_INSTALL_EXPORT ON)
set(SKIP_INSTALL_CONFIG_FILE ON)
set(PNG_DEBUG_POSTFIX "")

add_subdirectory("${png_SOURCE_DIR}" "${png_BINARY_DIR}" EXCLUDE_FROM_ALL)

target_include_directories(png_static
    INTERFACE "$<BUILD_INTERFACE:${png_SOURCE_DIR}>"
    INTERFACE "$<BUILD_INTERFACE:${png_BINARY_DIR}>"
)

# Override the PNG libraries default build names to simplify the later settings
set_target_properties(png_static
    PROPERTIES OUTPUT_NAME "pngs"
    DEBUG_POSTFIX ""
)

# This is to influence findPackage into detecting Navitab's version of PNG
set(PNG_PNG_INCLUDE_DIR "${png_SOURCE_DIR}" CACHE INTERNAL "")
if(MSVC)
    set(PNG_LIBRARY "${png_BINARY_DIR}/pngs.lib" CACHE INTERNAL "")
else()
    set(PNG_LIBRARY "${png_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libpngs.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_png_PKGLIBS png_static CACHE INTERNAL "")
set(NAVITAB_png_static_LOCATION "${PNG_LIBRARY}" CACHE INTERNAL "")
