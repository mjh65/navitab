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
    statusInfoText(0)
{
    uiDisplay = uiMgr->MakeDisplay(this);
}

CoreToolbar::~CoreToolbar()
{
}

void CoreToolbar::SetStausInfo(std::string s)
{
    lv_label_set_text(statusInfoText, s.c_str());
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
    if (!statusInfoText) {
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

    // Change the background colour
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(backgroundPixels), LV_PART_MAIN);

    // Create the status info text widget
    statusInfoText = lv_label_create(lv_screen_active());
    lv_label_set_text(statusInfoText, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(statusInfoText, LV_ALIGN_LEFT_MID, 6, 0);

    // Create the toolbar image buttons
    // TODO create our own images - will be reused in MSFS panel
    // TODO use the LVGL alignment grid
    lv_obj_t* img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, LV_SYMBOL_OK);
    lv_obj_align(img1, LV_ALIGN_RIGHT_MID, -6, 0);

    lv_obj_t* img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, LV_SYMBOL_CLOSE);
    lv_obj_align(img2, LV_ALIGN_RIGHT_MID, -30, 0);

    lv_obj_t* img3 = lv_image_create(lv_screen_active());
    lv_image_set_src(img3, LV_SYMBOL_PLUS);
    lv_obj_align(img3, LV_ALIGN_RIGHT_MID, -54, 0);

    lv_obj_t* img4 = lv_image_create(lv_screen_active());
    lv_image_set_src(img4, LV_SYMBOL_MINUS);
    lv_obj_align(img4, LV_ALIGN_RIGHT_MID, -78, 0);

}

} // namespace navitab
