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


configure_file(config.h.in "${PROJECT_BINARY_DIR}/navitab/config.h")

# The Navitab sources are built into one core library for all executables,
# simulator-specific libraries for X-Plane, Flight Simulator and the mock
# simulator (for development and test), and UI-window libraries.
# Each executable will link the core library, and whichever simulation
# and UI-window libraries are prescribed.

# All of the core Navitab classes are put in here
add_library(navitab_core)
target_include_directories(navitab_core PRIVATE
    "${NAVITAB_API_DIR}"
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(navitab_core PUBLIC
    navitab_core_3rdparty
)

# The mock simulator generates random or scripted events for the Navitab core.
add_library(navitab_sim_mock)
target_include_directories(navitab_sim_mock PRIVATE
    "${NAVITAB_API_DIR}"
    "${PROJECT_BINARY_DIR}"
)
# TODO target_link_libaries for navitab_sim_mock

# The glfw window library provides a desktop GUI window interface to the Navitab core.
add_library(navitab_win_glfw)
target_include_directories(navitab_win_glfw PRIVATE
    "${NAVITAB_API_DIR}"
    "${PROJECT_BINARY_DIR}"
)
if(APPLE)
    target_compile_definitions(navitab_win_glfw PRIVATE GL_SILENCE_DEPRECATION)
endif()
target_link_libraries(navitab_win_glfw PRIVATE
    navitab_core
    glfw
)

# The http window library provides an HTTP web server to stream images to a browser.
# The browser may be a desktop app, or the MSFS built-in used for in-game panels.
add_library(navitab_win_http)
target_include_directories(navitab_win_http PRIVATE
    "${NAVITAB_API_DIR}"
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(navitab_win_http PRIVATE
    navitab_core
)

if(XPLANE_SDK)
    # The XPlane library provides interfacing to XPlane for both simulator and window interfaces.
    add_library(navitab_sim_xp)
    target_include_directories(navitab_sim_xp PUBLIC
        "${NAVITAB_API_DIR}"
        "${PROJECT_BINARY_DIR}"
        "${XPLANE_SDK}/CHeaders"
    )
    target_compile_definitions(navitab_sim_xp PUBLIC
        XPLM400 XPLM303 XPLM301 XPLM300 XPLM210 XPLM200
        APL=$<BOOL:${APPLE}> IBM=$<BOOL:${WIN32}> LIN=$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>
    )
    if(APPLE)
        target_compile_definitions(navitab_sim_xp PRIVATE GL_SILENCE_DEPRECATION)
    endif()
    target_link_libraries(navitab_sim_xp PRIVATE
        navitab_core
    )
endif()

if(MSFS_SDK)
    # The MSFS library provides interfacing to MSFS for both simulator and window interfaces.
    add_library(navitab_sim_msfs)
    target_include_directories(navitab_sim_msfs PRIVATE
        "${NAVITAB_API_DIR}"
        "${PROJECT_BINARY_DIR}"
    )
    target_link_libraries(navitab_sim_msfs PRIVATE
        navitab_core
    )
endif()

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(lvglkit)
add_subdirectory(apps)
add_subdirectory(imgkit)
add_subdirectory(maps)
add_subdirectory(docs)
add_subdirectory(navdb)
add_subdirectory(sim)
add_subdirectory(win)
add_subdirectory(panel)
add_subdirectory(shells)
