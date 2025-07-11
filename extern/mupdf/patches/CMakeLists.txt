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

# This file is a patch for the MuPDF external library, and will be copied
# into the MuPDF downloaded source tree before it is added into the build.
# It SHOULD NOT be included directly with add_subdirectory.

# Works with 3.23 and tested through 3.29
cmake_minimum_required(VERSION 3.23...3.29)

# Project name and a few useful settings. Other commands can pick up the results
project(MuPDF
    VERSION 1.26.3
    DESCRIPTION "A library for rendering PDF and other formats"
    LANGUAGES C CXX
)

# Set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Stuff needed to build the MuPDF library.
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

file(GLOB MUPDF_SOURCES
    "source/fitz/*.c"
    "source/html/*.c"
    "source/pdf/*.c"
    "source/svg/*.c"
    "generated/resources/fonts/urw/*.cff.c"
    "source/cbz/muimg.c"
)
file(GLOB MUPDF_HEADERS "include/mupdf/*.h")

add_library(mupdf STATIC ${MUPDF_SOURCES} ${MUPDF_HEADERS})
add_library(MuPDF::mupdf ALIAS mupdf)
target_include_directories(mupdf
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
)
target_link_libraries(mupdf
    PUBLIC brotlidec brotlienc
    PUBLIC ZLIB::zlib
    PUBLIC lcms2
    PUBLIC JBIG2::jbig2
    PUBLIC jpeg_static
    PUBLIC openjp2
    PUBLIC freetype
    PUBLIC harfbuzz
    PUBLIC GUMBO::gumbo
)
