/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// inspired by GlfwGUIDriver.cpp in AviTab.

#include "winglfw.h"
#include <stdexcept>
#include <chrono>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowGLFW>();
}

namespace navitab {

WindowGLFW::WindowGLFW()
{
}

WindowGLFW::~WindowGLFW()
{
}

void WindowGLFW::SetPrefs(std::shared_ptr<Preferences> prefs)
{
}

void WindowGLFW::Connect(std::shared_ptr<WindowEvents> core)
{
}

void WindowGLFW::Disconnect()
{
}

int WindowGLFW::FrameRate()
{
}

} // namespace navitab
