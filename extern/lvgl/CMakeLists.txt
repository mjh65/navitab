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

# Graphics library used by Avitab

message(STATUS "Fetching lvgl")

FetchContent_Declare(lvgl
    URL "https://github.com/lvgl/lvgl/archive/refs/tags/v9.1.0.zip"
    SOURCE_SUBDIR bogus_directory_does_not_exist
)
FetchContent_MakeAvailable(lvgl)

set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE INTERNAL "")
#set(LV_CONF_BUILD_DISABLE_THORVG_INTERNAL ON CACHE BOOL "")
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON CACHE BOOL "")
set(LV_CONF_BUILD_DISABLE_DEMOS ON CACHE BOOL "")
set(LVGL_ROOT_DIR "${lvgl_SOURCE_DIR}")

project(lvgl LANGUAGES C CXX HOMEPAGE_URL https://github.com/lvgl/lvgl)
include(${LVGL_ROOT_DIR}/env_support/cmake/custom.cmake)

#target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)
