#   NaviTab - Navigation Tablet for VR, derived from Folke Will's AviTab
#   Copyright (c) 2024 Michael Hasling
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

# Alternative graphics library that Folke mentioned

set(BUILD_SHARED_LIBS OFF)
set(DEFAULT_SLINT_EMBED_RESOURCES "embed-for-software-renderer")

#set(SLINT_FEATURE_FREESTANDING ON)
#set(SLINT_FEATURE_BACKEND_WINIT OFF)
#set(SLINT_FEATURE_RENDERER_SOFTWARE ON)
#set(SLINT_FEATURE_BACKEND_QT OFF)


#SLINT_FEATURE_BACKEND_WINIT
#SLINT_FEATURE_BACKEND_WINIT_WAYLAND
#SLINT_FEATURE_BACKEND_WINIT_X11
#SLINT_FEATURE_COMPILER
#SLINT_FEATURE_EXPERIMENAL
#SLINT_FEATURE_FOO_BAR
#SLINT_FEATURE_FREESTANDING
#SLINT_FEATURE_INTERPRETER
#SLINT_FEATURE_RENDERER_
#SLINT_FEATURE_RENDERER_FEMTOVG
#SLINT_FEATURE_RENDERER_SKIA
#SLINT_FEATURE_RENDERER_SKIA_OPENGL
#SLINT_FEATURE_RENDERER_SKIA_VULKAN
#SLINT_FEATURE_RENDERER_SOFTWARE
#SLINT_FEATURE_RENDERER_WINIT_



message(STATUS "Fetching slint")
FetchContent_Declare(
  slint
  URL "https://github.com/slint-ui/slint/archive/refs/tags/v1.6.0.zip"
  SOURCE_SUBDIR api/cpp
)

FetchContent_MakeAvailable(slint)
