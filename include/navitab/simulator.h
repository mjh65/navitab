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

#include <memory>
#include <functional>
#include <vector>
#include "navitab/deferred.h"
#include "navitab/navigation.h"

/*
 * This header file defines the interface to the simulator, which will
 * include access to position data, aircraft type, METAR, etc, and specific
 * filesystem locations where different types of document are expected to
 * be stored.
*/

namespace navitab {

struct CoreServices;
class Settings;
struct Simulator;

struct SimStateData
{
    AircraftPosition    myPlane;
    size_t              nOtherPlanes;
    AircraftPosition    otherPlanes[MAX_OTHER_AIRCRAFT];
    int                 zuluTime;
    int                 fps;
};

// The Simulator2Core interface defines services that the simulator
// requires from the Navitab core. Calls to these services will generally
// be from the simulator's thread and should do minimal work.

struct Simulator2Core : public DeferredJobRunner
{
    // Called from the simulator on each flight loop, and provides updates
    // to simulation-derived data. Double-buffered in sim, but not mutex protected.
    void PostSimUpdates(const SimStateData &data) {
        RunLater([this, data]() { onSimFlightLoop(data); });
    }

protected:
    virtual void onSimFlightLoop(const SimStateData& data) = 0;

};


// The Simulator interface defines the services that the Simulator offers to 
// the application/plugin.

struct Simulator
{
    // Factory, implemented in each simulator library.
    static std::shared_ptr<Simulator> Factory();

    // APIs called from the application/plugin
    virtual void Connect(std::shared_ptr<CoreServices> core) = 0;
    virtual void Disconnect() = 0;

    virtual ~Simulator() = default;

};

} // namespace navitab
