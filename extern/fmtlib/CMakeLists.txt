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

# Formatting library, adds fmt::fmt to C++ namespace

set(FMT_DEBUG_POSTFIX "" CACHE INTERNAL "")
set(FMT_INSTALL OFF)

add_subdirectory("${fmtlib_SOURCE_DIR}" "${fmtlib_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_fmtlib_PKGLIBS fmt CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_fmt_LOCATION "${fmtlib_BINARY_DIR}/fmt.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_fmt_LOCATION "${fmtlib_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libfmt.a" CACHE INTERNAL "")
endif()
set(NAVITAB_fmt_INCDIRS "${fmtlib_SOURCE_DIR}/include" CACHE INTERNAL "")
