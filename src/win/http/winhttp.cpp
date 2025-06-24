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

#include <cassert>
#include <memory>
#include <fmt/core.h>
#include "winhttp.h"
#include "navitab/core.h"
#include "navitab/platform.h"
#include "../texbuffer.h"
#include "htmlserver.h"
#include "cmdhandler.h"

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowHTTP>();
}

namespace navitab {

WindowHTTP::WindowHTTP()
:   LOG(std::make_unique<logging::Logger>("winhttp")),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT),
    brightness(1.0f),
    running(true),
    activeModes(0),
    pendingModes(0),
    activeTools(0),
    pendingTools(0),
    activeRepeaters(0),
    pendingRepeaters(0)
{
    server = std::make_unique<HtmlServer>(this);
    image = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, (WIN_MAX_HEIGHT - TOOLBAR_HEIGHT));
    commands = std::make_unique< CommandHandler>(this);
}

WindowHTTP::~WindowHTTP()
{
}

void WindowHTTP::Connect(std::shared_ptr<CoreServices> c)
{
    // this connects the WinHTTP server to the Navitab core services,
    // it doesn't have anything to do with the HTTP panel connecting.
    core = c;
    core->SetWindowControl(shared_from_this());
    prefs = core->GetSettingsManager();
    toolbarClient = core->SetToolbar(shared_from_this());
    modebarClient = core->SetModebar(shared_from_this());
    doodlerClient = core->SetDoodler(shared_from_this());
    keypadClient = core->SetKeypad(shared_from_this());
    canvas = core->GetCanvas();
    canvas->SetPainter(shared_from_this());
    canvas->PostResize(winWidth, winHeight - TOOLBAR_HEIGHT);
    int port = 26730; // base port
    while (server->start(port)) {
        if (++port >= 26750) break;
    }
    if (port < 26750) {
        LOGS(fmt::format("Panel server started on port {}", port));
    } else {
        LOGE("Panel server did not start on any candidate port");
    }
}

void WindowHTTP::Disconnect()
{
    // this disconnects the WinHTTP server from the Navitab core,
    // it doesn't have anything to do with the HTTP panel disconnecting.
    server->stop();
    canvas.reset();
    keypadClient.reset();
    doodlerClient.reset();
    modebarClient.reset();
    toolbarClient.reset();
    prefs.reset();
    core.reset();
}

void WindowHTTP::EventLoop()
{
    while (1) {
        std::unique_lock<std::mutex> lock(qmutex);
        qsync.wait(lock, [this]() { return !running || !jobs.empty(); });
        if (!running) break;
        auto job = jobs.front();
        jobs.pop();
        lock.unlock();

        // run the job
        job();
    }
}

void WindowHTTP::Paint(int part, const FrameBuffer* src, const std::vector<FrameRegion>& regions)
{
    assert(part == WindowPart::CANVAS);

    // This function is called from the core thread.
    const std::lock_guard<std::mutex> lock(paintMutex);
    if ((image->Width() != src->Width()) || (image->Height() != src->Height())) {
        image->Resize(src->Width(), src->Height());
    }
    image->CopyRegionsFrom(src, regions);
}

void WindowHTTP::Brightness(int percent)
{
    UNIMPLEMENTED(__func__);
}

void WindowHTTP::SetStausInfo(int zt, int f, const Location& l)
{
    zuluTime = zt;
    fps = f;
    loc = l;
}

void WindowHTTP::SetActiveTools(int selectMask)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", selectMask));
    pendingTools = selectMask;
}

void WindowHTTP::SetRepeatingTools(int selectMask)
{
    pendingRepeaters = selectMask;
}

void WindowHTTP::SetHighlightedModes(int selectMask)
{
    pendingModes = selectMask;
}

void WindowHTTP::EnableDoodler()
{
    UNIMPLEMENTED(__func__);
}

void WindowHTTP::DisableDoodler()
{
    UNIMPLEMENTED(__func__);
}

void WindowHTTP::ShowKeypad()
{
    UNIMPLEMENTED(__func__);
}

void WindowHTTP::HideKeypad()
{
    UNIMPLEMENTED(__func__);
}

void WindowHTTP::RunLater(std::function<void()> j, void*)
{
    {
        std::lock_guard<std::mutex> lock(qmutex);
        jobs.push(j);
    }
    qsync.notify_one();
}

void WindowHTTP::RunLater(std::function<void()> j, int*)
{
    void* x = nullptr;
    RunLater(j,x);
}

