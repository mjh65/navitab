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
#include "navitab/logger.h"


int main(int arg, char** argv)
{
    std::shared_ptr<navitab::System> nvt;
    try {
        // try to initialise logging and preferences - raises exception if fails
        nvt = navitab::System::GetSystem(navitab::SimEngine::MOCK, navitab::AppClass::CONSOLE);
    }
    catch (navitab::StartupError& e) {
        // TODO - report anything we can to stderr and then exit
    }
    catch (...) {
        // TODO - handle other exceptions
    }

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    auto LOG = std::make_unique<logging::Logger>("main");

    LOGS("Early init completed, starting and enabling");
    nvt->Start();
    nvt->Enable();

    LOGS("Starting event loop");

    // TODO - in desktop mode we will handover to GL to run the GUI

    LOGS("Event loop finished, disabling and stopping");
    nvt->Disable();
    nvt->Stop();

    return 0;
}
