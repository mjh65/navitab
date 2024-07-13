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

# Desktop UI program - no target simulator, for interactive UI testing

add_executable(navitab_desktop
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_link_libraries(navitab_desktop
    lvgl::lvgl
)

if(WIN32)
    target_link_libraries(navitab_desktop
        glfw3
        opengl32
        -Wl,--subsystem,windows
    )
elseif(APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_directories(navitab_desktop PRIVATE ${GLFW_LIBRARY_DIRS})

    add_definitions(-DGL_SILENCE_DEPRECATION)
    target_link_libraries(navitab_desktop
        ${GLFW_STATIC_LIBRARIES}
        "-framework OpenGL"
        curl
    )
elseif(UNIX)
    target_link_libraries(navitab_desktop
        GL
        glfw
        pthread
    )
endif()
