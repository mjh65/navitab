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

# This file is a patch for the JBIG2 external library, and will be copied
# into the MuPDF downloaded thirdparty tree before it is added into the build.
# It SHOULD NOT be included directly with add_subdirectory.

# Works with 3.23 and tested through 3.29
cmake_minimum_required(VERSION 3.23...3.29)

# Project name and a few useful settings. Other commands can pick up the results
project(JBIG2
    VERSION 0.20.0
    DESCRIPTION "A library for decoding JBIG2 compressed images"
    LANGUAGES C CXX
)

# Set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Stuff needed to build the JBIG2 library.

# Use installed packages directly. Might need to add some fallback
# code later if any of these fail on other platforms.
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# The JBIG2 library
set(JBIG2_SOURCES
    jbig2_arith_iaid.c
    jbig2_arith_int.c
    jbig2_arith.c
    jbig2_generic.c
    jbig2_halftone.c
    jbig2_huffman.c
    jbig2_hufftab.c
    jbig2_image_pbm.c
    jbig2_image_png.c
    jbig2_image.c
    jbig2_mmr.c
    jbig2_page.c
    jbig2_refinement.c
    jbig2_segment.c
    jbig2_symbol_dict.c
    jbig2_text.c
    jbig2.c
)
set(JBIG2_HEADERS
    jbig2.h
)

add_library(jbig2 STATIC ${JBIG2_SOURCES} ${JBIG2_HEADERS})
add_library(JBIG2::jbig2 ALIAS jbig2)
target_include_directories(jbig2
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}"
)
target_link_libraries(jbig2
    PUBLIC PNG::PNG
)
