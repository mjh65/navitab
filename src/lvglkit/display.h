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
    void DevTesting() override;

    void FlushCallback(const lv_area_t* area, uint32_t* px);

private:
    Updater* const updater;
    lv_display_t* display;
    int width, height;
    uint32_t* buffer;
};


} // namespace lvglkit
