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
    sample.svg
)

set(SVG_TARGETS)

foreach(svg_source ${SVGC_SOURCES})
    get_filename_component(file_svg "${svg_source}" NAME_WE)
    set(file_h "${PROJECT_BINARY_DIR}/svg/${file_svg}.h")
    list(APPEND SVG_TARGETS "${file_h}")
    message(STATUS "CUSTOM ${file_h} from ${svg_source}")
    add_custom_command(
            OUTPUT "${file_h}"
            COMMENT "CUSTOM: $<TARGET_FILE:navitab_svgc> -o ${file_h} ${CMAKE_CURRENT_SOURCE_DIR}/${svg_source}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/svg"
            COMMAND $<TARGET_FILE:navitab_svgc> -o "${file_h}" "${CMAKE_CURRENT_SOURCE_DIR}/${svg_source}"
            DEPENDS "${svg_source}" navitab_svgc
            VERBATIM
    )
endforeach()

add_custom_target(navitab_svg_headers DEPENDS ${SVG_TARGETS})
