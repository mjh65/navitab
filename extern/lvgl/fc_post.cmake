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

# FetchContent_MakeAvailable post-processing

# Due to some issues in CMake with VC toolchain the normal process of
# adding the top-level CMakeLists.txt file is overridden to remove the
# ASM language (which causes the issue hinted at earlier).

if(MSVC)
    set(LVGL_ROOT_DIR "${lvgl_SOURCE_DIR}")
    project(lvgl LANGUAGES C CXX HOMEPAGE_URL https://github.com/lvgl/lvgl)
    include(${LVGL_ROOT_DIR}/env_support/cmake/custom.cmake)
endif()
