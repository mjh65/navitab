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
#include "mocksim.h"
#include "navitab/core.h"

std::shared_ptr<navitab::Simulator> navitab::Simulator::Factory()
{
    return std::make_shared<navitab::MockSimulator>();
}

namespace navitab {

MockSimulator::MockSimulator()
:   LOG(std::make_unique<logging::Logger>("mocksim")),
    running(false),
    tiktok(false)
{
    auto& d = mockData[0];
    d.myPlane.latitude = 55.974728f;
    d.myPlane.longitude = -3.970579f;
    d.myPlane.elevation = 100.0f;
    d.myPlane.heading = 290.0f;
    d.nOtherPlanes = MAX_OTHER_AIRCRAFT;
    for (auto i = 0; i < MAX_OTHER_AIRCRAFT; ++i) {
        d.otherPlanes[i].latitude = 55.0f + ((rand() % 2000) / 1000.0f);
        d.otherPlanes[i].longitude = -3.0f - ((rand() % 2000) / 1000.0f);
        d.otherPlanes[i].elevation = 100.0f + ((rand() % 100) * 100.0);
        d.otherPlanes[i].heading = (rand() % 360);
    }
    d.zuluTime = rand() % (24 * 60 * 60);
    d.fps = 15;
    mockData[1] = mockData[0];
}

MockSimulator::~MockSimulator()
{
}

void MockSimulator::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->GetPrefsManager();
    handler = core->GetSimulatorCallbacks();
    running = true;
    worker = std::make_unique<std::thread>([this]() { AsyncRunSimulator(); });
}

void MockSimulator::Disconnect()
{
    running = false;
    worker->join();
    handler.reset();
    prefs.reset();
    core.reset();
}

void MockSimulator::AsyncRunSimulator()
{
    // TODO - move the planes around a bit, and get frame rate from window
    using namespace std::chrono_literals;
    while (running) {
        std::this_thread::sleep_for(50ms);
        handler->PostSimUpdates(mockData[tiktok ? 1 : 0]);
        tiktok = !tiktok;
    }
}

} // namespace navitab
