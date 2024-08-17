if(WIN32 AND DEFINED ENV{MSFS_SDK})

set(PANEL_OUTPUTS
    "mjh65-ingamepanels-navitab/InGamePanels/mjh65-ingamepanels-navitab.spb"
    "mjh65-ingamepanels-navitab/layout.json"
    "mjh65-ingamepanels-navitab/manifest.json"
)

set(PANEL_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg"
    "${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css"
    "${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html"
    "${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js"
    "${NAVITAB_RES_DIR}/svg/mode_about.svg"
    "${CMAKE_CURRENT_LIST_DIR}/spb/mjh65-ingamepanels-navitab.xml"
    "${CMAKE_CURRENT_LIST_DIR}/spb/PackageDefinitions/mjh65-ingamepanels-navitab.xml"
    "${CMAKE_CURRENT_LIST_DIR}/spb/PackageSources/mjh65-ingamepanels-navitab.xml"
)

add_custom_command(
    OUTPUT ${PANEL_OUTPUTS}
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/build_spb.bat "${CMAKE_CURRENT_LIST_DIR}" "${NAVITAB_RES_DIR}"
    DEPENDS ${PANEL_INPUTS} ${CMAKE_CURRENT_LIST_DIR}/build_spb.bat
)

add_custom_target(
    navitab_ingamepanel ALL
    DEPENDS ${PANEL_OUTPUTS} ${PANEL_INPUTS}
)
endif()

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/mjh65-ingamepanels-navitab" DESTINATION navitab)
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/html_ui" DESTINATION navitab/mjh65-ingamepanels-navitab)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg"
        DESTINATION navitab/mjh65-ingamepanels-navitab/html_ui/Textures/Menu/toolbar)
