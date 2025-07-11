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

# Library for rendering PDF documents, and other image formats.

# Get some paths from the FetchContent properties
FetchContent_GetProperties(mupdf SOURCE_DIR srcdir)
FetchContent_GetProperties(mupdf BINARY_DIR bindir)

# Overwrite the default MuPDF configuration with Navitab's settings (unless done)
file(MD5 "${CMAKE_CURRENT_LIST_DIR}/config.h" refhash)
file(MD5 "${srcdir}/include/mupdf/fitz/config.h" trghash)
if(NOT trghash STREQUAL refhash)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/config.h" "${srcdir}/include/mupdf/fitz/config.h")
endif()

# Define the build process for the MuPDF libraries.
if(WIN32)
    if(MINGW)
        set(corelib "${bindir}/libmupdf.a")
        set(thirdplib "${bindir}/libmupdf-third.a")
        set(buildcmd "${CMAKE_CURRENT_LIST_DIR}/build_mupdf_mingw64.cmd")
    else()
        file(DOWNLOAD "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe" "vswhere.exe")
        set(corelib "${bindir}/mupdf.lib")
        set(thirdplib "${bindir}/mupdf-third.lib")
        set(buildcmd "${CMAKE_CURRENT_LIST_DIR}/build_mupdf_msvc.cmd")
    endif()
else()
    set(corelib "${bindir}/libmupdf.a")
    set(thirdplib "${bindir}/libmupdf-third.a")
    set(buildcmd "${CMAKE_CURRENT_LIST_DIR}/build.sh")
endif()
add_custom_command(
    OUTPUT ${corelib} ${thirdplib}
    COMMAND ${buildcmd} ${srcdir} ${bindir} ${CMAKE_BUILD_TYPE}
    DEPENDS mupdf
    VERBATIM
)
add_custom_target(mupdf_build DEPENDS ${corelib} ${thirdplib})

# Define the MuPDF library targets used by the Navitab core sources
add_library(mupdf_third STATIC IMPORTED)
set_target_properties(mupdf_third PROPERTIES
    IMPORTED_LOCATION ${thirdplib}
)
add_library(mupdf_core STATIC IMPORTED GLOBAL)
set_target_properties(mupdf_core PROPERTIES
    IMPORTED_LOCATION ${corelib}
    INTERFACE_INCLUDE_DIRECTORIES "${srcdir}/include"
    INTERFACE_LINK_LIBRARIES ${thirdplib}
    INTERFACE_LINK_OPTIONS "/VERBOSE"
)
add_dependencies(mupdf_core mupdf_build)
