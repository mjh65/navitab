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

#include <algorithm>
#include "appcanvas.h"

namespace navitab {

AppCanvas::AppCanvas(std::shared_ptr<AppCanvas2Core> c, std::shared_ptr<lvglkit::Manager> u)
:   WindowPart(CANVAS),
    LOG(std::make_unique<logging::Logger>("canvas")),
    core(c), uiMgr(u)
{
    uiDisplay = uiMgr->MakeDisplay(this);
}

AppCanvas::~AppCanvas()
{
}

void AppCanvas::UpdateProtoDevelopment()
{
    // TODO - this is just here for development and testing. of course it will get
    // replaced eventually!
    if (!image) return;
    // write random pixels
    for (int i = 0; i < 16; ++i) {
        auto y = rand() % height;
        auto x = rand() % width;
        // red in 7:0, green in 15:8, blue in 23:16, alpha in 31:24
        *(image->PixAt(y, x)) = (rand() % 0xff) + ((rand() % 0xff) << 8) + ((rand() % 0xff) << 16) + (0xff << 24);
    }
#if 0
    else {
        // draw one of our generated SVG icons to test the generator
        auto y0 = rand() % (canvas.Height() - sample_64x64_HEIGHT);
        auto x0 = rand() % (canvas.Width() - sample_64x64_WIDTH);
        for (int y = 0; y < sample_64x64_HEIGHT; ++y) {
            for (int x = 0; x < sample_64x64_WIDTH; ++x) {
                auto si = y * sample_64x64_WIDTH + x;
                auto di = (y + y0) * canvas.Width() + (x + x0);
                canvas.imageBuffer[di] = sample_64x64[si];
            }
        }
    }
#endif
    dirtyBits.push_back(ImageRegion(0, 0, width, height));
    core->RunLater([this]() { Redraw(); });
}

void AppCanvas::onResize(int w, int h)
{
    bool firstTime = (!image);

    // On resizing we just create a new image buffer
    image = std::make_unique<FrameBuffer>(w, h);
    width = w; height = h;

    // The canvas is drawn by the LVGL UI. Resize the display associated with the canvas.
    uiDisplay->Resize(w, h, image->Row(0));

    // On first resize the navitab apps need to be started
    if (firstTime) {
        core->StartApps();
    }

    dirtyBits.push_back(ImageRegion(0, 0, width, height));
    RunLater([this]() { Redraw(); });
}

void AppCanvas::Update(navitab::ImageRegion r, uint32_t* pixels)
{
    // this is the update function called from the LVGL library
    // TODO - as we're using LV_DISP_RENDER_MODE_DIRECT, there is probably not much to be done
    // maybe just post the region to the dirtyBits and redraw?
    dirtyBits.push_back(r);
    RunLater([this]() { Redraw(); });
}

void AppCanvas::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void AppCanvas::onWheelEvent(int x, int y, int xdir, int ydir)
{
    UNIMPLEMENTED(__func__);
}

void AppCanvas::onKeyEvent(int code)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
