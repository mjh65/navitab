# This file is part of the Navitab project. See the README and LICENSE for details.

target_sources(navitab_core PRIVATE
    navitab.cpp
    navitab.h
    logger.cpp
    logmanager.cpp
    logmanager.h
    settingsmgr.cpp
    settingsmgr.h
    appcanvas.cpp
    appcanvas.h
    coretoolbar.cpp
    coretoolbar.h
    coremodebar.cpp
    coremodebar.h
    coredoodler.cpp
    coredoodler.h
    corekeypad.cpp
    corekeypad.h
    geometrics.cpp
)

add_dependencies(navitab_core navitab_svg_headers)
