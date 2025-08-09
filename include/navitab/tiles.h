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

#include "navitab/window.h"
#include <cstdint>
#include <memory>

// This header file defines the interface for raster tiles which can be painted
// into the LVGL canvases, and the interface for subsystems that provide these
// tiles.

namespace navitab {

class RasterTile : public ImageBuffer
{
public:
    static const unsigned DefaultWidth = 256;
    static const unsigned DefaultHeight = 256;

    RasterTile(unsigned w, unsigned h) : ImageBuffer(w, h) {}
    RasterTile() : ImageBuffer(DefaultWidth, DefaultHeight) {}

    virtual ~RasterTile() = default;
};

} // namespace navitab
