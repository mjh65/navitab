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

# Any [abstract] interfaces that are used by the different executables
# are found in NAVITAB_API_DIR. All other headers that are internal to
# the Navitab core system are referenced using relative paths from the
# including file.

target_sources(navitab_win_http PRIVATE
    winhttp.cpp
    winhttp.h
    htmlserver.cpp
    htmlserver.h
    httpreq.cpp
    httpreq.h
    cmdhandler.cpp
    cmdhandler.h
)

if(WIN32)
    target_link_libraries(navitab_win_http PUBLIC ws2_32)
endif()
