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

#include <fmt/core.h>
#include "coretoolbar.h"
#include "navitab.h"

namespace navitab {

CoreToolbar::CoreToolbar(std::shared_ptr<Toolbar2Core> c, std::shared_ptr<lvglkit::Manager> u)
:   LOG(std::make_unique<logging::Logger>("toolbar")),
    core(c), uiMgr(u),
    label(0)
{
    uiDisplay = uiMgr->MakeDisplay(this);
}

CoreToolbar::~CoreToolbar()
{
}

void CoreToolbar::SetStausInfo(std::string s)
{
    lv_label_set_text(label, s.c_str());
}

void CoreToolbar::SetEnabledTools(int selectMask)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::onResize(int w, int)
{
    width = w; height = Window::TOOLBAR_HEIGHT;

    // If the toolbar is resized then the previous image is just abandoned
    // and a new one is created. On the first resize notification the UI
    // widgets are created (using raw LVGL API - no wrappers!)
    image = std::make_unique<FrameBuffer>(width, height);
    uiDisplay->Resize(width, height, image->Row(0));
    if (!label) {
        CreateWidgets();
    }
}

void CoreToolbar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::Update(navitab::FrameRegion r, uint32_t* pixels)
{
    // this is the update function called from the LVGL library
    UNIMPLEMENTED(__func__);
    dirtyBits.push_back(r);
    RunLater([this]() { Redraw(); });
}

void CoreToolbar::CreateWidgets()
{
    lv_display_set_default(uiDisplay->GetHandleLVGL());

    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(backgroundPixels), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
}

} // namespace navitab
