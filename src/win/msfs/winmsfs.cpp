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

#include "winmsfs.h"
#include <fmt/core.h>
#include "navitab/core.h"
#include "../../win/imagerect.h"

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowMSFS>();
}

namespace navitab {

WindowMSFS::WindowMSFS()
:   LOG(std::make_unique<logging::Logger>("winmsfs"))
{
}

WindowMSFS::~WindowMSFS()
{
}

void WindowMSFS::SetPrefs(std::shared_ptr<Preferences> prefs)
{
    UNIMPLEMENTED(__func__);
}

void WindowMSFS::Connect(std::shared_ptr<WindowEvents> core)
{
    UNIMPLEMENTED(__func__);
}

void WindowMSFS::Disconnect()
{
    UNIMPLEMENTED(__func__);
}

int WindowMSFS::EventLoop(int maxLoops)
{
    UNIMPLEMENTED(__func__);
    return 0;
}

void WindowMSFS::SetHandlers(std::shared_ptr<Toolbar>, std::shared_ptr<Modebar>, std::shared_ptr<Doodler>, std::shared_ptr<Keypad>)
{
    UNIMPLEMENTED(__func__);
}

std::unique_ptr<ImageRectangle> WindowMSFS::RefreshCanvas(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowMSFS::RefreshToolbar(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowMSFS::RefreshModebar(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowMSFS::RefreshDoodler(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowMSFS::RefreshKeypad(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

void WindowMSFS::Brightness(int percent)
{
    UNIMPLEMENTED(__func__);
}


} // namespace navitab
