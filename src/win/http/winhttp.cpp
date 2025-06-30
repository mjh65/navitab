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
    activeTools(0),
    activeRepeaters(0)
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
    activeTools = selectMask;
}

void WindowHTTP::SetRepeatingTools(int selectMask)
{
    activeRepeaters = selectMask;
}

void WindowHTTP::SetHighlightedModes(int selectMask)
{
    activeModes = selectMask;
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

void WindowHTTP::EncodeBMP(std::vector<unsigned char> &bmp)
{
    std::lock_guard<std::mutex> lock(paintMutex);
    uint32_t w = image->Width();
    uint32_t h = image->Height();
    uint32_t ncanvas = (4 * w * h);
    uint32_t bmpLength = 14 + 40 + ncanvas;
    
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
    *(reinterpret_cast<uint32_t *>(&bmp[2])) = bmpLength;   // blob length
    *(reinterpret_cast<uint32_t *>(&bmp[18])) = w;          // width in pixels
    *(reinterpret_cast<uint32_t *>(&bmp[22])) = h;          // height in pixels
    *(reinterpret_cast<uint32_t *>(&bmp[34])) = ncanvas;    // image size in bytes

    // BMP's are interpreted bottom to top. Since MSFS doesn't support the convention
    // that a negative height indicates top-to-bottom we need to copy each row into
    // it's mirror.
    const unsigned int rl = 4 * w;
    const unsigned char *sr = reinterpret_cast<const unsigned char *>(image->Data());
    unsigned char* dr = bmp.data() + 14 + 40 + ncanvas - rl;
    for (int i = 0; i < h; ++i) {
        memcpy(dr, sr, rl);
        sr += rl;
        dr -= rl;
    }
}

void WindowHTTP::EncodeStatus(std::vector<unsigned char> &status)
{
    int zth = zuluTime / (60 * 60);
    int ztm = (zuluTime / 60) % 60;
    int zts = zuluTime % 60;
    int ew = (int)((loc.longitude + 180) * 1000);
    int ns = (int)((loc.latitude + 90) * 1000);
    std::string s = fmt::format("{:02d}{:02d}{:02d}{:02d}{:06d}{:06d}", zth, ztm, zts, fps, ew, ns);

    // append the status of currently active modes, tools and which are 'repeaters'
    int activeApp, overlays = 0;
    for (auto i = 0; i < 6; ++i) {
        if (activeModes & (1 << i)) {
            activeApp = i;
            break;
        }
    }
    if (activeModes & (1 << 6)) overlays += 1;
    if (activeModes & (1 << 7)) overlays += 2;
    s += fmt::format("M{}{}", activeApp, overlays);
    s += fmt::format("T{:08d}", activeTools);
    s += fmt::format("R{:08d}", activeRepeaters);

    status.resize(0);
    status.insert(status.end(), s.begin(), s.end());
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
    toolbarClient->PostToolClick((ClickableTool)t);
}

} // namespace navitab
