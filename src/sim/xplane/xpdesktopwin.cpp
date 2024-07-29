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

#include "xpdesktopwin.h"
#include <cassert>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "navitab/config.h"
#include "navitab/core.h"

namespace navitab {

XPDesktopWindow::XPDesktopWindow()
:   XPlaneWindow("xpdeskwin"),
    winPoppedOut(false),
    winResizePollTimer(0),
    desktopPosition()
{
}

XPDesktopWindow::~XPDesktopWindow()
{
    assert(!winHandle);
}

void XPDesktopWindow::Create(std::shared_ptr<Preferences> prefs, std::shared_ptr<WindowEvents> core)
{
    SetPrefs(prefs);
    Connect(core);

    auto& xwdp = prefs->Get("/xplane/window/desktop");
    try {
        desktopPosition.left = xwdp.at("/left"_json_pointer);
        desktopPosition.top = xwdp.at("/top"_json_pointer);
        desktopPosition.right = xwdp.at("/right"_json_pointer);
        desktopPosition.bottom = xwdp.at("/bottom"_json_pointer);
    }
    catch (...) {}
    auto& dp = desktopPosition; // an alias to reduce line lengths!
    LOGD(fmt::format("desktop window (construction) ({},{}) -> ({},{})", dp.left, dp.top, dp.right, dp.bottom));

    assert(!winHandle);

    int sleft, stop, sright, sbottom;
    auto centre = screenBounds(sleft, stop, sright, sbottom);

    // sanity check that the desktop window position will fit into the screen bounds?
    int w = dp.right - dp.left;
    int h = dp.top - dp.bottom;
    if ((w < WIN_MIN_WIDTH) || (w > WIN_MAX_WIDTH) || (h < WIN_MIN_HEIGHT) || (h > WIN_MAX_HEIGHT)
                || (dp.left < sleft) || (dp.right >= sright) || (dp.bottom < sbottom) || (dp.top >= stop)) {
        LOGD("recentering non-conformant window");
        dp = WindowPos(centre);
    }

    // Create an XPlane desktop (floating) window
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
        reinterpret_cast<XPDesktopWindow*>(ref)->onDraw();
    };
    cwp.handleMouseClickFunc = [](XPLMWindowID id, int x, int y, XPLMMouseStatus status, void* ref) -> int {
        return reinterpret_cast<XPDesktopWindow*>(ref)->onLeftClick(x, y, status);
    };
    cwp.handleRightClickFunc = [](XPLMWindowID id, int x, int y, XPLMMouseStatus status, void* ref) -> int {
        return reinterpret_cast<XPDesktopWindow*>(ref)->onRightClick(x, y, status);
    };
    cwp.handleKeyFunc = [](XPLMWindowID id, char key, XPLMKeyFlags flags, char vKey, void* ref, int losingFocus) {
        reinterpret_cast<XPDesktopWindow*>(ref)->onKey(key, flags, vKey, losingFocus);
    };
    cwp.handleCursorFunc = [](XPLMWindowID id, int x, int y, void* ref) -> XPLMCursorStatus {
        return reinterpret_cast<XPDesktopWindow*>(ref)->getCursor(x, y);
    };
    cwp.handleMouseWheelFunc = [](XPLMWindowID id, int x, int y, int wheel, int clicks, void* ref) -> int {
        return reinterpret_cast<XPDesktopWindow*>(ref)->onMouseWheel(x, y, wheel, clicks);
    };
    winHandle = XPLMCreateWindowEx(&cwp);
    LOGD(fmt::format("XPLMCreateWindowEx() -> {}", winHandle));
    XPLMSetWindowTitle(winHandle, NAVITAB_NAME " " NAVITAB_VERSION_STR);
    XPLMSetWindowResizingLimits(winHandle, WIN_MIN_WIDTH, WIN_MIN_HEIGHT, WIN_MAX_WIDTH, WIN_MAX_HEIGHT);
    XPLMSetWindowIsVisible(winHandle, winVisible);
}

