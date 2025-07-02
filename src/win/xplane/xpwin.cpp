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
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT),
    winDrawWatchdog(0),
    winGeomLeft(0), winGeomTop(0), winGeomRight(0), winGeomBottom(0),
    winVisible(false),
    brightness(1.0f),
    activeWinPart(0)
{
    for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
        winParts[i].active = 0;
        winParts[i].top = winParts[i].left = 0;
    }
    winParts[WindowPart::CANVAS].textureImage = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, (WIN_MAX_HEIGHT - TOOLBAR_HEIGHT));
    winParts[WindowPart::CANVAS].top = TOOLBAR_HEIGHT;
    winParts[WindowPart::TOOLBAR].textureImage = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, TOOLBAR_HEIGHT);
    winParts[WindowPart::MODEBAR].textureImage = std::make_unique<TextureBuffer>(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    winParts[WindowPart::MODEBAR].top = TOOLBAR_HEIGHT;
    winParts[WindowPart::DOODLER].textureImage = std::make_unique<TextureBuffer>((WIN_MAX_WIDTH - MODEBAR_WIDTH), (WIN_MAX_HEIGHT - TOOLBAR_HEIGHT));
    winParts[WindowPart::DOODLER].top = TOOLBAR_HEIGHT;
    winParts[WindowPart::KEYPAD].textureImage = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, KEYPAD_HEIGHT);
    winParts[WindowPart::KEYPAD].top = winHeight - KEYPAD_HEIGHT;
    winParts[WindowPart::KEYPAD].left = MODEBAR_WIDTH;
}

XPlaneWindow::~XPlaneWindow()
{
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
    core->SetWindowControl(shared_from_this());
    prefs = core->GetSettingsManager();
    auto& xwdp = prefs->Get("/xplane/window");
    try {
        winWidth = xwdp.at("/width"_json_pointer);
        winHeight = xwdp.at("/height"_json_pointer);
    }
    catch (...) {}
    // apply the constraints immediately
    winWidth = std::min(std::max(winWidth, (int)WIN_MIN_WIDTH), (int)WIN_MAX_WIDTH);
    winHeight = std::min(std::max(winHeight, (int)WIN_MIN_HEIGHT), (int)WIN_MAX_HEIGHT);

    winParts[WindowPart::TOOLBAR].client = core->GetToolbar();
    winParts[WindowPart::TOOLBAR].client->SetPainter(shared_from_this());
    winParts[WindowPart::MODEBAR].client = core->GetModebar();
    winParts[WindowPart::MODEBAR].client->SetPainter(shared_from_this());
    winParts[WindowPart::DOODLER].client = core->GetDoodler();
    winParts[WindowPart::DOODLER].client->SetPainter(shared_from_this());
    winParts[WindowPart::KEYPAD].client = core->GetKeypad();
    winParts[WindowPart::KEYPAD].client->SetPainter(shared_from_this());
    winParts[WindowPart::CANVAS].client = core->GetCanvas();
    winParts[WindowPart::CANVAS].client->SetPainter(shared_from_this());

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
        winParts[i].client.reset();
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
    if ((winGeomLeft != l) || (winGeomTop != t) || (winGeomRight != r) || (winGeomBottom != b)) {
        winGeomLeft = l; winGeomTop = t; winGeomRight = r; winGeomBottom = b;
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
    winParts[WindowPart::CANVAS].client->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
    winParts[WindowPart::TOOLBAR].client->PostResize(w, TOOLBAR_HEIGHT);
    winParts[WindowPart::MODEBAR].client->PostResize(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    winParts[WindowPart::DOODLER].client->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
    winParts[WindowPart::KEYPAD].client->PostResize(w - MODEBAR_WIDTH, KEYPAD_HEIGHT);
}

void XPlaneWindow::ScreenToWindow(int& x, int& y)
{
    x = x - winGeomLeft;
    y = winHeight - (y - winGeomBottom);
}

bool XPlaneWindow::isActive()
{
    return winVisible;
}

void XPlaneWindow::Paint(int part, const FrameBuffer* src, const std::vector<FrameRegion>& regions)
{
    // This function is called from the core thread.
    const std::lock_guard<std::mutex> lock(paintMutex);
    auto& wp = winParts[part];
    wp.active = src && regions.size();
    if (!wp.active) return;
    auto& ti = wp.textureImage;
    if ((ti->Width() != src->Width()) || (ti->Height() != src->Height())) {
        ti->Resize(src->Width(), src->Height());
}
    ti->CopyRegionsFrom(src, regions);
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
    const std::lock_guard<std::mutex> lock(paintMutex);
    if (!winParts[part].active) return;
    assert(winParts[part].textureImage);

    auto& buffer = *(winParts[part].textureImage);

    XPLMBindTexture2d(xpTextureIds[part], 0);
    if (buffer.NeedsRegistration()) {
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, buffer.Width(), buffer.Height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, buffer.Data());
    }

    XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
    glColor4f(brightness, brightness, brightness, 1.0f);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
        0, 0, buffer.Width(), buffer.Height(),
        GL_RGBA, GL_UNSIGNED_BYTE, buffer.Data());

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

void XPlaneWindow::Brightness(int percent)
{
    if (percent < 0) percent = 0;
    else if (percent > 100) percent = 100;
    brightness = 0.1f + (0.9f * percent / 100.0f);
}

XPlaneWindow::WinPart* XPlaneWindow::LocateWinPart(int x, int y)
{
    if (y < TOOLBAR_HEIGHT) {
        return &winParts[WindowPart::TOOLBAR];
    }
    if ((x < MODEBAR_WIDTH) && (y < (TOOLBAR_HEIGHT + MODEBAR_HEIGHT))) {
        return &winParts[WindowPart::MODEBAR];
    }
    if (winParts[WindowPart::KEYPAD].active && (y >= (winHeight - KEYPAD_HEIGHT))) {
        return &winParts[WindowPart::KEYPAD];
    }
    if (winParts[WindowPart::DOODLER].active) {
        return &winParts[WindowPart::DOODLER];
    }
    return &winParts[WindowPart::CANVAS];
}


} // namespace navitab
