# This file is part of the Navitab project. See the README and LICENSE for details.

# Console server for MSFS InGame panel

add_executable(navitab_http)

target_sources(navitab_http PRIVATE
    main.cpp
)

target_link_libraries(navitab_http
    navitab_core
    navitab_sim_mock
    navitab_win_http
)

install(TARGETS navitab_http DESTINATION navitab/tools)