void XPDesktopWindow::Destroy()
{
    assert(winHandle);

    auto& dp = desktopPosition;
    LOGD(fmt::format("saving desktop window position ({},{}) -> ({},{})", dp.left, dp.top, dp.right, dp.bottom));
    auto xwdp = prefs->Get("/xplane/window/desktop");
    xwdp["left"] = desktopPosition.left;
    xwdp["top"] = desktopPosition.top;
    xwdp["right"] = desktopPosition.right;
    xwdp["bottom"] = desktopPosition.bottom;
    xwdp["visible"] = (winHandle != nullptr);
    prefs->Put("/xplane/window/desktop", xwdp);

    LOGD(fmt::format("XPLMDestroyWindow({})", winHandle));
    XPLMDestroyWindow(winHandle);
    winHandle = nullptr;

    Disconnect();
}

void XPDesktopWindow::Recentre()
{
    assert(winHandle);
    int sleft, stop, sright, sbottom;
    auto centre = screenBounds(sleft, stop, sright, sbottom);
    desktopPosition = WindowPos(centre);
    auto& dp = desktopPosition;
    XPLMSetWindowGeometry(winHandle, dp.left, dp.top, dp.right, dp.bottom);
    XPLMSetWindowPositioningMode(winHandle, xplm_WindowPositionFree, -1);
    Show();
}

int XPDesktopWindow::FrameRate()
{
    // TODO - implement calculation - may be in common base class
    return 1;
}


std::pair<int, int> XPDesktopWindow::screenBounds(int& l, int& t, int& r, int& b)
{
    XPLMGetScreenBoundsGlobal(&l, &t, &r, &b);
    LOGD(fmt::format("Screen bounds (LTRB): {},{} -> {},{}", l, t, r, b));
    return { (l + r) / 2, (t + b) / 2 };
}

void XPDesktopWindow::onDraw()
{
    assert(winHandle);

    // if we get a draw request then the window must be visible, so cancel the watchdog
    winClosedWatchdog = 0;

    // check the window position and size. don't need to do this every frame, to keep the overheads down
    if (++winResizePollTimer > 30) {
        winResizePollTimer = 0;
        winPoppedOut = XPLMWindowIsPoppedOut(winHandle);
        int l, r, t, b;
        if (winPoppedOut) {
            XPLMGetWindowGeometryOS(winHandle, &l, &t, &r, &b);
        } else {
            XPLMGetWindowGeometry(winHandle, &l, &t, &r, &b);
        }
        auto& dp = desktopPosition;
        if ((dp.left != l) || (dp.top != t) || (dp.right != r) || (dp.bottom != b)) {
            // window has been moved or resized
            int dw = (r - l) - (dp.right - dp.left);
            int dh = (t - b) - (dp.top - dp.bottom);
            if (dw || dh) {
                LOGD(fmt::format("resized -> {} x {}", r - l, t - b));
                core->onWindowResize(r - l, t - b);
            } else {
                LOGD(fmt::format("moved -> top-left {},{}", l, t));
            }
            desktopPosition = WindowPos(l, t, r, b);
        }
    }

    // TODO - still need to do the drawing stuff!
}

int XPDesktopWindow::onLeftClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates
    // use the general XPLMGetWindowGeometry() API, it works for both floating and popped out windows
    int l, r, t, b;
    XPLMGetWindowGeometry(winHandle, &l, &t, &r, &b);
    LOGD(fmt::format("onLeftClick({},{},{}) in win lt/rb {},{} -> {},{}", x, y, status, l, t, r, b));
    return 1;
}

int XPDesktopWindow::onRightClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onRightClick({},{},{})", x, y, status));
    return 1;
}

int XPDesktopWindow::onMouseWheel(int x, int y, int wheel, int clicks)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onMouseWheel({},{},{},{})", x, y, wheel, clicks));
    return 1;
}

void XPDesktopWindow::onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus)
{
    LOGD(fmt::format("onKey({},{},{},{})", (int)key, flags, (int)vKey, losingFocus));
}

} // namespace navitab
