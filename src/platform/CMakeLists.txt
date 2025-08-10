# This file is part of the Navitab project. See the README and LICENSE for details.

target_sources(navitab_core PRIVATE
    platform.cpp
    paths.cpp
)

if(WIN32)
elseif(APPLE)
elseif(UNIX)
endif()
