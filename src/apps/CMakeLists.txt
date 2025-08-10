# This file is part of the Navitab project. See the README and LICENSE for details.

target_sources(navitab_core PRIVATE
    app.cpp
    app.h
)

add_subdirectory(about)
add_subdirectory(map)
add_subdirectory(airport)
add_subdirectory(route)
add_subdirectory(reader)
add_subdirectory(settings)
