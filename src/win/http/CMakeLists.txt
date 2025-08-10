# This file is part of the Navitab project. See the README and LICENSE for details.

# Any [abstract] interfaces that are used by the different executables
# are found in NAVITAB_API_DIR. All other headers that are internal to
# the Navitab core system are referenced using relative paths from the
# including file.

target_sources(navitab_win_http PRIVATE
    winhttp.cpp
    winhttp.h
    htmlserver.cpp
    htmlserver.h
    httpreq.cpp
    httpreq.h
    cmdhandler.cpp
    cmdhandler.h
)

if(WIN32)
    target_link_libraries(navitab_win_http PUBLIC ws2_32)
endif()
