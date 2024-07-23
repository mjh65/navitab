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

#include_directories("${NAVITAB_API_DIR}")
configure_file(config.h.in "${PROJECT_BINARY_DIR}/navitab/config.h")

# The Navitab sources are built into one core library for all executables,
# and simulator-specific libraries for X-Plane, Flight Simulator and the
# mock simulator (for development and test).
# Each executable will link the core library, and the relevant
# simulator-specific library.

add_library(navitab_core)
target_include_directories(navitab_core PRIVATE
    "${NAVITAB_API_DIR}"
    "${PROJECT_BINARY_DIR}"
)

add_library(navitab_sim_mock)
target_include_directories(navitab_sim_mock PRIVATE
    "${NAVITAB_API_DIR}"
    "${PROJECT_BINARY_DIR}"
)
if(XPLANE_SDK)
    add_library(navitab_sim_xp)
    target_include_directories(navitab_sim_xp PRIVATE
        "${NAVITAB_API_DIR}"
        "${PROJECT_BINARY_DIR}"
    )
endif()
if(MSFS_SDK)
    add_library(navitab_sim_msfs)
    target_include_directories(navitab_sim_msfs PRIVATE
        "${NAVITAB_API_DIR}"
        "${PROJECT_BINARY_DIR}"
    )
endif()

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(lvglcpp)
add_subdirectory(charts)
add_subdirectory(sim)
