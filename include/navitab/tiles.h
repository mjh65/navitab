/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
