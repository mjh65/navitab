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

# Navitab custom CMake script since Gumbo does not provide one.

# Project name and a few useful settings. Other commands can pick up the results
project(GUMBO
    VERSION 0.10.1
    DESCRIPTION "A library for html5 parsing"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

# The GUMBO library
set(GUMBO_SOURCES
    string_buffer.c
    util.c
    vector.c
    attribute.c
    char_ref.c
    utf8.c
    error.c
    tag.c
    parser.c
    tokenizer.c
    string_piece.c
)
PrefixSources(GUMBO_SOURCES "${gumbo_SOURCE_DIR}/src")

add_library(gumbo STATIC ${GUMBO_SOURCES})
target_include_directories(gumbo
    PUBLIC "${gumbo_SOURCE_DIR}/src"
)
if(MSVC)
    target_include_directories(gumbo PRIVATE "${gumbo_SOURCE_DIR}/visualc/include")
endif()
