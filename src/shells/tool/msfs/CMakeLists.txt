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
