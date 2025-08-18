/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <chrono>
#include "mocksim.h"
#include "navitab/core.h"

std::shared_ptr<navitab::Simulator> navitab::Simulator::Factory()
{
    return std::make_shared<navitab::MockSimulator>();
}

namespace navitab {

struct AircraftJourney
{
    Position legStart;
    double angVelocity;
    unsigned step;

};

MockSimulator::MockSimulator()
:   LOG(std::make_unique<logging::Logger>("mocksim")),
    running(false),
    tiktok(false)
{
    // create some random journeys
    srand((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
    Location origin((rand() % 1600) / 10.0f - 80, (rand() % 3600) / 10.0f - 180, Location::DEGREES);
    while (journeys.size() < (SimStateData::MAX_OTHER_AIRCRAFT + 1)) {
        double lat = origin.latDegrees() + ((rand() % 2000) / 1000.0f) - 1;
        double lon = origin.lonDegrees() + ((rand() % 2000) / 1000.0f) - 1;
        double head = (rand() % 360);
        double alt = 24000 + (rand() % 15000);
        AircraftJourney j;
        j.legStart = Position(Trajectory(Location(lat, lon, Location::DEGREES), head, Location::DEGREES), alt, Position::FEET);
        j.step = 0;
        j.angVelocity = 0.0001f; // airliner cruise speed
        journeys.push_back(j);
    }
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
    using namespace std::chrono_literals;
    unsigned long n = 0;
    auto start = std::chrono::steady_clock::now();
    auto zuluStart = rand() % (24 * 60 * 60);
    mockData[0].fps = mockData[1].fps = 25;
    mockData[0].nOtherPlanes = mockData[1].nOtherPlanes = SimStateData::MAX_OTHER_AIRCRAFT;

    while (running) {
        std::this_thread::sleep_for(40ms); // 25fps
        auto& sd = mockData[tiktok ? 1 : 0];
        // update sim state data
        sd.loopCount = n++;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        sd.zuluTime = (zuluStart + elapsed.count()) % (24 * 60 * 60);
        // move the planes
        for (auto& j : journeys) {
            ++j.step;
        }
        auto& j = journeys[0];
        auto loc = j.legStart.getWaypoint(j.step * j.angVelocity, Location::DEGREES);
        sd.myPlane = loc;
        sd.myPlane.alt_metres = j.legStart.alt_metres;
        for (size_t i = 0; i < SimStateData::MAX_OTHER_AIRCRAFT; ++i) {
            auto& j = journeys[i + 1];
            auto loc = j.legStart.getWaypoint(j.step * j.angVelocity, Location::DEGREES);
            sd.otherPlanes[i] = loc;
            sd.otherPlanes[i].alt_metres = j.legStart.alt_metres;
        }
        handler->PostSimUpdates(sd);
        tiktok = !tiktok;
    }
}

} // namespace navitab
