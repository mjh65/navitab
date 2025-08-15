# This file is part of the Navitab project. See the README and LICENSE for details.

add_subdirectory("${lerc_SOURCE_DIR}" "${lerc_BINARY_DIR}" EXCLUDE_FROM_ALL)
add_library(lerc ALIAS Lerc)

# This is to influence findPackage into detecting Navitab's version of Lerc
set(LERC_INCLUDE_DIR "${lerc_SOURCE_DIR}/src/LercLib/include" CACHE INTERNAL "")
if(MSVC)
    set(LERC_LIBRARY "${NAVITAB_IMPORT_LIBS}/lerc.lib" CACHE INTERNAL "")
else()
    set(LERC_LIBRARY "${NAVITAB_IMPORT_LIBS}/liblerc.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_lerc_PKGLIBS lerc CACHE INTERNAL "")
set(NAVITAB_lerc_LOCATION "${LERC_LIBRARY}" CACHE INTERNAL "")
