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
    d.myPlane.loc.latitude = 55.974728f;
    d.myPlane.loc.longitude = -3.970579f;
    d.myPlane.elevation = 100.0f;
    d.myPlane.heading = 290.0f;
    d.nOtherPlanes = MAX_OTHER_AIRCRAFT;
    for (auto i = 0; i < MAX_OTHER_AIRCRAFT; ++i) {
        d.otherPlanes[i].loc.latitude = 55.0f + ((rand() % 2000) / 1000.0f);
        d.otherPlanes[i].loc.longitude = -3.0f - ((rand() % 2000) / 1000.0f);
        d.otherPlanes[i].elevation = 100.0f + ((rand() % 100) * 100.0);
        d.otherPlanes[i].heading = (rand() % 360);
    }
    d.fps = 15;
    mockData[1] = mockData[0];
}

MockSimulator::~MockSimulator()
{
}

void MockSimulator::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->GetSettingsManager();
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
    auto start = std::chrono::steady_clock::now();
    srand((unsigned int)start.time_since_epoch().count());
    auto zuluStart = rand() % (24 * 60 * 60);
    mockData[1].zuluTime = mockData[0].zuluTime = zuluStart;
    double lat = (rand() % 150) - 75;
    double lon = (rand() % 360);
    double dlat = ((rand() % 1000) - 500) / 10000.0;
    double dlon = ((rand() % 1000) - 500) / 10000.0;

    // TODO - move the planes around a bit, and get frame rate from window
    using namespace std::chrono_literals;
    while (running) {
        std::this_thread::sleep_for(50ms);
        mockData[tiktok].myPlane.loc.latitude = lat;
        mockData[tiktok].myPlane.loc.longitude = lon;
        lat += dlat;
        if ((lat <= -90.0) || (lat >= 90.0)) {
            dlat = 0 - dlat;
            lat += 2 * dlat;
        }
        lon += dlon;
        if (lon < -180.0) { lon += 360.0; }
        if (lon >= 360.0) { lon -= 360.0; }
        if ((rand() % 200) == 0) {
            dlat = ((rand() % 1000) - 500) / 10000.0;
        }
        if ((rand() % 200) == 0) {
            dlon = ((rand() % 1000) - 500) / 10000.0;
        }
        handler->PostSimUpdates(mockData[tiktok ? 1 : 0]);
        tiktok = !tiktok;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        mockData[tiktok].zuluTime = (zuluStart + elapsed.count()) % (24 * 60 * 60);
    }
}

} // namespace navitab
