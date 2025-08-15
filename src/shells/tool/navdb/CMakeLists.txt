# This file is part of the Navitab project. See the README and LICENSE for details.

# Console program for building navigation database from various sources

add_executable(navitab_builddb)

target_sources(navitab_builddb PRIVATE
    main.cpp
)

if(WIN32)
    target_link_options(navitab_builddb PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
    )
endif()

target_link_libraries(navitab_builddb PRIVATE
)

