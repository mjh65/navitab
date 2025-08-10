/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <lvgl.h>
#include "toolkit.h"

namespace lvglkit {

class DisplayWrapper : public Display, protected RefCounter
{
public:
    DisplayWrapper(Updater*);
    ~DisplayWrapper();

    void Resize(int w, int h, uint32_t* buffer) override;
    lv_display_t* GetHandleLVGL() override { return display; }

    void FlushCallback(const lv_area_t* area, uint32_t* px);

    void Select() const override { lv_disp_set_default(display); }

private:
    Updater* const updater;
    lv_display_t* display;
    int width, height;
    uint32_t* buffer;
};


} // namespace lvglkit
