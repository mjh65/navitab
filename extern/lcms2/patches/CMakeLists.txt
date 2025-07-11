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

# This file is a patch for the LCMS2 external library, and will be copied
# into the MuPDF downloaded thirdparty tree before it is added into the build.
# It SHOULD NOT be included directly with add_subdirectory.

# Works with 3.23 and tested through 3.29
cmake_minimum_required(VERSION 3.23...3.29)

# Project name and a few useful settings. Other commands can pick up the results
project(LCMS2
    VERSION 2.2.17
    DESCRIPTION "A library for color management"
    LANGUAGES C CXX
)

# Set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Stuff needed to build the LCMS2 library.

# Use installed packages directly. Might need to add some fallback
# code later if any of these fail on other platforms.
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# The LCMS2 library
set(LCMS2_SOURCES
    src/cmsalpha.c
    src/cmscam02.c
    src/cmscgats.c
    src/cmscnvrt.c
    src/cmserr.c
    src/cmsgamma.c
    src/cmsgmt.c
    src/cmshalf.c
    src/cmsintrp.c
    src/cmsio0.c
    src/cmsio1.c
    src/cmslut.c
    src/cmsmd5.c
    src/cmsmtrx.c
    src/cmsnamed.c
    src/cmsopt.c
    src/cmspack.c
    src/cmspcs.c
    src/cmsplugin.c
    src/cmsps2.c
    src/cmssamp.c
    src/cmssm.c
    src/cmstypes.c
    src/cmsvirt.c
    src/cmswtpnt.c
    src/cmsxform.c
)
set(LCMS2_HEADERS
    include/lcms2.h
)

add_library(lcms2 STATIC ${LCMS2_SOURCES} ${LCMS2_HEADERS})
add_library(LCMS2::lcms2 ALIAS lcms2)
target_include_directories(lcms2
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
)
