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

# Curl library for carrying out client-side URL transfers, used to fetch
# map tiles, charts, docs, etc.

# Curl library has some external dependencies which are not part of the MSVC build
# environment, so these may eventually need to be added here. Currently it means the
# MSVC build of curl might not support all the protocols, but that may only be an
# issue for a small subset of downloads (TBD!)

message(STATUS "Fetching curl")
FetchContent_Declare(curl
    URL "https://github.com/curl/curl/releases/download/curl-8_14_1/curl-8.14.1.zip"
)

# PSL is required by default, manually disabled until a suitable CMake-usable project is found.
set(CURL_USE_LIBPSL OFF)

FetchContent_MakeAvailable(curl)
