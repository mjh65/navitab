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
#include "svg/mode_about_40x40.h"
#include "svg/mode_map_40x40.h"
#include "svg/mode_route_40x40.h"
#include "svg/mode_airport_40x40.h"
#include "svg/mode_docs_40x40.h"
#include "svg/mode_settings_40x40.h"
#include "svg/mode_doodler_40x40.h"
#include "svg/mode_keypad_40x24.h"

namespace navitab {

CoreModebar::CoreModebar(std::shared_ptr<Modebar2Core> c, std::shared_ptr<lvglkit::Manager> u)
:   LOG(std::make_unique<logging::Logger>("modebar")),
    core(c), uiMgr(u)
{
    uiDisplay = uiMgr->MakeDisplay(this);
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
    //image->Clear(backgroundPixels);

    // generate the basic modebar image
    const uint32_t* icons[] = {
        mode_about_40x40,
        mode_map_40x40,
        mode_route_40x40,
        mode_airport_40x40,
        mode_docs_40x40,
        mode_settings_40x40,
        mode_doodler_40x40
    };
    for (int i = 0; i < 7; ++i) {
        int y = i * 40;
        image->PaintIcon(0, i*40, icons[i], 40, 40);
    }
    image->PaintIcon(0, 7 * 40, mode_keypad_40x24, 40, 24);

    dirtyBits.push_back(FrameRegion(0, 0, width, height));
    RunLater([this]() { Redraw(); });
}

void CoreModebar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void CoreModebar::Update(navitab::FrameRegion r, uint32_t* pixels)
{
    // this is the update function called from the LVGL library
    // TODO - as we're using LV_DISP_RENDER_MODE_DIRECT, there is probably not much to be done
    // maybe just post the region to the dirtyBits and redraw?
    UNIMPLEMENTED(__func__);
    dirtyBits.push_back(r);
    RunLater([this]() { Redraw(); });
}

} // namespace navitab
