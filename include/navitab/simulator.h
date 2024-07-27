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

/*
 * This header file defines the interface to the simulator, which will
 * include access to position data, aircraft type, METAR, etc, and specific
 * filesystem locations where different types of document are expected to
 * be stored.
*/

namespace navitab {

class Preferences;

// The SimulatorEvents interface defines services that the simulator
// requires from the Navitab core. Calls to these services will generally
// be from the simulator's thread and should do minimal work.

struct SimulatorEvents
{

    // called from the simulator thread on each flight loop.
    virtual void onFlightLoop() = 0;

};


// The Simulator interface defines the services that the Simulator offers to 
// the application or Navitab core.

struct Simulator
{
    // Factory
    static std::shared_ptr<Simulator> New(std::shared_ptr<Preferences> p);

    // APIs called from the application/plugin
    // Start, enable, disable and stop events.
    virtual void Connect(std::shared_ptr<navitab::SimulatorEvents> core) = 0;
    virtual void Disconnect() = 0;

    // Things that Navitab might want to query from the simulation
    virtual int FrameRate() = 0;

    virtual ~Simulator() = default;

};

} // namespace navitab
