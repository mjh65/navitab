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

#include "simxp.h"
#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMWeather.h>
#include <XPLM/XPLMProcessing.h>
#include <fmt/core.h>
#include "navitab/config.h"

namespace navitab {

std::shared_ptr<Simulator> navitab::Simulator::GetSimulator(SimulatorCallbacks &cb)
{
    return std::make_shared<sim::SimXPlane>(cb);
}

namespace sim {

SimXPlane::SimXPlane(SimulatorCallbacks &cb)
:   core(&cb),
    LOG(std::make_unique<navitab::logging::Logger>("simxp")),
    flightLoopId(nullptr),
    subMenuIdx(-1),
    subMenu(nullptr)
{
    LOGS("SimXPlane() called");

    // construction is done in response to the XPluginStart() entry point
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1); // 
    XPLMDebugString("NaviTab version " NAVITAB_VERSION_STR "\n");

    flightLoopId = CreateFlightLoop();

    XPLMGetVersions(&xplaneVersion, &xplmVersion, &hostId);
    LOGI(fmt::format("XPLMGetVersion({},{},{})", xplaneVersion, xplmVersion, hostId));

    std::vector<char> scratch;
    scratch.resize(1024);
    char* buffer = &scratch[0];
    XPLMGetSystemPath(buffer);
    xplaneRootPath = buffer;
    LOGI(fmt::format("XPlane system path: {}", xplaneRootPath.string()));

    ourId = XPLMGetMyID();
    XPLMGetPluginInfo(ourId, nullptr, buffer, nullptr, nullptr);
    char* filePart = XPLMExtractFileAndPath(buffer);
    std::filesystem::path p = std::string(buffer, 0, filePart - buffer) + "/../";
    pluginRootPath = p.lexically_normal();
    LOGI(fmt::format("Navitab plugin path: {}", pluginRootPath.string()));

#if 0 // TODO, revive later
    if (xplmVersion >= 400) {
        getMetar = (GetMetarPtr)XPLMFindSymbol("XPLMGetMETARForAirport");
    }
    else {
        getMetar = nullptr;
    }

    updatePlaneCount();

    panelEnabled = std::make_shared<int>(0);
    panelPowered = std::make_shared<int>(0);
    brightness = std::make_shared<float>(1);

    reloadAircraftPath();

    panelEnabledRef = std::make_unique<DataRefExport<int>>("avitab/panel_enabled", this,
        [](void* self) { return *((reinterpret_cast<XPlaneEnvironment*>(self))->panelEnabled); },
        [](void* self, int v) { *((reinterpret_cast<XPlaneEnvironment*>(self))->panelEnabled) = v; });

    panelPoweredRef = std::make_unique<DataRefExport<int>>("avitab/panel_powered", this,
        [](void* self) { return *((reinterpret_cast<XPlaneEnvironment*>(self))->panelPowered); },
        [](void* self, int v) { *((reinterpret_cast<XPlaneEnvironment*>(self))->panelPowered) = v; });

    brightnessRef = std::make_unique<DataRefExport<float>>("avitab/brightness", this,
        [](void* self) { return *((reinterpret_cast<XPlaneEnvironment*>(self))->brightness); },
        [](void* self, float v) { *((reinterpret_cast<XPlaneEnvironment*>(self))->brightness) = v; });

    isInMenuRef = std::make_unique<DataRefExport<int>>("avitab/is_in_menu", this,
        [](void* self) { return (reinterpret_cast<XPlaneEnvironment*>(self))->isInMenu; });

    mapLatitudeRef = std::make_unique<DataRefExport<float>>("avitab/map/latitude", this,
        [](void* self) { return (reinterpret_cast<XPlaneEnvironment*>(self))->getMapLatitude(); });

    mapLongitudeRef = std::make_unique<DataRefExport<float>>("avitab/map/longitude", this,
        [](void* self) { return (reinterpret_cast<XPlaneEnvironment*>(self))->getMapLongitude(); });

    mapZoomRef = std::make_unique<DataRefExport<int>>("avitab/map/zoom", this,
        [](void* self) { return (reinterpret_cast<XPlaneEnvironment*>(self))->getMapZoom(); });

    mapVerticalRangeRef = std::make_unique<DataRefExport<float>>("avitab/map/vertical_range", this,
        [](void* self) { return (reinterpret_cast<XPlaneEnvironment*>(self))->getMapVerticalRange(); });
#endif

