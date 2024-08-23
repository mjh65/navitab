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

set(PANEL_HTML_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.css"
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.html"
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabPanel.js"
)

set(PANEL_ICON_INPUT
    "${CMAKE_CURRENT_LIST_DIR}/svg/ICON_TOOLBAR_NAVITAB_PANEL.svg"
)

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
