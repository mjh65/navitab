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
#include "canvas.h"
#include "../win/imagerect.h"

namespace navitab {

Canvas::Canvas(std::shared_ptr<CanvasEvents> c)
:   LOG(std::make_unique<logging::Logger>("canvas")),
    core(c),
    dirty(false)
{
}

void Canvas::Update()
{
    // TODO - this is just here for development and testing. of course it will get
    // replaced eventually!
    if (!image) return;
    // write random pixels
    for (int i = 0; i < 16; ++i) {
        auto y = rand() % height;
        auto x = rand() % width;
        // red in 7:0, green in 15:8, blue in 23:16, alpha in 31:24
        *(image->PixAt(y, x)) = (rand() % 0xff) + ((rand() % 0xff) << 8) + ((rand() % 0xff) << 16);
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

    dirty = true;
    core->AsyncCall([this]() { Redraw(); });
}

void Canvas::onResize(int w, int h)
{
    // During initial testing we're just generating random data.
    // If the canvas is resized then we create a new image and copy whatever fits
    // from the old one before dumping it.

    auto ni = std::make_unique<ImageRectangle>(w, h);
    ni->Clear(0xff701010);
    if (image) {
        for (auto y = 0; y < std::min(h, height); ++y) {
            auto sr = image->PixAt(y, 0);
            auto dr = ni->PixAt(y, 0);
            auto nx = std::min(w, width);
            std::copy(sr, sr + nx, dr);
        }
    }
    std::swap(image, ni);
    width = w; height = h;
    dirty = true;
    core->AsyncCall([this]() { Redraw(); });
}

void Canvas::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void Canvas::onWheelEvent(int x, int y, int xdir, int ydir)
{
    UNIMPLEMENTED(__func__);
}

void Canvas::onKeyEvent(int code)
{
    UNIMPLEMENTED(__func__);
}

void Canvas::AsyncCall(std::function<void()> f)
{
    core->AsyncCall(f);
}

void Canvas::Redraw()
{
    if (!image || !dirty) return;
    dirty = false;
    image = painter->RefreshPart(Window::PART_CANVAS, std::move(image));
    if (!image) {
        image = std::make_unique<ImageRectangle>(width, height);
        image->Clear(0xff701010);
    }

}

} // namespace navitab
