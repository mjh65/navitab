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

include_directories("${NAVITAB_API_DIR}")

if(NAVITAB_SANDBOX_MODE)
message(STATUS "Navitab in sandbox mode")
add_subdirectory(sandbox)
else()
add_subdirectory(desktop)
add_subdirectory(console)
add_subdirectory(navdb)
add_subdirectory(msfs)
add_subdirectory(xplane)
endif()
