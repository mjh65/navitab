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

# LunaSVG provides libraries for rendering SVG images to bitmaps.

add_subdirectory("${lunasvg_SOURCE_DIR}" "${lunasvg_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_lunasvg_PKGLIBS lunasvg plutovg CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_lunasvg_LOCATION "${lunasvg_BINARY_DIR}/lunasvg.lib" CACHE INTERNAL "")
    set(NAVITAB_plutovg_LOCATION "${lunasvg_BINARY_DIR}/plutovg/plutovg.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_lunasvg_LOCATION "${lunasvg_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}liblunasvg.a" CACHE INTERNAL "")
    set(NAVITAB_plutovg_LOCATION "${lunasvg_BINARY_DIR}/plutovg/${XCODE_LIB_PATH_PREFIX}libplutovg.a" CACHE INTERNAL "")
endif()
set(NAVITAB_lunasvg_INCDIRS "${lunasvg_SOURCE_DIR}/include" CACHE INTERNAL "")
set(NAVITAB_lunasvg_LINKLIBS plutovg CACHE INTERNAL "")
