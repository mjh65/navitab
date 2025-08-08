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

# Stuff that might affect the way the project command works
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE INTERNAL "Minimum macOS version" FORCE)
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "Build architectures for mac OS X" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Navitab requires static libraries")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "Link MSVC static runtime, required by X-Plane plugin")
set(CMAKE_FRAMEWORK FALSE CACHE INTERNAL "")

# Project name and a few useful settings. Other commands can pick up the results
project(Navitab
    VERSION 0.1.0
    DESCRIPTION "An Avitab-like add-on for flight simulation"
    LANGUAGES C CXX
)

# This project is not currently designed to be imported into another project.
# Maybe a later update will split the libraries into separate repos??
if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(FATAL_ERROR "Navitab is not supported as a subsidiary CMake project")
endif()

# Various CMake configuration bits and pieces
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Nicely support folders in IDEs
find_package(Threads)
option(NAVITAB_DOWNLOAD_PREFIX
    "Directory to be used for third-party package downloads"
    "${CMAKE_CURRENT_SOURCE_DIR}/build/download"
)
option(BUILD_NAVITAB_THIRDPARTY
    "If not set then the build will be configured to import pre-built third-party libraries rather than build them."
    ON
)

# Set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF) # ensure -std=c++xx instead of -std=g++xx
if(WIN32 AND MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
endif()

# Testing
include(CTest)

# Docs
add_custom_target(navitab_docs SOURCES README.md)
find_package(Doxygen)
if(NOT Doxygen_FOUND)
    message(STATUS "Doxygen not found, not building docs")
endif()
add_subdirectory(docs)

# External (third-party) source packages are downloaded and configured here.
# Create a single interface library for Navitab which references the packages required.
add_library(navitab_3rdparty INTERFACE)
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

# Host platform definitions applying to everything declared after here
if(WIN32)
    add_compile_definitions(NAVITAB_WINDOWS  _USE_MATH_DEFINES)
    if(MSVC)
        add_compile_definitions(NOMINMAX)
    endif()
elseif(APPLE)
    add_compile_definitions(NAVITAB_MACOSX)
elseif(UNIX)
    add_compile_definitions(NAVITAB_LINUX)
endif()

# Abstract interfaces to the main Navitab architectural components are
# found in NAVITAB_API_DIR. These interfaces are used by the application
# and plugin variants. Interfaces that are strictly internal to the Navitab
# core libraries are kept in the src directory (and referenced using relative
# paths).
set(NAVITAB_API_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(NAVITAB_RES_DIR "${CMAKE_CURRENT_LIST_DIR}/res")

# This custom tool is used to compile SVG icons and is declared at this outer
# level as it is referenced in the resources sub-tree below.
add_executable(navitab_svgc)

# Navitab common source code is here
add_subdirectory(src)

# Navitab resources (icons, etc?) are here
add_subdirectory(res)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if(BUILD_TESTING AND (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING))
    add_subdirectory(tests)
endif()
