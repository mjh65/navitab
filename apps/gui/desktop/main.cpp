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

// This file provides the main function for the desktop variant of Navitab.
// It does the required generic initialisation of the Navitab components,
// and if all is successful starts a GL window manager for the UI.
// It's used on Linux and Mac.

#include <memory>
#include "navitab/core.h"
#include "navitab/simulator.h"
#include "navitab/logger.h"


int main(int arg, char** argv)
{
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<navitab::System> nvt;
    std::shared_ptr<navitab::Simulator> sim;
    try {
        // try to initialise logging and preferences - raises exception if fails
        LOG = std::make_unique<logging::Logger>("main");
        nvt = navitab::System::GetSystem(navitab::SimEngine::MOCK, navitab::AppClass::DESKTOP);
    }
    catch (navitab::StartupError& e) {
        // TODO - report anything we can to stderr and then exit
    }
    catch (...) {
        // TODO - handle other exceptions
    }

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    LOGS("Early init completed, starting and enabling");
    nvt->Start();
    sim = navitab::Simulator::Factory();
    sim->SetPrefs(nvt->PrefsManager());
    sim->Connect(nvt->SetSimulator(sim));
    nvt->Enable();

    LOGS("Starting event loop");

    // TODO - in desktop mode we will handover to GL to run the GUI

    LOGS("Event loop finished, disabling and stopping");
    nvt->Disable();
    sim->Disconnect();
    sim.reset();
    nvt->Stop();
    nvt.reset();    // Navitab core will now shutdown gracefully

    return 0;
}
