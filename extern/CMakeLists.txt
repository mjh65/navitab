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

# This script handles all of the 3rd party libraries that are used by the
# Navitab project. A number of these projects do not support CMake natively,
# and in those cases a rudimentary CMake project has been created and is
# patched into the source tree after downloading. The goal is to have a
# fully cross-platform build system (specifically supporting VS/MSVC).

# TODO - the following 3rd party libraries are used by Avitab, but have not
# yet been brought into Navitab. It's possible they aren't even required.

# url = https://github.com/hglm/detex
# url = https://github.com/nothings/stb
# url = https://github.com/curl/curl
# url = https://github.com/ARMmbed/mbedtls
# url = https://github.com/vadz/libtiff
# url = https://github.com/OSGeo/proj.4
# url = https://github.com/OSGeo/libgeotiff
# url = https://github.com/ldarren/QuickJS/
# url = https://github.com/gulrak/filesystem.git


cmake_policy(SET CMP0077 NEW)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

message(STATUS "Downloading and configuring external content")

find_package(Threads REQUIRED)

add_subdirectory(zlib)
add_subdirectory(bzip2)
add_subdirectory(brotli)
add_subdirectory(fmtlib)
add_subdirectory(json)
add_subdirectory(png)
add_subdirectory(jbig2dec)
add_subdirectory(freetype)
add_subdirectory(harfbuzz)
add_subdirectory(gumbo)
add_subdirectory(lcms2)
add_subdirectory(libjpeg)
add_subdirectory(libtiff)
add_subdirectory(openjpeg)
add_subdirectory(mupdf)
add_subdirectory(glfw)
add_subdirectory(lvgl)
add_subdirectory(lunasvg)

message(STATUS "Finished making external content available")

add_library(navitab_core_3rdparty INTERFACE)
target_link_libraries(navitab_core_3rdparty
    INTERFACE Threads::Threads
    INTERFACE ZLIB::zlib
    INTERFACE BZip2::bzip2
    INTERFACE png_static
    INTERFACE brotlidec brotlienc
    INTERFACE harfbuzz
    INTERFACE freetype
    INTERFACE fmt::fmt
    INTERFACE nlohmann_json::nlohmann_json
    INTERFACE JBIG2::jbig2
    INTERFACE GUMBO::gumbo
    INTERFACE lcms2
    INTERFACE jpeg_static
    INTERFACE TIFF::tiff
    INTERFACE openjp2
    INTERFACE mupdf
    INTERFACE lvgl::lvgl
)
