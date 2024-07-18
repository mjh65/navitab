/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
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

// This file provides the main function for the console variant of Navitab.
// It does the required generic initialisation of the Navitab components,
// and if all is successful emulates an event loop that either runs a script
// or just does random stuff.

#include <memory>
#include "navitab/core.h"
#include "navitab/logging.h"


int main(int arg, char** argv)
{
    std::shared_ptr<navitab::core::Navitab> nvt;
    try {
        // try to initialise logging and preferences - raises exception if fails
        nvt = std::make_unique<navitab::core::Navitab>(navitab::core::Simulation::NONE, navitab::core::AppClass::CONSOLE);
    }
    catch (navitab::core::StartupError& e) {
        // TODO - report anything we can to stderr and then exit
    }
    catch (...) {
        // TODO - handle other exceptions
    }

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    auto logger = std::make_unique<navitab::logging::Logger>("main");
    auto LOG = (*logger);

    zSTATUS(LOG, "Early init completed, starting and enabling");
    nvt->start();
    nvt->enable();

    zSTATUS(LOG, "Starting event loop");

    // TODO - in console mode we need to run an event loop

    zSTATUS(LOG, "Event loop finished, disabling and stopping");
    nvt->disable();
    nvt->stop();

    return 0;
}
