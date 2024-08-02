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

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowMSFS>();
}

namespace navitab {

WindowMSFS::WindowMSFS()
{
}

WindowMSFS::~WindowMSFS()
{
}

void WindowMSFS::SetPrefs(std::shared_ptr<Preferences> prefs)
{
}

void WindowMSFS::Connect(std::shared_ptr<WindowEvents> core)
{
}

void WindowMSFS::Disconnect()
{
}

int WindowMSFS::EventLoop(int maxLoops)
{
    return 0;
}

void WindowMSFS::Brightness(int percent)
{
    UNIMPLEMENTED("");
}


} // namespace navitab