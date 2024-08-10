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

#include "coremodebar.h"
#include "navitab.h"

namespace navitab {

CoreModebar::CoreModebar(std::shared_ptr<Modebar2Core> c)
:   LOG(std::make_unique<logging::Logger>("modebar")),
    core(c)
{
}

CoreModebar::~CoreModebar()
{
}

void CoreModebar::SetHighlighted(int selectMask)
{
    UNIMPLEMENTED(__func__);
}

void CoreModebar::onResize(int, int)
{
    // if the modebar is resized then the previous image is just abandoned
    // and a new one is created and scheduled for redrawing
    width = Window::MODEBAR_WIDTH; height = Window::MODEBAR_HEIGHT;
    image = std::make_unique<FrameBuffer>(width, height);
    image->Clear(backgroundPixels);
    // this is temporary code during initial development
    for (int i = 1; i < 8; ++i) {
        int y = i * (Window::MODEBAR_HEIGHT / 8);
        auto r = image->Row(y);
        for (int x = 0; x < Window::MODEBAR_WIDTH; ++x) {
            *(r + x) = 0x400000ff;
        }
    }

    // TODO - generate the basic modebar image

    dirtyBits.push_back(FrameRegion(0, 0, width, height));
    RunLater([this]() { Redraw(); });
}

void CoreModebar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
