/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <functional>
#include <vector>
#include "navitab/deferred.h"
#include "navitab/navigation.h"

/*
 * This header file defines the interface to the simulator, which will
 * include access to position data, aircraft type, METAR, etc, and specific
 * filesystem locations where different types of document are expected to
 * be stored.
*/

namespace navitab {

struct CoreServices;
class Settings;
struct Simulator;

struct SimStateData
{
    AircraftPosition    myPlane;
    size_t              nOtherPlanes;
    AircraftPosition    otherPlanes[MAX_OTHER_AIRCRAFT];
    unsigned            zuluTime;
    unsigned            fps;
    unsigned long       loopCount;
};

// The Simulator2Core interface defines services that the simulator
// requires from the Navitab core. Calls to these services will generally
// be from the simulator's thread and should do minimal work.

struct Simulator2Core : public DeferredJobRunner<>
{
    // Called from the simulator on each flight loop, and provides updates
    // to simulation-derived data. Double-buffered in sim, but not mutex protected.
    void PostSimUpdates(const SimStateData &data) {
        RunLater([this, data]() { onSimFlightLoop(data); });
    }

protected:
    virtual void onSimFlightLoop(const SimStateData& data) = 0;

};


// The Simulator interface defines the services that the Simulator offers to 
// the application/plugin.

struct Simulator
{
    // Factory, implemented in each simulator library.
    static std::shared_ptr<Simulator> Factory();

    // APIs called from the application/plugin
    virtual void Connect(std::shared_ptr<CoreServices> core) = 0;
    virtual void Disconnect() = 0;

    virtual ~Simulator() = default;

};

} // namespace navitab
