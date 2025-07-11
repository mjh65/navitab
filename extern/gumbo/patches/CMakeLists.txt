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

# This file is a patch for the GUMBO external library, and will be copied
# into the MuPDF downloaded thirdparty tree before it is added into the build.
# It SHOULD NOT be included directly with add_subdirectory.

# Works with 3.23 and tested through 3.29
cmake_minimum_required(VERSION 3.23...3.29)

# Project name and a few useful settings. Other commands can pick up the results
project(GUMBO
    VERSION 0.10.1
    DESCRIPTION "A library for html5 parsing"
    LANGUAGES C CXX
)

# Set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Stuff needed to build the GUMBO library.

# Use installed packages directly. Might need to add some fallback
# code later if any of these fail on other platforms.
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# The GUMBO library
set(GUMBO_SOURCES
    src/string_buffer.c
    src/util.c
    src/vector.c
    src/attribute.c
    src/char_ref.c
    src/utf8.c
    src/error.c
    src/tag.c
    src/parser.c
    src/tokenizer.c
    src/string_piece.c
)
set(GUMBO_HEADERS
    src/gumbo.h
)

add_library(gumbo STATIC ${GUMBO_SOURCES} ${GUMBO_HEADERS})
add_library(GUMBO::gumbo ALIAS gumbo)
target_include_directories(gumbo
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src"
)
if(MSVC)
    target_include_directories(gumbo PRIVATE "${CMAKE_CURRENT_LIST_DIR}/visualc/include")
endif()
