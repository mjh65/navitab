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

# Works with 3.23 and tested through 3.29
cmake_minimum_required(VERSION 3.23...3.29)
message(${CMAKE_VERSION})

# Project name and a few useful settings. Other commands can pick up the results
project(Navitab
    VERSION 0.1
    DESCRIPTION "An Avitab-like add-on for flight simulation"
    LANGUAGES CXX
)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Optionally set things like CMAKE_CXX_STANDARD here
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(BUILD_SHARED_LIBS OFF)

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

    # Docs only available if this is the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
# FetchContent_MakeAvailable was added in CMake 3.14; simpler usage
include(FetchContent)

# Imported 3rd-party libraries are here
add_subdirectory(extern)

# Navitab sandbox mode adds some temporary apps for build testing
set(NAVITAB_SANDBOX_MODE OFF CACHE BOOL "Enables sandbox mode for testing APIs etc")

# Host platform definitions
if(WIN32)
    add_compile_definitions(NAVITAB_WINDOWS)
elseif(APPLE)
    add_compile_definitions(NAVITAB_MACOSX)
elseif(UNIX)
    add_compile_definitions(NAVITAB_LINUX)
endif()

# Navitab library API is here
set(NAVITAB_API_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

# Navitab common source code is here
add_subdirectory(src)

# Navitab apps, tools and plugins are here
add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if(BUILD_TESTING AND (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING))
    add_subdirectory(tests)
endif()
