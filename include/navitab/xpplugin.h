/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/simulator.h"
#include <memory>

 /*
  * This header file defines the interface to the simulator, which will
  * include access to position data, aircraft type, METAR, etc, and specific
  * filesystem locations where different types of document are expected to
  * be stored.
 */

namespace navitab {

// XPlaneSimulator extends simulator with XP-specific behaviours that allow the
// XPlane plugin to interact directly without routing through the Navitab core.

struct XPlaneSimulator : public Simulator
{
    // Factory
    static std::shared_ptr<XPlaneSimulator> Factory();

    // Additional APIs called from the plugin.
    // Start, enable, disable and stop events corresponding to the plugin APIs
    virtual void Start() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;
    virtual void Stop() = 0;

    // Switching to/from VR mode
    virtual void onVRmodeChange(bool entering) = 0;

    // Changing to a new aircraft
    virtual void onPlaneLoaded() = 0;

    virtual ~XPlaneSimulator() = default;
};

} // namespace navitab
