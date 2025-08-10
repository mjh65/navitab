/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
    unsigned long n = 0;
    auto start = std::chrono::steady_clock::now();
    srand((unsigned int)start.time_since_epoch().count());
    auto zuluStart = rand() % (24 * 60 * 60);
    mockData[1].zuluTime = mockData[0].zuluTime = zuluStart;
    double lat = (rand() % 150) - 75;
    double lon = (rand() % 360) - 180;
    double dlat = ((rand() % 1000) - 500) / 10000.0;
    double dlon = ((rand() % 1000) - 500) / 10000.0;

    // TODO - move the planes around a bit, and get frame rate from window
    using namespace std::chrono_literals;
    while (running) {
        std::this_thread::sleep_for(50ms);
        mockData[tiktok].loopCount = n++;
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
