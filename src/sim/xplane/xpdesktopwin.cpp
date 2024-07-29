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
#include <algorithm>
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
    winLeft(10), winTop(WIN_STD_HEIGHT + 10)
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
        winLeft = xwdp.at("/left"_json_pointer);
        winTop = xwdp.at("/top"_json_pointer);
    }
    catch (...) {}
    LOGD(fmt::format("desktop window (construction) ({}x{}) at ({},{})", winWidth, winHeight, winLeft, winTop));

    assert(!winHandle);

    int sLeft, sTop, sRight, sBottom;
    auto centre = screenCentre(sLeft, sTop, sRight, sBottom);

    // sanity check that the desktop window position will have some part of the
    // window title bar visible (and therefore grabbable!)
    winTop = std::min(winTop, sTop - 70);
    winTop = std::max(winTop, sBottom + 20);
    winLeft = std::min(winLeft, sRight - 40);
    auto wr = winLeft + winWidth;
    wr = std::max(wr, sLeft + 40);
    winLeft = wr - winWidth;

    // Create an XPlane desktop (floating) window
    XPLMCreateWindow_t cwp;
    cwp.structSize = sizeof(cwp);
    cwp.left = winLeft;
    cwp.top = winTop;
    cwp.right = winLeft + winWidth;
    cwp.bottom = winTop - winHeight;
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
    XPLMSetWindowIsVisible(winHandle, isVisible());
}

void XPDesktopWindow::Destroy()
{
    assert(winHandle);

    LOGD(fmt::format("saving desktop window position ({},{})", winLeft, winTop));
    auto xwdp = prefs->Get("/xplane/window/desktop");
    xwdp["left"] = winLeft;
    xwdp["top"] = winTop;
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
    auto centre = screenCentre(sleft, stop, sright, sbottom);
    winLeft = centre.first - (WIN_STD_WIDTH / 2);
    winTop = centre.second + (WIN_STD_HEIGHT / 2);
    winWidth = WIN_STD_WIDTH; winHeight = WIN_STD_HEIGHT;
    XPLMSetWindowGeometry(winHandle, winLeft, winTop, winLeft + winWidth, winTop - winHeight);
    XPLMSetWindowPositioningMode(winHandle, xplm_WindowPositionFree, -1);
    Show();
}

int XPDesktopWindow::FrameRate()
{
    // TODO - implement calculation - may be in common base class
    return 1;
}


std::pair<int, int> XPDesktopWindow::screenCentre(int& l, int& t, int& r, int& b)
{
    XPLMGetScreenBoundsGlobal(&l, &t, &r, &b);
    LOGD(fmt::format("Screen bounds (LTRB): {},{} -> {},{}", l, t, r, b));
    return { (l + r) / 2, (t + b) / 2 };
}

void XPDesktopWindow::onDraw()
{
    assert(winHandle);

    // if we get a draw request then the window must be visible, so cancel the watchdog
    prodWatchdog();

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
        int w = r - l;
        int h = t - b;
        if ((w != winWidth) || (h != winHeight)) {
            winWidth = w; winHeight = h;
            LOGD(fmt::format("resized -> width-height {}x{}", winWidth, winHeight));
            core->onWindowResize(winWidth, winHeight);
        }
        if ((l != winLeft) || (t != winTop)) {
            winLeft = l; winTop = t;
            LOGD(fmt::format("moved -> left-top {},{}", l, t));
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
