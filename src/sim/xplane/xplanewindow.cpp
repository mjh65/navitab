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

// this file inspired by XPlaneGUIDriver in Avitab.

#include "xplanewindow.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "navitab/config.h"

namespace navitab {
namespace sim {

enum {
    WIN_MIN_WIDTH = 400,
    WIN_STD_WIDTH = 600,
    WIN_MAX_WIDTH = 1600,
    WIN_MIN_HEIGHT = 200,
    WIN_STD_HEIGHT = 300,
    WIN_MAX_HEIGHT = 800
};

XPlaneWindow::XPlaneWindow(std::shared_ptr<Preferences> p)
:   prefs(p),
    LOG(std::make_unique<navitab::logging::Logger>("xpwin")),
    winHandle(nullptr),
    visibilityWatchdog(0),
    pollStatusWatchdog(0),
    desktopPosition(),
    vrPosition()
{
    // get the preferences relating to the window so that it can be opened in the
    // same location as it was on the last run. separate entries for desktop and VR.
    int visible = false;
    auto& xwdp = prefs->Get("/xplane/window/desktop");
    try {
        desktopPosition.left = xwdp.at("/left"_json_pointer);
        desktopPosition.top = xwdp.at("/top"_json_pointer);
        desktopPosition.right = xwdp.at("/right"_json_pointer);
        desktopPosition.bottom = xwdp.at("/bottom"_json_pointer);
        visible = xwdp.at("/visible"_json_pointer);
    }
    catch (...) {}
    auto& dp = desktopPosition;
    LOGD(fmt::format("desktop window (construction) ({},{}) -> ({},{})", dp.left, dp.top, dp.right, dp.bottom));

    auto& xwvp = prefs->Get("/xplane/window/vr");
    try {
        vrPosition.left = xwvp.at("/left"_json_pointer);
        vrPosition.top = xwvp.at("/top"_json_pointer);
        vrPosition.right = xwvp.at("/right"_json_pointer);
        vrPosition.bottom = xwvp.at("/bottom"_json_pointer);
    }
    catch (...) {}

    if (visible) create();
}

XPlaneWindow::~XPlaneWindow()
{
    auto& dp = desktopPosition;
    LOGD(fmt::format("saving desktop window position ({},{}) -> ({},{})", dp.left, dp.top, dp.right, dp.bottom));
    auto xwdp = prefs->Get("/xplane/window/desktop");
    xwdp["left"] = desktopPosition.left;
    xwdp["top"] = desktopPosition.top;
    xwdp["right"] = desktopPosition.right;
    xwdp["bottom"] = desktopPosition.bottom;
    xwdp["visible"] = (winHandle != nullptr);
    prefs->Put("/xplane/window/desktop", xwdp);

    destroy();
}

void XPlaneWindow::toggle()
{
    if (winHandle) {
        destroy();
    } else {
        create();
    }
}

void XPlaneWindow::recentre()
{
    int sleft, stop, sright, sbottom;
    auto centre = screenBounds(sleft, stop, sright, sbottom);
    desktopPosition = WindowPos(centre);
    auto& dp = desktopPosition;
    if (winHandle) {
        XPLMSetWindowGeometry(winHandle, dp.left, dp.top, dp.right, dp.bottom);
    } else {
        create();
    }
}

void XPlaneWindow::onFlightLoop()
{
    if (!winHandle) return;
    if (++visibilityWatchdog > 10) {
        LOGI("Draw callback watchdog has fired, window has been closed");
        destroy();
    }
}

void XPlaneWindow::onVRmodeChange(bool entering)
{
    // TODO - assumption is that we will close & destroy the current window,
    // and then create a replacement in the other place. maybe there will be
    // some need to defer this for a few frames?
}

void XPlaneWindow::create()
{
    if (winHandle) return;

    // TODO - add VR support later

    int sleft, stop, sright, sbottom;
    auto centre = screenBounds(sleft, stop, sright, sbottom);

    // sanity check that the desktop window position will fit into the screen bounds?
    auto &dp = desktopPosition; // just aliasing really
    int w = dp.right - dp.left;
    int h = dp.top - dp.bottom;
    if ((w < WIN_MIN_WIDTH) || (w > WIN_MAX_WIDTH) || (h < WIN_MIN_HEIGHT) || (h > WIN_MAX_HEIGHT)
            || (dp.left < sleft) || (dp.right >= sright) || (dp.bottom < sbottom) || (dp.top >= stop)) {
        LOGD("recentering non-conformant window");
        dp = WindowPos(centre);
    }

    // Create an XPlane window
    XPLMCreateWindow_t cwp;
    cwp.structSize = sizeof(cwp);
    cwp.left = dp.left;
    cwp.top = dp.top;
    cwp.right = dp.right;
    cwp.bottom = dp.bottom;
    cwp.visible = true;
    cwp.refcon = reinterpret_cast<void*>(this);
    cwp.layer = xplm_WindowLayerFloatingWindows;
    cwp.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
    cwp.drawWindowFunc = [](XPLMWindowID id, void* ref) {
        reinterpret_cast<XPlaneWindow*>(ref)->onDraw();
    };
    cwp.handleMouseClickFunc = [](XPLMWindowID id, int x, int y, XPLMMouseStatus status, void* ref) -> int {
        return reinterpret_cast<XPlaneWindow*>(ref)->onLeftClick(x, y, status);
    };
    cwp.handleRightClickFunc = [](XPLMWindowID id, int x, int y, XPLMMouseStatus status, void* ref) -> int {
        return reinterpret_cast<XPlaneWindow*>(ref)->onRightClick(x, y, status);
    };
    cwp.handleKeyFunc = [](XPLMWindowID id, char key, XPLMKeyFlags flags, char vKey, void* ref, int losingFocus) {
        reinterpret_cast<XPlaneWindow*>(ref)->onKey(key, flags, vKey, losingFocus);
    };
    cwp.handleCursorFunc = [](XPLMWindowID id, int x, int y, void* ref) -> XPLMCursorStatus {
        return reinterpret_cast<XPlaneWindow*>(ref)->getCursor(x, y);
    };
    cwp.handleMouseWheelFunc = [](XPLMWindowID id, int x, int y, int wheel, int clicks, void* ref) -> int {
        return reinterpret_cast<XPlaneWindow*>(ref)->onMouseWheel(x, y, wheel, clicks);
    };
    winHandle = XPLMCreateWindowEx(&cwp);
    LOGD(fmt::format("XPLMCreateWindowEx() -> {}", winHandle));
    XPLMSetWindowTitle(winHandle, NAVITAB_NAME " " NAVITAB_VERSION_STR);
    XPLMSetWindowResizingLimits(winHandle, WIN_MIN_WIDTH, WIN_MIN_HEIGHT, WIN_MAX_WIDTH, WIN_MAX_HEIGHT);
    visibilityWatchdog = 0;
}

void XPlaneWindow::destroy()
{
    if (!winHandle) return;
    LOGD(fmt::format("XPLMDestroyWindow({})", winHandle));
    XPLMDestroyWindow(winHandle);
    winHandle = nullptr;
}

std::pair<int, int> XPlaneWindow::screenBounds(int& l, int& t, int& r, int& b)
{
    XPLMGetScreenBoundsGlobal(&l, &t, &r, &b);
    LOGD(fmt::format("Screen bounds (LTRB): {},{} -> {},{}", l, t, r, b));
    return { (l + r) / 2, (t + b) / 2 };
}

void XPlaneWindow::onDraw()
{
    // TODO - adapt this for VR window
    if (!winHandle) return;
    visibilityWatchdog = 0;
    ++pollStatusWatchdog;
    if (pollStatusWatchdog == 10) {
        int l, r, t, b;
        XPLMGetWindowGeometry(winHandle, &l, &t, &r, &b);
        auto& dp = desktopPosition;
        // right,bottom pos will change on move or resize so test these first
        if ((dp.right != r) || (dp.bottom != b) || (dp.left != l) || (dp.top != t)) {
            // window moved or resized
            int dw = (r - l) - (dp.right - dp.left);
            int dh = (t - b) - (dp.top - dp.bottom);
            if (dw || dh) {
                LOGD(fmt::format("resized -> {} x {}", r - l, t - b));
            } else {
                LOGD(fmt::format("moved -> {},{}", l, t));
            }
            desktopPosition = WindowPos(l, t, r, b);
        }
    } else if (pollStatusWatchdog == 30) {
        // TODO - we might want to watch for VR mode changes here, although hopefully
        // the plugin messages will be reliable so we don't need to.
    } else if (pollStatusWatchdog > 40) {
        pollStatusWatchdog = 0;
    }
}

int XPlaneWindow::onLeftClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onLeftClick({},{},{})", x, y, status));
    return 1;
}

int XPlaneWindow::onRightClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onRightClick({},{},{})", x, y, status));
    return 1;
}

int XPlaneWindow::onMouseWheel(int x, int y, int wheel, int clicks)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onMouseWheel({},{},{},{})", x, y, wheel, clicks));
    return 1;
}

void XPlaneWindow::onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus)
{
    LOGD(fmt::format("onKey({},{},{},{})", (int)key, flags, (int)vKey, losingFocus));
}

XPlaneWindow::WindowPos::WindowPos(std::pair<int, int> centre)
{
    left = centre.first - (WIN_STD_WIDTH / 2);
    top = centre.second + (WIN_STD_HEIGHT / 2);
    right = left + WIN_STD_WIDTH;
    bottom = top - WIN_STD_HEIGHT;
}

} // namespace sim
} // namespace navitab
