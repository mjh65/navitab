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

add_library(navitab_plugin SHARED
    "${CMAKE_CURRENT_LIST_DIR}/plugin.cpp"
)

if(WIN32)
    target_compile_definitions(navitab_plugin PRIVATE IBM=1)
    target_link_libraries(navitab_plugin
        -static
        -static-libgcc
        -static-libstdc++
        # other dependencies here
        opengl32
    )
elseif(APPLE)
    target_compile_definitions(navitab_plugin PRIVATE APL=1)
    target_link_libraries(navitab_plugin
        #"-F${PROJECT_SOURCE_DIR}/build-third/lib/ -framework XPLM"
        "-framework OpenGL"
        # other dependencies here
    )
elseif(UNIX)
    target_compile_definitions(navitab_plugin PRIVATE LIN=1)
    target_link_libraries(navitab_plugin
        -static-libgcc
        -static-libstdc++
        -fPIC
        # other dependencies here
        GL
    )
endif()

set_target_properties(navitab_plugin PROPERTIES PREFIX "")
set_target_properties(navitab_plugin PROPERTIES OUTPUT_NAME "Navitab")
set_target_properties(navitab_plugin PROPERTIES SUFFIX ".xpl")
