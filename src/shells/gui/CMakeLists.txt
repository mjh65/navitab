# This file is part of the Navitab project. See the README and LICENSE for details.

add_subdirectory(desktop)
if(MSFS_SDK)
    add_subdirectory(msfs)
endif()
