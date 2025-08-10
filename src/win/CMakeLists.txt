# This file is part of the Navitab project. See the README and LICENSE for details.

target_sources(navitab_core PRIVATE
    texbuffer.cpp
)

add_subdirectory(glfw)
add_subdirectory(http)
if(XPLANE_SDK)
    add_subdirectory(xplane)
endif()
