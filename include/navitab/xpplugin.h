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
