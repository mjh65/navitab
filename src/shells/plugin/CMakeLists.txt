# This file is part of the Navitab project. See the README and LICENSE for details.

if(XPLANE_SDK)
    message(STATUS "Adding X-Plane plugin to build")
    add_subdirectory(xplane)
endif()
if(MSFS_SDK)
    message(STATUS "Adding Flight Simulator panel to build")
    add_subdirectory(msfs)
endif()
