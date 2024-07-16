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

#include "navitab/core.h"

namespace navitab {
namespace core {


#if defined(NAVITAB_WINDOWS)
static navitab::core::HostPlatform host = navitab::core::HostPlatform::WIN;
#elif defined(NAVITAB_MACOSX)
navitab::core::HostPlatform host = navitab::core::HostPlatform::WIN;
#elif defined(NAVITAB_LIUNX)
navitab::core::HostPlatform host = navitab::core::HostPlatform::WIN;
#endif


Navitab::Navitab(Simulation s, AppClass c)
:   mHost(host),
    mSim(s),
    mAppClass(c)
{

}

Navitab::~Navitab()
{

}

void Navitab::early_init()
{

}

void Navitab::init()
{

}

} // namespace core
} // namespace navitab