    XPLMScheduleFlightLoop(flightLoopId, -1, true);
}

void SimXPlane::Enable()
{
    // this is done in response to the XPluginEnable() entry point
    LOGS("Enable() called");

    // In Avitab quite a lot of this stuff seems to be in the Avitab class,
    // although it feels like it should be simulator specific, so in Navitab
    // it has been moved into here.

    XPLMMenuID pluginMenu = XPLMFindPluginsMenu();
    subMenuIdx = XPLMAppendMenuItem(pluginMenu, NAVITAB_NAME, nullptr, 0);
    if (subMenuIdx < 0) {
        throw navitab::StartupError("Couldn't create our menu item");
    }

    subMenu = XPLMCreateMenu(NAVITAB_NAME, pluginMenu, subMenuIdx, [](void* ref, void* cb) {
        SimXPlane* us = reinterpret_cast<SimXPlane*>(ref);
        auto menuId = reinterpret_cast<intptr_t>(cb);
        MenuCallback callback = us->menuCallbacks[menuId];
        if (callback) callback();
    }, this);

    if (!subMenu) {
        XPLMRemoveMenuItem(pluginMenu, subMenuIdx);
        throw navitab::StartupError("Couldn't create our menu");
    }

    intptr_t idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { toggleWindow(); });
    XPLMAppendMenuItem(subMenu, "Toggle window", reinterpret_cast<void*>(idx), 0);

    idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { resetWindowPosition(); });
    XPLMAppendMenuItem(subMenu, "Reset position", reinterpret_cast<void*>(idx), 0);

    // TODO - this is where the window creation should be done
    // see AviTab::startApp()
}

void SimXPlane::Disable()
{
    // this is done in response to the XPluginDisable() entry point
    LOGS("Disable() called");

    // TODO - save the window position for next time, and destroy the window
    // see AviTab::stopApp()

    if (subMenu) {
        XPLMDestroyMenu(subMenu);
        subMenu = nullptr;
        XPLMRemoveMenuItem(XPLMFindPluginsMenu(), subMenuIdx);
        subMenuIdx = -1;
    }
}

SimXPlane::~SimXPlane()
{
    // destruction is done in response to the XPluginStop() entry point
    LOGS("~SimXPlane() called");

    if (flightLoopId) {
        XPLMDestroyFlightLoop(flightLoopId);
        flightLoopId = nullptr;
    }
}

XPLMFlightLoopID SimXPlane::CreateFlightLoop()
{
    XPLMCreateFlightLoop_t loop;
    loop.structSize = sizeof(XPLMCreateFlightLoop_t);
    loop.phase = 0; // ignored according to docs
    loop.refcon = this;
    loop.callbackFunc = [](float f1, float f2, int c, void* ref) -> float {
        if (!ref) {
            return 0;
        }
        auto* me = reinterpret_cast<SimXPlane*>(ref);
        return me->onFlightLoop(f1, f2, c);
    };

    XPLMFlightLoopID id = XPLMCreateFlightLoop(&loop);
    if (!id) {
        throw navitab::StartupError("Couldn't create flight loop");
    }
    return id;
}

float SimXPlane::onFlightLoop(float elapsedSinceLastCall, float elapseSinceLastLoop, int count)
{
#if 0 // TODO, revive later
    std::vector<Location> activeAircraftLocations;

    updatePlaneCount();
    for (AircraftID i = 0; i <= otherAircraftCount; ++i) {
        try {
            Location loc;
            loc.latitude = dataCache.getLocationData(i, 0).doubleValue;
            loc.longitude = dataCache.getLocationData(i, 1).doubleValue;
            loc.elevation = dataCache.getLocationData(i, 2).doubleValue;
            loc.heading = dataCache.getLocationData(i, 3).floatValue;
            activeAircraftLocations.push_back(loc);
        }
        catch (const std::exception& e) {
            // silently ignore to avoid flooding the log
            // can fail with TCAS override, more than 19 AI aircraft
        }
    }

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        aircraftLocations = activeAircraftLocations;
    }

    setLastFrameTime(dataCache.getData("sim/operation/misc/frame_rate_period").floatValue);

    runEnvironmentCallbacks();
#endif
    core->onFlightLoop();

    return -1;
}

void SimXPlane::toggleWindow()
{
    LOGS("Toggle window menu callback");
}

void SimXPlane::resetWindowPosition()
{
    LOGS("Reset window position menu callback");
}



} // namespace sim
} // namespace navitab
