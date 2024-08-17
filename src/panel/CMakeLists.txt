if(WIN32 AND DEFINED ENV{MSFS_SDK})

add_custom_command(
    OUTPUT
    mjh65-ingamepanels-navitab/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg
    mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css
    mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html
    mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js
    mjh65-ingamepanels-navitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg
    mjh65-ingamepanels-navitab/InGamePanels/mjh65-ingamepanels-navitab.spb
    mjh65-ingamepanels-navitab/layout.json
    mjh65-ingamepanels-navitab/manifest.json
    COMMAND
    ${CMAKE_CURRENT_LIST_DIR}/build.sh
    DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/build.sh
    ${CMAKE_CURRENT_LIST_DIR}/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg
    ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css
    ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html
    ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js
    ${CMAKE_CURRENT_LIST_DIR}/spb/mjh65-ingamepanels-navitab.xml
    ${CMAKE_CURRENT_LIST_DIR}/spb/PackageDefinitions/mjh65-ingamepanels-navitab.xml
    ${CMAKE_CURRENT_LIST_DIR}/spb/PackageSources/mjh65-ingamepanels-navitab.xml
)

add_custom_target(
    navitab_ingamepanel ALL
    DEPENDS
    mjh65-ingamepanels-navitab/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg
    mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css
    mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html
    mjh65-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js
    mjh65-ingamepanels-navitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg
    mjh65-ingamepanels-navitab/InGamePanels/mjh65-ingamepanels-navitab.spb
    mjh65-ingamepanels-navitab/layout.json
    mjh65-ingamepanels-navitab/manifest.json
)
endif()

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mjh65-ingamepanels-navitab DESTINATION navitab)
