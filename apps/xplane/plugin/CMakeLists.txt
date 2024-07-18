#   Navitab - Navigation Tablet for VR flight simulation
#   Copyright (c) 2024 Michael Hasling
#   Significantly derived from Avitab
#   Copyright (c) 2018-2024 Folke Will
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.

# The Navitab plugin for the X-Plane simulator

add_library(navitab_plugin SHARED)

target_sources(navitab_plugin PRIVATE
    plugin.cpp
)

if(WIN32)
    target_compile_definitions(navitab_plugin PRIVATE IBM=1)
    target_link_options(navitab_plugin PRIVATE)
    target_link_libraries(navitab_plugin
        navitab_core
        navitab_platform
        navitab_simxp
        "${XPLANE_SDK}/Libraries/Win/XPLM_64.lib"
        opengl32
    )
    install(TARGETS navitab_plugin RUNTIME DESTINATION navitab/xplane/Navitab/win_x64)
elseif(APPLE)
    target_compile_definitions(navitab_plugin PRIVATE APL=1)
    target_link_options(navitab_plugin PRIVATE
        "-F${XPLANE_SDK}/Libraries/Mac"
        -framework XPLM
        #-framework OpenGL
    )
    target_link_libraries(navitab_plugin
        navitab_core
        navitab_platform
        navitab_simxp
    )
    install(TARGETS navitab_plugin DESTINATION navitab/xplane/Navitab/mac_x64)
elseif(UNIX)
    target_compile_definitions(navitab_plugin PRIVATE LIN=1)
    target_link_options(navitab_plugin PRIVATE
        -static-libgcc
        -static-libstdc++
    )
    target_link_libraries(navitab_plugin
        navitab_core
        navitab_platform
        navitab_simxp
        GL
    )
endif()

set_target_properties(navitab_plugin PROPERTIES PREFIX "")
set_target_properties(navitab_plugin PROPERTIES OUTPUT_NAME "Navitab")
set_target_properties(navitab_plugin PROPERTIES SUFFIX ".xpl")
