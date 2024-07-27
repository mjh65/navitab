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
#include "navitab/window.h"
#include "navitab/logger.h"

namespace navitab {
namespace xplane {

 // XPlaneSimulator extends simulator to allow the XPlane plugin to interact
 // directly without passing XPlane-specific interactions through the Navitab
 // core.

struct XPlaneSimulator : public Simulator {

    // Switching to/from VR mode
    virtual void onEnteringVR() = 0;
    virtual void onLeavingVR() = 0;

    // Changing to a new aircraft
    virtual void onPlaneLoaded() = 0;

    virtual ~XPlaneSimulator() = default;

};


} // namespace xplane
} // namespace navitab
