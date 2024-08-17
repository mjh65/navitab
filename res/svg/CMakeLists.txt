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

# Navitab uses SVG format icons in its UI. These are used in their source
# format for the MSFS panel and web-tester variants. For the XPlane and
# GLFW variants the SV files are pre-compiled to embedded bitmaps which
# are then drawn directly into the frame buffers. There is a custom tool
# to do this.

set(SVGC_SOURCES
    # This style of encoding allows specification of the generated bitmap
    # width and height in the pseudo source file name. The actual SVG source
    # file name is everything before the final 2 extensions!
    mode_about.svg.40.40
    mode_airport.svg.40.40
    mode_about.svg.40.40
    mode_airport.svg.40.40
    mode_docs.svg.40.40
    mode_doodler.svg.40.40
    mode_keypad.svg.40.24
    mode_map.svg.40.40
    mode_route.svg.40.40
    mode_settings.svg.40.40
    tool_bottom.svg.24.24
    tool_cancel.svg.24.24
    tool_centre.svg.24.24
    tool_cog.svg.24.24
    tool_down.svg.24.24
    tool_first.svg.24.24
    tool_last.svg.24.24
    tool_left.svg.24.24
    tool_right.svg.24.24
    tool_rotateleft.svg.24.24
    tool_rotateright.svg.24.24
    tool_stop.svg.24.24
    tool_top.svg.24.24
    tool_up.svg.24.24
    tool_zoomin.svg.24.24
    tool_zoomout.svg.24.24
)

set(SVG_TARGETS)

foreach(work ${SVGC_SOURCES})
    get_filename_component(height "${work}" LAST_EXT)
    string(SUBSTRING ${height} 1 -1 height)
    get_filename_component(work "${work}" NAME_WLE)
    get_filename_component(width "${work}" LAST_EXT)
    string(SUBSTRING ${width} 1 -1 width)
    get_filename_component(svg_source "${work}" NAME_WLE)
    get_filename_component(base "${svg_source}" NAME_WLE)
    set(file_h "${PROJECT_BINARY_DIR}/svg/${base}_${width}x${height}.h")
    list(APPEND SVG_TARGETS "${file_h}")
    #message(STATUS "CUSTOM ${file_h} from ${svg_source}")
    add_custom_command(
            OUTPUT "${file_h}"
            #COMMENT "CUSTOM: $<TARGET_FILE:navitab_svgc> -o${file_h} -w${width} -h${height} ${CMAKE_CURRENT_LIST_DIR}/${svg_source}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/svg"
            COMMAND $<TARGET_FILE:navitab_svgc> "-o${file_h}" -w${width} -h${height} "${CMAKE_CURRENT_LIST_DIR}/${svg_source}"
            DEPENDS "${svg_source}" navitab_svgc
            VERBATIM
    )
endforeach()

add_custom_target(navitab_svg_headers DEPENDS ${SVG_TARGETS})
