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

#include "xpvrwin.h"
#include <cassert>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "navitab/config.h"

namespace navitab {

XPVRWindow::XPVRWindow()
:   XPlaneWindow("xpvrwin"),
    winResizePollTimer(0),
    width(0), height(0)
{
}

XPVRWindow::~XPVRWindow()
{
}

int XPVRWindow::FrameRate()
{
    return 1;
}

void XPVRWindow::Create(std::shared_ptr<Preferences> prefs, std::shared_ptr<WindowEvents> core)
{
    SetPrefs(prefs);
    Connect(core);

    assert(!winHandle);

    // Create an XPlane VR (floating) window
    XPLMCreateWindow_t cwp;
    cwp.structSize = sizeof(cwp);
    cwp.left = 0;
    cwp.top = WIN_STD_HEIGHT;
    cwp.right = WIN_STD_WIDTH;
    cwp.bottom = 0;
    cwp.visible = true;
    cwp.refcon = reinterpret_cast<void*>(this);
    cwp.layer = xplm_WindowLayerFloatingWindows;
    cwp.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
    cwp.drawWindowFunc = [](XPLMWindowID id, void* ref) {
        reinterpret_cast<XPVRWindow*>(ref)->onDraw();
        };
    cwp.handleMouseClickFunc = [](XPLMWindowID id, int x, int y, XPLMMouseStatus status, void* ref) -> int {
        return reinterpret_cast<XPVRWindow*>(ref)->onLeftClick(x, y, status);
        };
    cwp.handleRightClickFunc = [](XPLMWindowID id, int x, int y, XPLMMouseStatus status, void* ref) -> int {
        return reinterpret_cast<XPVRWindow*>(ref)->onRightClick(x, y, status);
        };
    cwp.handleKeyFunc = [](XPLMWindowID id, char key, XPLMKeyFlags flags, char vKey, void* ref, int losingFocus) {
        reinterpret_cast<XPVRWindow*>(ref)->onKey(key, flags, vKey, losingFocus);
        };
    cwp.handleCursorFunc = [](XPLMWindowID id, int x, int y, void* ref) -> XPLMCursorStatus {
        return reinterpret_cast<XPVRWindow*>(ref)->getCursor(x, y);
        };
    cwp.handleMouseWheelFunc = [](XPLMWindowID id, int x, int y, int wheel, int clicks, void* ref) -> int {
        return reinterpret_cast<XPVRWindow*>(ref)->onMouseWheel(x, y, wheel, clicks);
        };
    winHandle = XPLMCreateWindowEx(&cwp);
    LOGD(fmt::format("XPLMCreateWindowEx() -> {}", winHandle));
    XPLMSetWindowTitle(winHandle, NAVITAB_NAME " " NAVITAB_VERSION_STR);
    XPLMSetWindowResizingLimits(winHandle, WIN_STD_WIDTH - 10, WIN_STD_HEIGHT - 10, WIN_STD_WIDTH + 10, WIN_STD_HEIGHT + 10);
    XPLMSetWindowPositioningMode(winHandle, xplm_WindowVR, -1);
    XPLMSetWindowIsVisible(winHandle, winVisible);
}

void XPVRWindow::Destroy()
{
    assert(winHandle);

    LOGD(fmt::format("XPLMDestroyWindow({})", winHandle));
    XPLMDestroyWindow(winHandle);
    winHandle = nullptr;

    Disconnect();
}

void XPVRWindow::Recentre()
{
    assert(winHandle);
    XPLMSetWindowGeometryVR(winHandle, WIN_STD_WIDTH, WIN_STD_HEIGHT);
    Show();
}

void XPVRWindow::onDraw()
{
    assert(winHandle);

    // if we get a draw request then the window must be visible, so cancel the watchdog
    winClosedWatchdog = 0;

    // check the window position and size. don't need to do this every frame, to keep the overheads down
    if (++winResizePollTimer > 30) {
        winResizePollTimer = 0;
        int w, h;
        XPLMGetWindowGeometryVR(winHandle, &w, &h);
        if ((width != w) || (height != h)) {
            LOGD(fmt::format("resized -> {} x {}", w, h));
            core->onWindowResize(w, h);
        }
        width = w; height = h;
    }

    // TODO - still need to do the drawing stuff!
}

int XPVRWindow::onLeftClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates
    int l, r, t, b;
    XPLMGetWindowGeometry(winHandle, &l, &t, &r, &b);
    int w, h;
    XPLMGetWindowGeometryVR(winHandle, &w, &h);
    LOGD(fmt::format("onLeftClick({},{},{}) in win({},{}) lt/rb {},{} -> {},{}", x, y, status, w, h, l, t, r, b));
    return 1;
}

int XPVRWindow::onRightClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onRightClick({},{},{})", x, y, status));
    return 1;
}

int XPVRWindow::onMouseWheel(int x, int y, int wheel, int clicks)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onMouseWheel({},{},{},{})", x, y, wheel, clicks));
    return 1;
}

void XPVRWindow::onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus)
{
    LOGD(fmt::format("onKey({},{},{},{})", (int)key, flags, (int)vKey, losingFocus));
}

}
