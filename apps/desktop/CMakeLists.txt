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

add_executable(navitab_desktop WIN32 MACOSX_BUNDLE)

target_link_libraries(navitab_desktop
    navitab_core
    navitab_platform
    lvgl::lvgl
)

if(WIN32)
    target_sources(navitab_desktop PRIVATE "${CMAKE_CURRENT_LIST_DIR}/win32main.cpp")
    target_compile_definitions(navitab_desktop PRIVATE UNICODE)
    if(MSVC)
        target_link_options(navitab_desktop PRIVATE /municode)
    endif()
elseif(APPLE)
    target_sources(navitab_desktop PRIVATE "${CMAKE_CURRENT_LIST_DIR}/main.cpp")
elseif(UNIX)
    target_sources(navitab_desktop PRIVATE "${CMAKE_CURRENT_LIST_DIR}/main.cpp")
endif()

install(TARGETS navitab_desktop DESTINATION navitab/tools)
