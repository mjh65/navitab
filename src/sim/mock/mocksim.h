/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/simulator.h"
#include "navitab/logger.h"
#include <thread>
#include <vector>

namespace navitab {

struct AircraftJourney;

class MockSimulator : public Simulator
{
public:
    MockSimulator();
    ~MockSimulator();

    void Connect(std::shared_ptr<CoreServices>) override;
    void Disconnect() override;

private:
    void AsyncRunSimulator();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Settings> prefs;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Simulator2Core> handler;

    bool running;
    std::unique_ptr<std::thread> worker;

    std::vector<AircraftJourney> journeys;

    SimStateData mockData[2];
    bool tiktok;
};

} // namespace navitab
