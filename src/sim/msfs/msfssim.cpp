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

#include <chrono>
#include "msfssim.h"
#include "navitab/core.h"

std::shared_ptr<navitab::Simulator> navitab::Simulator::Factory()
{
    return std::make_shared<navitab::MsfsSimulator>();
}

namespace navitab {

MsfsSimulator::MsfsSimulator()
:   LOG(std::make_unique<logging::Logger>("msfssim")),
    running(false),
    tiktok(false)
{
    // TODO - replace this with real data extracted from the simlation
    mockData[0].nOtherPlanes = MAX_OTHER_AIRCRAFT;
    mockData[0].zuluTime = 0;
    mockData[0].fps = 15;
    mockData[1] = mockData[0];
}

MsfsSimulator::~MsfsSimulator()
{
}

void MsfsSimulator::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->GetSettingsManager();
    handler = core->GetSimulatorCallbacks();
    running = true;
    worker = std::make_unique<std::thread>([this]() { AsyncPollSimulator(); });
}

void MsfsSimulator::Disconnect()
{
    running = false;
    worker->join();
    handler.reset();
    prefs.reset();
    core.reset();
}

void MsfsSimulator::AsyncPollSimulator()
{
    // TODO - look at the current Avitab MSFS interface (using SimDLL) for pattern
    using namespace std::chrono_literals;
    while (running) {
        std::this_thread::sleep_for(50ms);
        handler->PostSimUpdates(mockData[tiktok ? 1 : 0]);
        tiktok = !tiktok;
    }
}

} // namespace navitab