unsigned WindowHTTP::EncodeBMP(std::vector<unsigned char> &bmp)
{
    std::lock_guard<std::mutex> lock(paintMutex);
    uint32_t w = image->Width();
    uint32_t h = image->Height();
    uint32_t n = (4 * w * h);
    uint32_t bmpLength = 14 + 40 + n;
    
    bmp.resize(bmpLength);
    
    static const unsigned char hdr[] = {
        0x42, 0x4d,                 // signature
        0x36, 0x94, 0x11, 0x00,     // file length
        0x00, 0x00,                 // res1
        0x00, 0x00,                 // res2
        0x36, 0x00, 0x00, 0x00,     // offset of pixel map
        0x28, 0x00, 0x00, 0x00,     // length of DIB header
        0x20, 0x03, 0x00, 0x00,     // width in pixels
        0xe0, 0x01, 0x00, 0x00,     // height in pixels
        0x01, 0x00,                 // # colour planes (1)
        0x20, 0x00,                 // # bits per pixel (32)
        0x00, 0x00, 0x00, 0x00,     // compression method
        0x00, 0x94, 0x11, 0x00,     // image size in bytes
        0xc3, 0x0e, 0x00, 0x00,     // horizontal resolution (pixels/m)
        0xc3, 0x0e, 0x00, 0x00,     // vertical resolution (pixels/m)
        0x00, 0x00, 0x00, 0x00,     // # colours in palette
        0x00, 0x00, 0x00, 0x00      // # important colours in palette
    };
    
    // copy header template and then overwrite width, height and derived fields
    memcpy(bmp.data(), hdr, sizeof(hdr));
    *(reinterpret_cast<uint32_t *>(&bmp[2])) = bmpLength;   // file length
    *(reinterpret_cast<uint32_t *>(&bmp[18])) = w;          // width in pixels
    *(reinterpret_cast<int32_t *>(&bmp[22])) = 0 - h;       // height in pixels (negative to draw top to bottom)
    *(reinterpret_cast<uint32_t *>(&bmp[34])) = n;          // image size in bytes

    // copy the pixel data
    memcpy(bmp.data() + 14 + 40, image->Data(), n);
    
    return 0;
}

std::string WindowHTTP::EncodeStatus()
{
    int zth = zuluTime / (60 * 60);
    int ztm = (zuluTime / 60) % 60;
    int zts = zuluTime % 60;
    int ew = (int)((loc.longitude + 180) * 1000);
    int ns = (int)((loc.latitude + 90) * 1000);
    return fmt::format("{:02d}{:02d}{:02d}{:02d}{:06d}{:06d}", zth, ztm, zts, fps, ew, ns);
}

std::string WindowHTTP::EncodeControls()
{
    std::string cs;
#if 0
    // some temporary code to vary control settings until everything is hooked up
    static char nextModeChange = '0' + (rand() % 10);
    static char nextToolChange = '0' + (rand() % 10);
    auto t = navitab::LocalTime("%S");
    if (t[1] == nextModeChange) {
        cs += fmt::format("M{}{}", rand() % 6, rand() % 4);
        nextModeChange += (nextModeChange < '3') ? 7 : -3;
    }
    if (t[1] == nextToolChange) {
        uint32_t te = ((rand() % 0x100) << 16) + (rand() % 0x10000);
        cs += fmt::format("T{:08d}", te);
        nextToolChange += (nextToolChange < '5') ? 5 : -5;
    }
#endif
    if (pendingModes != activeModes) {
        int activeApp, overlays = 0;
        for (auto i = 0; i < 6; ++i) {
            if (pendingModes & (1 << i)) {
                activeApp = i;
                break;
            }
        }
        if (pendingModes & (1 << 6)) overlays += 1;
        if (pendingModes & (1 << 7)) overlays += 2;
        cs += fmt::format("M{}{}", activeApp, overlays);
        activeModes = pendingModes;
    }
    return cs;
}

void WindowHTTP::mouseEvent(int x, int y, int b)
{
    canvas->PostMouseEvent(x, y, b, false);
}

void WindowHTTP::wheelEvent(int x, int y, int d)
{
    canvas->PostWheelEvent(x, y, 0, d);
}

void WindowHTTP::panelResize(int w, int h)
{
    w = std::max(std::min(w, (int)WIN_MAX_WIDTH), (int)WIN_MIN_WIDTH);
    h = std::max(std::min(h, (int)(WIN_MAX_HEIGHT - TOOLBAR_HEIGHT)), (int)WIN_MIN_HEIGHT);
    LOGS(fmt::format("Resizing to {} x {}", w, h));
    canvas->PostResize(w, h);
}

void WindowHTTP::modebarIconSelect(int m)
{
    Modebar2Core::Mode mode = (Modebar2Core::Mode)(1 << m);
    if (mode == Modebar2Core::Mode::DOODLER) {
        modebarClient->PostDoodlerToggle();
    } else if (mode == Modebar2Core::Mode::KEYPAD) {
        modebarClient->PostKeypadToggle();
    } else {
        modebarClient->PostAppSelect(mode);
    }
}

void WindowHTTP::toolClick(int t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", t));
}

} // namespace navitab
