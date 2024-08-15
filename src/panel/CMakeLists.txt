if(WIN32 AND DEFINED ENV{MSFS_SDK})
add_custom_command(
    OUTPUT
        navitab-ingamepanels-navitab/html_ui/icons/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css
        navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html
        navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js
        navitab-ingamepanels-navitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        navitab-ingamepanels-navitab/InGamePanels/navitab-ingamepanels-navitab.spb
        navitab-ingamepanels-navitab/layout.json
        navitab-ingamepanels-navitab/manifest.json
    COMMAND
        ${CMAKE_CURRENT_LIST_DIR}/build.sh
    DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/build.sh
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/icons/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js
        ${CMAKE_CURRENT_LIST_DIR}/spb/navitab-ingamepanels-navitab.xml
        ${CMAKE_CURRENT_LIST_DIR}/spb/PackageDefinitions/navitab-ingamepanels-navitab.xml
        ${CMAKE_CURRENT_LIST_DIR}/spb/PackageSources/navitab-ingamepanels-navitab.xml
)

add_custom_target(
    Navitab-IngamePanel ALL
    DEPENDS
        navitab-ingamepanels-navitab/html_ui/icons/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css
        navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html
        navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js
        navitab-ingamepanels-navitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        navitab-ingamepanels-navitab/InGamePanels/navitab-ingamepanels-navitab.spb
        navitab-ingamepanels-navitab/layout.json
        navitab-ingamepanels-navitab/manifest.json
)
endif()
