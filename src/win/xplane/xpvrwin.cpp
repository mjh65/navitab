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
    winResizePollTimer(0)
{
}

XPVRWindow::~XPVRWindow()
{
}

void XPVRWindow::Create(std::shared_ptr<CoreServices> core)
{
    XPlaneWindow::Create(core);

    assert(!winHandle);

    // Create an XPlane VR (floating) window
    XPLMCreateWindow_t cwp;
    cwp.structSize = sizeof(cwp);
    cwp.left = 0;
    cwp.top = 0;
    cwp.right = 0;
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
    XPLMSetWindowPositioningMode(winHandle, xplm_WindowVR, -1);
    XPLMSetWindowTitle(winHandle, NAVITAB_NAME " " NAVITAB_VERSION_STR);
    XPLMSetWindowGeometryVR(winHandle, winWidth, winHeight);
    XPLMSetWindowResizingLimits(winHandle, WIN_MIN_WIDTH, WIN_MIN_HEIGHT, WIN_MAX_WIDTH, WIN_MAX_HEIGHT);
    XPLMSetWindowIsVisible(winHandle, isVisible());
}

void XPVRWindow::Destroy()
{
    assert(winHandle);

    LOGD(fmt::format("XPLMDestroyWindow({})", winHandle));
    XPLMDestroyWindow(winHandle);
    winHandle = nullptr;

    Disconnect();
}

void XPVRWindow::Reset()
{
    assert(winHandle);
    XPLMSetWindowGeometryVR(winHandle, WIN_STD_WIDTH, WIN_STD_HEIGHT);
    XPLMSetWindowIsVisible(winHandle, false);
    Show();
}

void XPVRWindow::onDraw()
{
    assert(winHandle);

    // if we get a draw request then the window must be visible, so cancel the watchdog
    ProdWatchdog();

    // check the window position and size. don't need to do this every frame, to keep the overheads down
    if (++winResizePollTimer > 30) {
        winResizePollTimer = 0;
        if (UpdateWinGeometry()) {
            int w, h;
            XPLMGetWindowGeometryVR(winHandle, &w, &h);
            LOGD(fmt::format("Geometry in VR is {}x{} or {}x{}", w, h, winWidth, winHeight));
            ResizeNotifyAll(winWidth, winHeight);
        }
    }

    RenderContent();
}

// TODO - Avitab has code to capture the VR controller trigger and
// treat this as a left-button mouse for selection and dragging. We
// should probably do this too!

int XPVRWindow::onLeftClick(int x, int y, XPLMMouseStatus status)
{
    // x,y in screen, not window coordinates, we need to convert them,
    // and also into our GUI normal form: 0,0 at top-left.
    ScreenToWindow(x, y);
    LOGD(fmt::format("Mouse event at {},{} (local coordinates)", x, y));

    switch (status) {
    case xplm_MouseDown:
        activeWinPart = LocateWinPart(x, y);
        activeWinPart->client->PostMouseEvent(x - activeWinPart->left, y - activeWinPart->top, true, false);
        break;
    case xplm_MouseDrag:
        activeWinPart->client->PostMouseEvent(x - activeWinPart->left, y - activeWinPart->top, true, false);
        break;
    case xplm_MouseUp:
        activeWinPart->client->PostMouseEvent(x - activeWinPart->left, y - activeWinPart->top, false, false);
        activeWinPart = nullptr;
        break;
    }

    return 1;
}

int XPVRWindow::onMouseWheel(int x, int y, int wheel, int clicks)
{
    // x,y in screen, not window coordinates
    LOGD(fmt::format("onMouseWheel({},{},{},{})", x, y, wheel, clicks));
    UNIMPLEMENTED(__func__);
    return 1;
}

void XPVRWindow::onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus)
{
    LOGD(fmt::format("onKey({},{},{},{})", (int)key, flags, (int)vKey, losingFocus));
    UNIMPLEMENTED(__func__);
}

}
