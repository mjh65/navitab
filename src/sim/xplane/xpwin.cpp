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
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
//#include <GL/glext.h>
#endif
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "navitab/core.h"

namespace navitab {

XPlaneWindow::XPlaneWindow(const char* logId)
:   LOG(std::make_unique<logging::Logger>(logId)),
    winHandle(nullptr),
    winDrawWatchdog(0),
    wgl(0), wgt(0), wgr(0), wgb(0),
    winVisible(false),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT)
{
}

XPlaneWindow::~XPlaneWindow()
{
}

void XPlaneWindow::Brightness(int percent)
{
}

void XPlaneWindow::Show()
{
    assert(winHandle);
    winVisible = true;
    winDrawWatchdog = 0;
    XPLMSetWindowIsVisible(winHandle, true);
}

void XPlaneWindow::CheckVitalSigns()
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

void XPlaneWindow::ProdWatchdog()
{
    // The watchdog prod comes from the onDraw() callback, and this only
    // happens if the window is visible.
    winDrawWatchdog = 0;
    winVisible = true;
}

bool XPlaneWindow::UpdateWinGeometry()
{
    int l, t, r, b;
    XPLMGetWindowGeometry(winHandle, &l, &t, &r, &b);
    if ((wgl != l) || (wgt != t) || (wgr != r) || (wgb != b)) {
        wgl = l; wgt = t; wgr = r; wgb = b;
        auto popped = XPLMWindowIsPoppedOut(winHandle);
        auto vr = XPLMWindowIsInVR(winHandle);
        const char* mode = vr ? "VR" : popped ? "OS" : "IG";
        int w = r - l;
        int h = t - b;
        LOGD(fmt::format("Geometry now :{}: {},{} -> {},{} ({}x{})", mode, l, t, r, b, w, h));
        if ((w != winWidth) || (h != winHeight)) {
            LOGD(fmt::format("Width has changed from {}x{} to {}x{}", winWidth, winHeight, w, h));
            winWidth = w; winHeight = h;
            return true;
        }
    }
    return false;
}

void XPlaneWindow::ScreenToWindow(int& x, int& y)
{
    x = x - wgl;
    y = winHeight - (y - wgb);
}

bool XPlaneWindow::isActive()
{
    return winVisible;
}


} // namespace navitab
