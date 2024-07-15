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

// This file provides the wWinMain function required for the WIN32 desktop
// build of Navitab. It does the required generic initialisation of the
// Navitab components, and if all is successful starts the standard Win32
// event loop and message processing. If there is a problem during startup
// before 

#include <memory>
#include "navitab/core.h"

int main(int argc, char **argv)
{
    auto nvt = std::make_unique<navitab::core::SubSystems>(navitab::core::Simulation::NONE, navitab::core::AppClass::DESKTOP);

    try {
        // try to initialise logging and preferences - raises exception if fails
        nvt->early_init();
    }
    catch (...) {
        // TODO - if an exception occurs then we should show report information we got
        // to stderr
    }



    return 0;
}
