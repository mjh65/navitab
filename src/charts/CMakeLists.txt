# This file is part of the Navitab project. See the README and LICENSE for details.

if (NOT "$ENV{CHARTFOX_CLIENTID}" STREQUAL "")
    set(CHARTFOX_CLIENTID "$ENV{CHARTFOX_CLIENTID}" CACHE INTERNAL "Copied from environment variable")
endif()

target_sources(navitab_core PRIVATE charts.cpp)
