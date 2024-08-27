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
#include "../texbuffer.h"
#include "imageserver.h"

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowHTTP>();
}

namespace navitab {

WindowHTTP::WindowHTTP()
:   LOG(std::make_unique<logging::Logger>("winhttp")),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT),
    brightness(1.0f)
{
    server = std::make_unique<PanelServer>(this);
    image = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, (WIN_MAX_HEIGHT - TOOLBAR_HEIGHT));
}

WindowHTTP::~WindowHTTP()
{
}

void WindowHTTP::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->GetSettingsManager();
    canvas = core->GetPartCallbacks(WindowPart::CANVAS);
    canvas->SetPainter(shared_from_this());
    canvas->PostResize(winWidth - MODEBAR_WIDTH, winHeight - TOOLBAR_HEIGHT);
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
    server->stop();
    canvas.reset();
    prefs.reset();
    core.reset();
}

int WindowHTTP::EventLoop(int maxLoops)
{
    // TODO - deal with events received from the panel via the web server
    auto k = server->key();
    if (k == 0) {
        return -1;
    }
    // TODO - perhaps implement some kind of command interface here?

    return 0;
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

unsigned WindowHTTP::encodeBMP(std::vector<unsigned char> &bmp)
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
        0x20, 0x03, 0x00, 0x00,     // width in pixels (800)
        0xe0, 0x01, 0x00, 0x00,     // height in pixels (480)
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
    *(reinterpret_cast<uint32_t *>(&bmp[2])) = bmpLength;
    *(reinterpret_cast<uint32_t *>(&bmp[18])) = w;
    *(reinterpret_cast<uint32_t *>(&bmp[22])) = h;
    *(reinterpret_cast<uint32_t *>(&bmp[34])) = n;

    // copy the pixel data
    memcpy(bmp.data() + 14 + 40, image->Data(), n);
    
    return 0;
}

} // namespace navitab
