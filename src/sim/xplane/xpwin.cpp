/*
 *  Navitab - Navigation tablet for VR flight simulation
 *  Copyright (C) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (C) 2018-2024 Folke Will <folko@solhost.org>
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

#include "xpwin.h"
#include <cassert>

namespace navitab {

XPlaneWindow::XPlaneWindow(const char* logId)
:   LOG(std::make_unique<logging::Logger>(logId)),
    winHandle(nullptr),
    winVisible(false),
    winClosedWatchdog(0)
{
}

XPlaneWindow::~XPlaneWindow()
{
}

void XPlaneWindow::onFlightLoop()
{
    assert(winHandle);

    // XPLMGetWindowIsVisible() returns true even if the user closed the window.
    // So, if the window is supposed to be visible then check the watchdog counter.
    // It gets reset on every call to onDraw. If it reaches 10 then the window is not
    // being drawn, so it must have been closed!
    if (winVisible && (++winClosedWatchdog > 10)) {
        LOGD("Draw callback watchdog has fired, window has been closed");
        winVisible = false;
    }
}

bool XPlaneWindow::isActive()
{
    return winVisible;
}

void XPlaneWindow::SetPrefs(std::shared_ptr<Preferences> p)
{
    prefs = p;
}

void XPlaneWindow::Connect(std::shared_ptr<WindowEvents> wcb)
{
    core = wcb;
}

void XPlaneWindow::Disconnect()
{
    core.reset();
}

} // namespace navitab
