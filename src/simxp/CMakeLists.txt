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

add_library(navitab_simxp)

target_sources(navitab_simxp PRIVATE
    datarefexport.cpp
    datarefimport.cpp
)

target_compile_definitions(navitab_simxp PUBLIC
    XPLM400 XPLM303 XPLM301 XPLM300 XPLM210 XPLM200
)

target_include_directories(navitab_simxp PUBLIC
    "${XPLANE_SDK}/CHeaders"
)

if(WIN32)
elseif(APPLE)
elseif(UNIX)
endif()
