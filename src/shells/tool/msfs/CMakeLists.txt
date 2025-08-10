# This file is part of the Navitab project. See the README and LICENSE for details.

# Console server for MSFS InGame panel

add_executable(navitab_igps)

target_sources(navitab_igps PRIVATE
    main.cpp
)

target_link_libraries(navitab_igps
    navitab_core
    navitab_sim_msfs
    navitab_win_http
)

install(TARGETS navitab_igps DESTINATION navitab/msfs/navitab-ingamepanels-navitab/PanelServer)
