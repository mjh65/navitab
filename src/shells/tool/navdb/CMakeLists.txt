# This file is part of the Navitab project. See the README and LICENSE for details.

# Console program for building navigation database from various sources

add_executable(navitab_builddb)

target_sources(navitab_builddb PRIVATE
    main.cpp
    bglnavdata.cpp
    bglnavdata.h
    sceneryreader.cpp
    sceneryreader.h
    bglfilereader.cpp
    bglfilereader.h
    bglairport.cpp
	bglairport.h
	bglnamelist.cpp
	bglnamelist.h
	bglnavaid.cpp
	bglnavaid.h
	bglwaypoint.cpp
	bglwaypoint.h
)

if(WIN32)
    target_link_options(navitab_builddb PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
    )
endif()

target_link_libraries(navitab_builddb PRIVATE
    navitab_core
)
