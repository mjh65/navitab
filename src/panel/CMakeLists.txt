# This file is part of the Navitab project. See the README and LICENSE for details.

# The same html/css/js sources are used for the MSFS panel and for a local
# web server for testing. The sources are passed through the C pre-processor
# before use in order to tailor the files to the target environment.

if (MSVC)
    function(tailor_panel_file inf outf workd defsym)
        add_custom_command(OUTPUT "${outf}"
            COMMAND "${CMAKE_C_COMPILER}" /nologo /D${defsym}=1 /EP "${inf}" > "${outf}"
            MAIN_DEPENDENCY "${inf}"
            WORKING_DIRECTORY "${workd}"
            COMMENT "Tailoring ${outf}"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endfunction()
else()
    function(tailor_panel_file inf outf workd defsym)
        add_custom_command(OUTPUT "${outf}"
            COMMAND "${CMAKE_C_COMPILER}" -D${defsym}=1 -E -P -o "${outf}" "${inf}"
            MAIN_DEPENDENCY "${inf}"
            WORKING_DIRECTORY "${workd}"
            COMMENT "Tailoring ${outf}"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endfunction()
endif()

function(generate_html_sources panel_out_var htdocs_out_var)
    set(panel_out)
    set(htdocs_out)
    foreach(in_f ${ARGN})
        file(RELATIVE_PATH rpi ${CMAKE_CURRENT_LIST_DIR} ${in_f})
        get_filename_component(rpo "${rpi}" NAME_WLE)
        # Custom rules for MSFS panel variants
        set(out_pf "${CMAKE_CURRENT_BINARY_DIR}/panel/${rpo}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/panel")
        tailor_panel_file("${in_f}" "${out_pf}" "${CMAKE_CURRENT_BINARY_DIR}/panel" NAVITAB_MSFS_PANEL)
        list(APPEND panel_out ${out_pf})
        # Custom rules for local htdocs variants
        set(out_hf "${CMAKE_CURRENT_BINARY_DIR}/htdocs/${rpo}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/htdocs")
        tailor_panel_file("${in_f}" "${out_hf}" "${CMAKE_CURRENT_BINARY_DIR}/htdocs" NAVITAB_MOCK_WWW)
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
    "${CMAKE_CURRENT_LIST_DIR}/html/NavitabProtocol.js.hpp"
)

# These are the SVG icons/images used in both MSFS panel and htdocs environments.
set(PANEL_SVG_INPUTS
    "${NAVITAB_RES_DIR}/svg/favicon.svg"
    "${NAVITAB_RES_DIR}/svg/mode_about.svg"
    "${NAVITAB_RES_DIR}/svg/mode_airport.svg"
    "${NAVITAB_RES_DIR}/svg/mode_docs.svg"
    "${NAVITAB_RES_DIR}/svg/mode_doodler.svg"
    "${NAVITAB_RES_DIR}/svg/mode_keypad.svg"
    "${NAVITAB_RES_DIR}/svg/mode_map.svg"
    "${NAVITAB_RES_DIR}/svg/mode_route.svg"
    "${NAVITAB_RES_DIR}/svg/mode_settings.svg"
    "${NAVITAB_RES_DIR}/svg/tool_affirm.svg"
    "${NAVITAB_RES_DIR}/svg/tool_bottom.svg"
    "${NAVITAB_RES_DIR}/svg/tool_cancel.svg"
    "${NAVITAB_RES_DIR}/svg/tool_centre.svg"
    "${NAVITAB_RES_DIR}/svg/tool_cog.svg"
    "${NAVITAB_RES_DIR}/svg/tool_down.svg"
    "${NAVITAB_RES_DIR}/svg/tool_first.svg"
    "${NAVITAB_RES_DIR}/svg/tool_last.svg"
    "${NAVITAB_RES_DIR}/svg/tool_left.svg"
    "${NAVITAB_RES_DIR}/svg/tool_magnify.svg"
    "${NAVITAB_RES_DIR}/svg/tool_menu.svg"
    "${NAVITAB_RES_DIR}/svg/tool_null.svg"
    "${NAVITAB_RES_DIR}/svg/tool_reduce.svg"
    "${NAVITAB_RES_DIR}/svg/tool_right.svg"
    "${NAVITAB_RES_DIR}/svg/tool_rotateleft.svg"
    "${NAVITAB_RES_DIR}/svg/tool_rotateright.svg"
    "${NAVITAB_RES_DIR}/svg/tool_stop.svg"
    "${NAVITAB_RES_DIR}/svg/tool_top.svg"
    "${NAVITAB_RES_DIR}/svg/tool_up.svg"
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
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.html"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.css"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/NavitabPanel.js"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/layout.json"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/manifest.json"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/InGamePanels/navitab-ingamepanels-navitab.spb"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/icons/toolbar/ICON_TOOLBAR_NAVITAB_PANEL.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_about.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_airport.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_docs.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_doodler.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_keypad.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_map.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_route.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/mode_settings.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_affirm.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_bottom.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_cancel.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_centre.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_cog.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_down.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_first.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_last.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_left.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_magnify.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_menu.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_null.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_reduce.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_right.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_rotateleft.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_rotateright.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_stop.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_top.svg"
    "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel/tool_up.svg"
)

set(PANEL_SPB_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/spb/navitab-ingamepanels-navitab.xml"
    "${CMAKE_CURRENT_LIST_DIR}/spb/PackageDefinitions/navitab-ingamepanels-navitab.xml"
    "${CMAKE_CURRENT_LIST_DIR}/spb/PackageSources/navitab-ingamepanels-navitab.xml"
)

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

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/navitab-ingamepanels-navitab" DESTINATION navitab/msfs)
install(FILES ${PANEL_HTML_INPUTS} DESTINATION navitab/msfs/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel)
install(FILES ${PANEL_ICON_INPUT} DESTINATION navitab/msfs/navitab-ingamepanels-navitab/html_ui/icons/toolbar)
install(FILES ${PANEL_ICON_INPUT} DESTINATION navitab/msfs/navitab-ingamepanels-navitab/html_ui/Textures/Menu/toolbar)
install(FILES ${PANEL_SVG_INPUTS} DESTINATION navitab/msfs/navitab-ingamepanels-navitab/html_ui/InGamePanels/NavitabPanel)

endif()
