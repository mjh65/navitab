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

#include "coredoodler.h"
#include "navitab.h"

namespace navitab {

CoreDoodler::CoreDoodler(std::shared_ptr<Doodler2Core> c)
:   LOG(std::make_unique<logging::Logger>("doodler")),
    core(c),
    enabled(false)
{
}

CoreDoodler::~CoreDoodler()
{
}

void CoreDoodler::EnableDoodler()
{
    if (!enabled) {
        if (oldDoodle) std::swap(image, oldDoodle);
        enabled = true;
        RunLater([this]() { onResize(width, height); });
    }
}

void CoreDoodler::DisableDoodler()
{
    if (enabled) {
        std::swap(image, oldDoodle);
        enabled = false;
        RunLater([this]() { Redraw(); });
    }
}

void CoreDoodler::onResize(int w, int h)
{
    // if the doodler is resized then a new image is created, and if the doodler is enabled
    // the old image is copied into the new image

    auto ni = std::make_unique<FrameBuffer>(w, h);
    ni->Clear(backgroundPixels);
    // temporarily swap the old doodle into image if doodler is currently disabled
    if (!enabled) std::swap(image, oldDoodle);
    if (image) {
        for (auto y = 0; y < std::min(h, height); ++y) {
            auto sr = image->PixAt(y, 0);
            auto dr = ni->PixAt(y, 0);
            auto nx = std::min(w, width);
            std::copy(sr, sr + nx, dr);
        }
    }
    std::swap(image, ni);
    // and swap back after updating
    if (!enabled) std::swap(image, oldDoodle);
    width = w; height = h;

    dirtyBits.push_back(ImageRegion(0, 0, width, height));
    RunLater([this]() { Redraw(); });
}

void CoreDoodler::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::onKeyEvent(int c)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
