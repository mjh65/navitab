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

# Stuff that might affect the way the project command works
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version" FORCE)
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for mac OS X" FORCE)

# Project name and a few useful settings. Other commands can pick up the results
project(Navitab
    VERSION 0.1.0
    DESCRIPTION "An Avitab-like add-on for flight simulation"
    LANGUAGES CXX
)

# This project is not currently designed to be imported into another project.
# Maybe a later update will split the libraries into separate repos??
if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(FATAL_ERROR "Navitab is not supported as a subsidiary CMake project")
endif()

# Optionally set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF)

# Let's ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Testing
include(CTest)

# Docs
add_custom_target(navitab_docs SOURCES README.md)
find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS "Doxygen not found, not building docs")
endif()

# CMake-compatible external 3rd-party projects are handled here
include(FetchContent)
add_subdirectory(extern)

# Check for simulator SDK's and set variables if defined
if(WIN32 AND DEFINED ENV{MSFS_SDK})
    cmake_path(SET MSFS_SDK NORMALIZE "$ENV{MSFS_SDK}")
    message(STATUS "Detected Microsoft Flight Simulator SDK @ ${MSFS_SDK}")
endif()
if(DEFINED ENV{XPLANE_SDK})
    cmake_path(SET XPLANE_SDK NORMALIZE "$ENV{XPLANE_SDK}")
    message(STATUS "Detected X-Plane_SDK @ ${XPLANE_SDK}")
endif()

# Host platform definitions
if(WIN32)
    add_compile_definitions(NAVITAB_WINDOWS)
    if(MSVC)
        add_compile_definitions(NOMINMAX)
    endif()
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
