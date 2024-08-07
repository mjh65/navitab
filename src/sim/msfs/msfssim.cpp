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

#include "msfssim.h"

std::shared_ptr<navitab::Simulator> navitab::Simulator::Factory()
{
    return std::make_shared<navitab::MsfsSimulator>();
}

namespace navitab {

MsfsSimulator::MsfsSimulator()
:   LOG(std::make_unique<logging::Logger>("msfssim"))
{
}

MsfsSimulator::~MsfsSimulator()
{
}

void MsfsSimulator::SetPrefs(std::shared_ptr<Preferences> p)
{
    prefs = p;
}

void MsfsSimulator::Connect(std::shared_ptr<SimulatorEvents> c)
{
    core = c;
    running = true;
    worker = std::make_unique<std::thread>([this]() { AsyncRunSimulator(); });
}

void MsfsSimulator::Disconnect()
{
    running = false;
    worker->join();
    core.reset();
}

void MsfsSimulator::AsyncPollSimulator()
{
    // TODO - look at the current Avitab MSFS interface (using SimDLL) for pattern
    using namespace std::chrono_literals;
    while (running) {
        std::this_thread::sleep_for(50ms);
        core->PostSimUpdates();
    }
}

} // namespace navitab
