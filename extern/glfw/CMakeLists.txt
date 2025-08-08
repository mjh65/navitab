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

# GLFW project - provides OpenGL window manager, etc

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory("${glfw_SOURCE_DIR}" "${glfw_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_glfw_PKGLIBS glfw CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_glfw_LOCATION "${glfw_BINARY_DIR}/src/glfw3.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_glfw_LOCATION "${glfw_BINARY_DIR}/src/${XCODE_LIB_PATH_PREFIX}libglfw3.a" CACHE INTERNAL "")
endif()
set(NAVITAB_glfw_INCDIRS "${glfw_SOURCE_DIR}/include" CACHE INTERNAL "")
