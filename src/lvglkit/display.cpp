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

#include <cassert>
#include "display.h"
#include "navitab/core.h"
#include "navitab/window.h"

static void flush_callback(lv_display_t* d, const lv_area_t* area, unsigned char* px)
{
    auto dw = reinterpret_cast<lvglkit::DisplayWrapper*>(lv_display_get_user_data(d));
    assert(dw);
    dw->FlushCallback(area, reinterpret_cast<uint32_t*>(px));
    lv_display_flush_ready(d);
}

namespace lvglkit {

DisplayWrapper::DisplayWrapper(Updater *u)
:   updater(u),
    display(0),
    width(0), height(0)
{
}

DisplayWrapper::~DisplayWrapper()
{
    if (display) {
        lv_display_delete(display);
        display = 0;
    }
}

void DisplayWrapper::FlushCallback(const lv_area_t* area, uint32_t* px)
{
    assert(updater);
    // TODO - as we're using LV_DISP_RENDER_MODE_DIRECT, there is probably not much to be done, just signal the update
    updater->Update(navitab::FrameRegion(area->x1, area->y1, area->x2, area->y2), px);
}

void DisplayWrapper::Resize(int w, int h, uint32_t* buffer)
{
    // the first time this is called it triggers the initialisation
    if (!display) {
        if (!(display = lv_display_create(w, h))) {
            throw navitab::LogFatal("Could not create LVGL display");
        }
        lv_display_set_user_data(display, this);
        lv_display_set_flush_cb(display, flush_callback);
    } else {
        lv_display_set_resolution(display, w, h);
    }
    lv_display_set_buffers(display, buffer, nullptr, w * h * sizeof(uint32_t), LV_DISP_RENDER_MODE_DIRECT);
    lv_obj_invalidate(lv_scr_act());
}

} // namespace lvglkit
