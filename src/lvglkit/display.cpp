/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
    // as we're using LV_DISP_RENDER_MODE_DIRECT, there is not much to be done, just signal the update
    updater->Update(navitab::ImageRegion(area->x1, area->y1, area->x2, area->y2), px);
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
