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

# Tool for rasterising SVG files to make icons for the UI.

target_sources(navitab_svgc PRIVATE
    main.cpp
)

# this is required to workaround a bug in lunasvg v2.4.1
target_compile_definitions(navitab_svgc PRIVATE
    LUNASVG_BUILD_STATIC
)

target_link_libraries(navitab_svgc PRIVATE
    lunasvg
)

set_target_properties(navitab_svgc PROPERTIES OUTPUT_NAME "svgc")
