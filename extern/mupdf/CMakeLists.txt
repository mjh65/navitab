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

# Navitab custom CMake for the MuPDF library, since it doesn't provide one.

project(MuPDF
    VERSION 1.26.3
    DESCRIPTION "A library for rendering PDF and other formats"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

file(GLOB MUPDF_SOURCES
    "${mupdf_SOURCE_DIR}/source/fitz/*.c"
    "${mupdf_SOURCE_DIR}/source/html/*.c"
    "${mupdf_SOURCE_DIR}/source/pdf/*.c"
    "${mupdf_SOURCE_DIR}/source/svg/*.c"
    "${mupdf_SOURCE_DIR}/generated/resources/fonts/urw/*.cff.c"
    "${mupdf_SOURCE_DIR}/source/cbz/muimg.c"
)

add_library(mupdf STATIC ${MUPDF_SOURCES})
target_include_directories(mupdf
    PUBLIC "${mupdf_SOURCE_DIR}/include"
)
target_link_libraries(mupdf
    PUBLIC zlibstatic
    PUBLIC brotlidec
    PUBLIC lcms2
    PUBLIC jbig2dec
    PUBLIC jpeg_static
    PUBLIC openjp2
    PUBLIC freetype
    PUBLIC harfbuzz
    PUBLIC gumbo
)
