# This file is part of the Navitab project. See the README and LICENSE for details.

include_directories("${NAVITAB_API_DIR}")

if(XPLANE_SDK)
    add_subdirectory(xplane)
endif()
if(MSFS_SDK)
    add_subdirectory(msfs)
endif()
add_subdirectory(mock)
