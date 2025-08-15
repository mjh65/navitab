# This file is part of the Navitab project. See the README and LICENSE for details.

# Console server for MSFS InGame panel

add_executable(navitab_igps)

target_sources(navitab_igps PRIVATE
    main.cpp
)

target_link_options(navitab_igps PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
)

target_link_libraries(navitab_igps PRIVATE
    navitab_core
    navitab_sim_msfs
    navitab_win_http
)

install(TARGETS navitab_igps DESTINATION navitab/msfs/navitab-ingamepanels-navitab/PanelServer)
