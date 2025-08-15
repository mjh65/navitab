# This file is part of the Navitab project. See the README and LICENSE for details.

# Formatting library, adds fmt::fmt to C++ namespace

set(FMT_INSTALL OFF)

add_subdirectory("${fmt_SOURCE_DIR}" "${fmt_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_fmt_PKGLIBS fmt CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_fmt_LOCATION "${NAVITAB_IMPORT_LIBS}/fmt.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_fmt_LOCATION "${NAVITAB_IMPORT_LIBS}/libfmt.a" CACHE INTERNAL "")
endif()
set(NAVITAB_fmt_INCDIRS "${fmt_SOURCE_DIR}/include" CACHE INTERNAL "")
