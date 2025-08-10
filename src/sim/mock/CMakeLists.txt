# This file is part of the Navitab project. See the README and LICENSE for details.

target_sources(navitab_sim_mock PRIVATE
    mocksim.cpp
)

target_link_libraries(navitab_sim_mock PRIVATE navitab_core)
