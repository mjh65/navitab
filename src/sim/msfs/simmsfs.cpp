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

#include "simmsfs.h"

namespace navitab {

std::shared_ptr<Simulator> navitab::Simulator::GetSimulator(std::shared_ptr <SimulatorCallbacks> core, std::shared_ptr<Preferences> prefs)
{
    return std::make_shared<sim::SimMsfs>(core);
}

namespace sim {

SimMsfs::SimMsfs(std::shared_ptr<SimulatorCallbacks> c)
:   core(c)
{
}

SimMsfs::~SimMsfs()
{
}

void SimMsfs::Enable()
{
}

void SimMsfs::Disable()
{
}

} // namespace sim
} // namespace navitab