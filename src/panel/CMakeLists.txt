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

# The same html/css/js sources are used for the MSFS panel and for a local
# web server for testing. The sources are passed through the C pre-processor
# before use in order to tailor the files to the target environment.

function(generate_html_sources panel_out_var htdocs_out_var)
    set(panel_out)
    set(htdocs_out)
    foreach(in_f ${ARGN})
        message(STATUS "Generating from input ${in_f}")
        file(RELATIVE_PATH rpi ${CMAKE_CURRENT_LIST_DIR} ${in_f})
        get_filename_component(rpo "${rpi}" NAME_WLE)
        message(STATUS "Relative path is ${rpo}")
        # Custom rules for MSFS panel variants
        set(out_pf "${CMAKE_CURRENT_BINARY_DIR}/panel/${rpo}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/panel")
        add_custom_command(OUTPUT ${out_pf}
            COMMAND "${CMAKE_C_COMPILER}" -o ${out_pf} -E -P ${in_f}
            MAIN_DEPENDENCY ${in_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Creating panel source ${out_pf}"
            VERBATIM
        )
        list(APPEND panel_out ${out_pf})
        # Custom rules for local htdocs variants
        set(out_hf "${CMAKE_CURRENT_BINARY_DIR}/htdocs/${rpo}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/htdocs")
        add_custom_command(OUTPUT ${out_hf}
            COMMAND "${CMAKE_C_COMPILER}" -D NAVITAB_MOCK_WWW=1 -E -P -o ${out_hf} ${in_f}
            MAIN_DEPENDENCY ${in_f}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Creating panel source ${out_hf}"
            VERBATIM
        )
        list(APPEND htdocs_out ${out_hf})
    endforeach()
    set(${panel_out_var} "${panel_out}" PARENT_SCOPE)
    set(${htdocs_out_var} "${htdocs_out}" PARENT_SCOPE)
endfunction()

# This generates tailored html/css/js files for the MSFS panel and htdocs environments.
generate_html_sources(PANEL_HTML_INPUTS HTDOCS_HTML_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.css.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.html.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.js.hpp"
)

# These are the SVG icons/images used in both MSFS panel and htdocs environments.
set(PANEL_SVG_INPUTS
    "${NAVITAB_RES_DIR}/svg/mode_about.svg"
    "${NAVITAB_RES_DIR}/svg/mode_airport.svg"
    "${NAVITAB_RES_DIR}/svg/mode_docs.svg"
    "${NAVITAB_RES_DIR}/svg/mode_doodler.svg"
    "${NAVITAB_RES_DIR}/svg/mode_keypad.svg"
    "${NAVITAB_RES_DIR}/svg/mode_map.svg"
    "${NAVITAB_RES_DIR}/svg/mode_route.svg"
    "${NAVITAB_RES_DIR}/svg/mode_settings.svg"
    "${NAVITAB_RES_DIR}/svg/tool_bottom.svg"
    "${NAVITAB_RES_DIR}/svg/tool_cancel.svg"
    "${NAVITAB_RES_DIR}/svg/tool_centre.svg"
    "${NAVITAB_RES_DIR}/svg/tool_cog.svg"
    "${NAVITAB_RES_DIR}/svg/tool_down.svg"
    "${NAVITAB_RES_DIR}/svg/tool_first.svg"
    "${NAVITAB_RES_DIR}/svg/tool_last.svg"
    "${NAVITAB_RES_DIR}/svg/tool_left.svg"
    "${NAVITAB_RES_DIR}/svg/tool_right.svg"
    "${NAVITAB_RES_DIR}/svg/tool_rotateleft.svg"
    "${NAVITAB_RES_DIR}/svg/tool_rotateright.svg"
    "${NAVITAB_RES_DIR}/svg/tool_stop.svg"
    "${NAVITAB_RES_DIR}/svg/tool_top.svg"
    "${NAVITAB_RES_DIR}/svg/tool_up.svg"
    "${NAVITAB_RES_DIR}/svg/tool_zoomin.svg"
    "${NAVITAB_RES_DIR}/svg/tool_zoomout.svg"
)

add_custom_target(
    navitab_htdocs ALL
    DEPENDS ${HTDOCS_HTML_INPUTS} ${PANEL_SVG_INPUTS}
)

# This builds a web-server htdocs environment.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/html/index.html" DESTINATION navitab/htdocs)
install(FILES ${HTDOCS_HTML_INPUTS} DESTINATION navitab/htdocs)
install(FILES ${PANEL_SVG_INPUTS} DESTINATION navitab/htdocs)

# This section for the MSFS panel is only enabled on Windows and if the SDK is installed.
if(WIN32 AND DEFINED ENV{MSFS_SDK})

set(PANEL_OUTPUTS
    "mjh65-ingamepanels-navitab/layout.json"
    "mjh65-ingamepanels-navitab/manifest.json"
    "mjh65-ingamepanels-navitab/InGamePanels/mjh65-ingamepanels-navitab.spb"
    "mjh65-ingamepanels-navitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg"
    "mjh65-ingamepanels-navitab/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_about.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_airport.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_docs.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_doodler.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_keypad.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_map.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_route.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_settings.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_bottom.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_cancel.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_centre.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_cog.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_down.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_first.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_last.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_left.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_right.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_rotateleft.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_rotateright.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_stop.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_top.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_up.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_zoomin.svg"
    "mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_zoomout.svg"
)

set(PANEL_SPB_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/spb/mjh65-ingamepanels-navitab.xml"
    "${CMAKE_CURRENT_LIST_DIR}/spb/PackageDefinitions/mjh65-ingamepanels-navitab.xml"
    "${CMAKE_CURRENT_LIST_DIR}/spb/PackageSources/mjh65-ingamepanels-navitab.xml"
)

#set(PANEL_HTML_INPUTS
#    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.css"
#    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.html"
#    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.js"
#)

set(PANEL_ICON_INPUT
    "${CMAKE_CURRENT_LIST_DIR}/svg/ICON_TOOLBAR_NAVITAB_PANEL.svg"
)

add_custom_command(
    OUTPUT ${PANEL_OUTPUTS}
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/build_spb.bat "${CMAKE_CURRENT_LIST_DIR}" "${NAVITAB_RES_DIR}"
    DEPENDS ${PANEL_SPB_INPUTS} ${PANEL_HTML_INPUTS} ${PANEL_ICON_INPUT} ${PANEL_SVG_INPUTS} ${CMAKE_CURRENT_LIST_DIR}/build_spb.bat
)

add_custom_target(
    navitab_ingamepanel ALL
    DEPENDS ${PANEL_OUTPUTS}
)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mjh65-ingamepanels-navitab" DESTINATION navitab)
install(FILES ${PANEL_HTML_INPUTS} DESTINATION navitab/mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel)
install(FILES ${PANEL_ICON_INPUT} DESTINATION navitab/mjh65-ingamepanels-navitab/html_ui/icons/toolbar)
install(FILES ${PANEL_ICON_INPUT} DESTINATION navitab/mjh65-ingamepanels-navitab/html_ui/Textures/Menu/toolbar)
install(FILES ${PANEL_SVG_INPUTS} DESTINATION navitab/mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel)

endif()

