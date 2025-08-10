# This file is part of the Navitab project. See the README and LICENSE for details.

add_subdirectory(svgc)
add_subdirectory(navdb)
add_subdirectory(web)
if(MSFS_SDK)
    message(STATUS "Adding Flight Simulator panel-server to build")
    add_subdirectory(msfs)
endif()
