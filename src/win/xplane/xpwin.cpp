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

#include <cassert>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <XPLM/XPLMGraphics.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "xpwin.h"
#include "../texbuffer.h"
#include "navitab/core.h"


namespace navitab {

std::vector<int> XPlaneWindow::xpTextureIds;

XPlaneWindow::XPlaneWindow(const char* logId)
:   LOG(std::make_unique<logging::Logger>(logId)),
    winHandle(nullptr),
    winDrawWatchdog(0),
    wgl(0), wgt(0), wgr(0), wgb(0),
    winVisible(false),
    brightness(1.0f),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT)
{
}

XPlaneWindow::~XPlaneWindow()
{
}

void XPlaneWindow::Brightness(int percent)
{
    if (percent < 0) percent = 0;
    else if (percent > 100) percent = 100;
    brightness = 0.1f + (0.9f * percent / 100.0f);
}

void XPlaneWindow::Paint(int part, const FrameBuffer* src, const std::vector<FrameRegion>& regions)
{
#if 0
    // This function is called from the core thread.
    const std::lock_guard<std::mutex> lock(imageMutex);
    std::unique_ptr<ImageRectangle> returnedImage;
    if (partImages[part]) partImages[part]->Reset();
    returnedImage = std::move(partImages[part]);
    partImages[part] = std::move(newImage);
    return returnedImage;
#endif
}

void XPlaneWindow::Create(std::shared_ptr<CoreServices> core)
{
    if (xpTextureIds.empty()) {
        xpTextureIds.resize(WindowPart::TOTAL_PARTS);
        XPLMGenerateTextureNumbers(xpTextureIds.data(), WindowPart::TOTAL_PARTS);
        for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
            XPLMBindTexture2d(xpTextureIds[i], 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    Connect(core);
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

void XPlaneWindow::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;

    prefs = core->GetPrefsManager();
    auto& xwdp = prefs->Get("/xplane/window");
    try {
        winWidth = xwdp.at("/width"_json_pointer);
        winHeight = xwdp.at("/height"_json_pointer);
    }
    catch (...) {}
    // apply the constraints immediately
    winWidth = std::min(std::max(winWidth, (int)WIN_MIN_WIDTH), (int)WIN_MAX_WIDTH);
    winHeight = std::min(std::max(winHeight, (int)WIN_MIN_HEIGHT), (int)WIN_MAX_HEIGHT);

    for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
        parts[i] = core->GetPartCallbacks(i);
        parts[i]->SetPainter(shared_from_this());
    }
    ResizeNotifyAll(winWidth, winHeight);
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

    for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
        parts[i].reset();
    }
    prefs.reset();

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

void XPlaneWindow::ResizeNotifyAll(int w, int h)
{
    parts[WindowPart::TOOLBAR]->PostResize(w, TOOLBAR_HEIGHT);
    parts[WindowPart::MODEBAR]->PostResize(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    parts[WindowPart::DOODLER]->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
    parts[WindowPart::KEYPAD]->PostResize(w - MODEBAR_WIDTH, KEYPAD_HEIGHT);
    parts[WindowPart::CANVAS]->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
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

void XPlaneWindow::RenderContent()
{
    int left, top, right, bottom;
    XPLMGetWindowGeometry(winHandle, &left, &top, &right, &bottom);
    RenderPart(WindowPart::CANVAS, left, top - TOOLBAR_HEIGHT, right, bottom);
    RenderPart(WindowPart::TOOLBAR, left, top, right, top - TOOLBAR_HEIGHT);
    RenderPart(WindowPart::MODEBAR, left, top - TOOLBAR_HEIGHT, left + MODEBAR_WIDTH, std::max(top - (TOOLBAR_HEIGHT + MODEBAR_HEIGHT), bottom));
    RenderPart(WindowPart::DOODLER, left + MODEBAR_WIDTH, top - TOOLBAR_HEIGHT, right, bottom);
    RenderPart(WindowPart::KEYPAD, left + MODEBAR_WIDTH, bottom + KEYPAD_HEIGHT, right, bottom);
}

void XPlaneWindow::RenderPart(int part, int left, int top, int right, int bottom)
{
    const std::lock_guard<std::mutex> lock(imageMutex);
    if (!partImages[part]) return;

    auto& image = *(partImages[part]);

    XPLMBindTexture2d(xpTextureIds[part], 0);
    if (image.NeedsRegistration()) {
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, image.Width(), image.Height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
    }

    XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
    glColor4f(brightness, brightness, brightness, 1.0f);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
        0, 0, image.Width(), image.Height(),
        GL_RGBA, GL_UNSIGNED_BYTE, image.Data());

    glBegin(GL_QUADS);
    // map top left texture to bottom left vertex
    glTexCoord2i(0, 1); glVertex2i(left, bottom);
    // map bottom left texture to top left vertex
    glTexCoord2i(0, 0); glVertex2i(left, top);
    // map bottom right texture to top right vertex
    glTexCoord2i(1, 0); glVertex2i(right, top);
    // map top right texture to bottom right vertex
    glTexCoord2i(1, 1); glVertex2i(right, bottom);
    glEnd();
}

} // namespace navitab
