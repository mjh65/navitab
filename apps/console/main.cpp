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

int main(int arg, char** argv)
{
    auto nvt = std::make_unique<navitab::core::SubSystems>(navitab::core::Simulation::NONE, navitab::core::AppClass::CONSOLE);

    try {
        // try to initialise logging and preferences - raises exception if fails
        nvt->early_init();
    }
    catch (...) {
        // if an exception occurs then we extract what we can and dump it to stderr
        // and then exit.


    }

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    nvt->full_init();



}
