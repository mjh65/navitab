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
#include <iostream>
#include "navitab/core.h"
#include "navitab/simulator.h"
#include "navitab/window.h"
#include "navitab/logger.h"


int main(int arg, char** argv)
{
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<navitab::CoreServices> nvt;
    std::shared_ptr<navitab::Simulator> sim;
    std::shared_ptr<navitab::Window> win;
    try {
        // try to initialise logging and preferences - raises exception if fails
        LOG = std::make_unique<logging::Logger>("main");
        nvt = navitab::CoreServices::MakeNavitab(navitab::SimEngine::MSFS, navitab::AppClass::CONSOLE);
    }
    catch (navitab::StartupError& e) {
        std::cerr << "Navitab startup exception: " << e.What() << std::endl;
        exit(1);
    }
    catch (std::exception& e) {
        std::cerr << "General exception during startup: " << e.what() << std::endl;
        exit(1);
    }

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    LOGS("Early init completed, starting and enabling");
    nvt->Start();
    auto p = nvt->PrefsManager();
    sim = navitab::Simulator::Factory();
    sim->SetPrefs(p);
    sim->Connect(nvt->GetSimulatorCallbacks());
    win = navitab::Window::Factory();
    win->SetPrefs(p);
    win->Connect(nvt->GetWindowCallbacks());
    nvt->Enable();

    LOGS("Starting event loop");

    // TODO - in console mode we need to run an event loop

    LOGS("Event loop finished, disabling and stopping");

    nvt->Disable();
    win->Disconnect();
    win.reset();
    sim->Disconnect();
    sim.reset();
    nvt->Stop();
    nvt.reset();

    return 0;
}

