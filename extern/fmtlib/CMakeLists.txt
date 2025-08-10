# This file is part of the Navitab project. See the README and LICENSE for details.

# Formatting library, adds fmt::fmt to C++ namespace

set(FMT_DEBUG_POSTFIX "" CACHE INTERNAL "")
set(FMT_INSTALL OFF)

add_subdirectory("${fmtlib_SOURCE_DIR}" "${fmtlib_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_fmtlib_PKGLIBS fmt CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_fmt_LOCATION "${fmtlib_BINARY_DIR}/fmt.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_fmt_LOCATION "${fmtlib_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libfmt.a" CACHE INTERNAL "")
endif()
set(NAVITAB_fmt_INCDIRS "${fmtlib_SOURCE_DIR}/include" CACHE INTERNAL "")
