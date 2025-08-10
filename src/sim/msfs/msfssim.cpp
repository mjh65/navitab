/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
    unsigned long n = 0;
    // TODO - look at the current Avitab MSFS interface (using SimDLL) for pattern
    using namespace std::chrono_literals;
    while (running) {
        std::this_thread::sleep_for(50ms);
        mockData[tiktok].loopCount = n++;
        handler->PostSimUpdates(mockData[tiktok ? 1 : 0]);
        tiktok = !tiktok;
    }
}

} // namespace navitab
