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
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "navitab/core.h"

namespace navitab {

XPlaneWindow::XPlaneWindow(const char* logId)
:   LOG(std::make_unique<logging::Logger>(logId)),
    winHandle(nullptr),
    winVisible(false),
    winDrawWatchdog(0),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT)
{
}

XPlaneWindow::~XPlaneWindow()
{
}

void XPlaneWindow::Show()
{
    assert(winHandle);
    winVisible = true;
    winDrawWatchdog = 0;
    XPLMSetWindowIsVisible(winHandle, true);
}

void XPlaneWindow::onFlightLoop()
{
    assert(winHandle);

    // XPLMGetWindowIsVisible() returns true even if the user closed the window.
    // So, if the window is supposed to be visible then check the watchdog counter.
    // It gets reset on every call to onDraw. If it reaches 10 then the window is not
    // being drawn. It might have been closed, or moved off-screen. Either way ...
    if (winVisible && (++winDrawWatchdog > 10)) {
        LOGD("Draw callback watchdog has fired, window is not visible");
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
    auto& xwdp = prefs->Get("/xplane/window");
    try {
        winWidth = xwdp.at("/width"_json_pointer);
        winHeight = xwdp.at("/height"_json_pointer);
    }
    catch (...) {}
    // apply the constraints immediately
    winWidth = std::min(std::max(winWidth, (int)WIN_MIN_WIDTH), (int)WIN_MAX_WIDTH);
    winHeight = std::min(std::max(winHeight, (int)WIN_MIN_HEIGHT), (int)WIN_MAX_HEIGHT);
}

void XPlaneWindow::Connect(std::shared_ptr<WindowEvents> wcb)
{
    core = wcb;
}

void XPlaneWindow::Disconnect()
{
    winWidth = std::min(std::max(winWidth, (int)WIN_MIN_WIDTH), (int)WIN_MAX_WIDTH);
    winHeight = std::min(std::max(winHeight, (int)WIN_MIN_HEIGHT), (int)WIN_MAX_HEIGHT);
    LOGD(fmt::format("saving window dimensions ({}x{})", winWidth, winHeight));
    auto xwdp = prefs->Get("/xplane/window");
    xwdp["width"] = winWidth;
    xwdp["height"] = winHeight;
    prefs->Put("/xplane/window", xwdp);

    core.reset();
}

void XPlaneWindow::prodWatchdog()
{
    // The watchdog prod comes from the onDraw() callback, and this only
    // happens if the window is visible.
    winDrawWatchdog = 0;
    winVisible = true;
}

} // namespace navitab
