# This file is part of the Navitab project. See the README and LICENSE for details.

# Console program for building navigation database from various sources

add_executable(navitab_builddb)

target_sources(navitab_builddb PRIVATE
    main.cpp
)

target_link_libraries(navitab_builddb
)

