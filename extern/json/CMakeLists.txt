# This file is part of the Navitab project. See the README and LICENSE for details.

# NLohmann's JSON library

add_subdirectory("${NAVITAB_json_SOURCE_DIR}" "${NAVITAB_json_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_json_PKGLIBS nlohmann_json CACHE INTERNAL "")
set(NAVITAB_nlohmann_json_INCDIRS "${NAVITAB_json_SOURCE_DIR}/include" CACHE INTERNAL "")
