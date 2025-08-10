/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/logger.h"

// This header file defines the interface for the imaging kit, which provides
// services to draw various forms of document into pixel buffers that are then
// used as tiles for drawing into the app canvas..

namespace navitab {

class ImagingKit
{
public:
    ImagingKit();

    virtual ~ImagingKit() = default;

private:
    std::unique_ptr<logging::Logger> LOG;
};

} // namespace navitab
