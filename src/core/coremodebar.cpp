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
#include "../win/imagerect.h"

namespace navitab {

CoreModebar::CoreModebar(std::shared_ptr<ModebarEvents> c)
:   core(c),
    LOG(std::make_unique<logging::Logger>("modebar"))
{
}

CoreModebar::~CoreModebar()
{
}

void CoreModebar::SetHighlights(int selectMask)
{
    UNIMPLEMENTED(__func__);
}

void CoreModebar::AsyncCall(std::function<void()> f)
{
    core->AsyncCall(f);
}

void CoreModebar::onResize(int w, int h)
{
    dirty = true;
    core->AsyncCall([this]() { Redraw(); });
}

void CoreModebar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void CoreModebar::Redraw()
{
    if (!image) {
        // a complete redraw is required
        image = std::make_unique<ImageRectangle>(Window::MODEBAR_WIDTH, Window::MODEBAR_HEIGHT);
        image->Clear(0x40f0f0f0);
        dirty = true;
    }

    if (!dirty) return;

    // TODO - do the proper drawing work here (selected mode)
    // but meanwhile let's draw a grid of where the mode selectors will go
    for (int i = 1; i < 8; ++i) {
        int y = i * (Window::MODEBAR_HEIGHT / 8);
        auto r = image->Row(y);
        for (int x = 0; x < Window::MODEBAR_WIDTH; ++x) {
            *(r + x) = 0x400000ff;
        }
    }

    dirty = false;

    // do the image buffer swap with the window
    image = painter->RefreshPart(Window::PART_MODEBAR, std::move(image));
}

} // namespace navitab
