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

#include "xpsimenvoy.h"
#include <cassert>
#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMWeather.h>
#include <XPLM/XPLMProcessing.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "navitab/config.h"
#include "navitab/core.h"

std::shared_ptr<navitab::XPlaneSimulator> navitab::XPlaneSimulator::Factory()
{
    return std::make_shared<navitab::XPlaneSimulatorEnvoy>();
}

namespace navitab {

XPlaneSimulatorEnvoy::XPlaneSimulatorEnvoy()
:   LOG(std::make_unique<logging::Logger>("simxp")),
    flightLoopId(nullptr),
    subMenuIdx(-1),
    subMenu(nullptr)
{
    LOGI("Constructing XPlaneSimulatorEnvoy()");
}

void XPlaneSimulatorEnvoy::Connect(std::shared_ptr<SimulatorEvents> cb)
{
    LOGI("Connect() called");
    core = cb;
}

void XPlaneSimulatorEnvoy::Disconnect()
{
    LOGI("Disonnect() called");
    core.reset();
}

void XPlaneSimulatorEnvoy::Start()
{
    LOGI("Start() called");
    assert(core);
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1); // 
    XPLMDebugString("NaviTab version " NAVITAB_VERSION_STR "\n");

    // get versions and host ID
    XPLMGetVersions(&xplaneVersion, &xplmVersion, &hostId);
    LOGI(fmt::format("XPLMGetVersion({},{},{})", xplaneVersion, xplmVersion, hostId));

    // get path to XPlane root directory
    std::vector<char> scratch;
    scratch.resize(1024);
    char* buffer = &scratch[0];
    XPLMGetSystemPath(buffer);
    xplaneRootPath = buffer;
    LOGI(fmt::format("XPlane system path: {}", xplaneRootPath.string()));

    // get path to this plugin's root directory
    ourId = XPLMGetMyID();
    XPLMGetPluginInfo(ourId, nullptr, buffer, nullptr, nullptr);
    char* filePart = XPLMExtractFileAndPath(buffer);
    std::filesystem::path rp = std::string(buffer, 0, filePart - buffer);
    pluginRootPath = rp.parent_path().lexically_normal();
    LOGI(fmt::format("Navitab plugin path: {}", pluginRootPath.string()));

    // create and schedule the flight loop
    flightLoopId = CreateFlightLoop();
    XPLMScheduleFlightLoop(flightLoopId, -1, true);

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
}

void XPlaneSimulatorEnvoy::Enable()
{
    // this is done in response to the XPluginStart() entry point
    LOGI("Enable() called");
    assert(core);

    // In Avitab quite a lot of this stuff seems to be in the Avitab class,
    // although it feels like it should be simulator specific, so in Navitab
    // it has been moved into here.

    XPLMMenuID pluginMenu = XPLMFindPluginsMenu();
    subMenuIdx = XPLMAppendMenuItem(pluginMenu, NAVITAB_NAME, nullptr, 0);
    if (subMenuIdx < 0) {
        throw StartupError("Couldn't create our menu item");
    }

    subMenu = XPLMCreateMenu(NAVITAB_NAME, pluginMenu, subMenuIdx, [](void* ref, void* cb) {
        XPlaneSimulatorEnvoy* us = reinterpret_cast<XPlaneSimulatorEnvoy*>(ref);
        auto menuId = reinterpret_cast<intptr_t>(cb);
        MenuCallback callback = us->menuCallbacks[menuId];
        if (callback) callback();
    }, this);

    if (!subMenu) {
        XPLMRemoveMenuItem(pluginMenu, subMenuIdx);
        throw StartupError("Couldn't create our menu");
    }

    intptr_t idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { toggleWindow(); });
    XPLMAppendMenuItem(subMenu, "Toggle window", reinterpret_cast<void*>(idx), 0);

    idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { resetWindowPosition(); });
    XPLMAppendMenuItem(subMenu, "Reset position", reinterpret_cast<void*>(idx), 0);

    // TODO - we probably want to add this menu item only in debug builds??
    idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { reloadAllPlugins(); });
    XPLMAppendMenuItem(subMenu, "Reload all plugins", reinterpret_cast<void*>(idx), 0);

    // find out what plane is being flown
    onPlaneLoaded();

    // TODO - this is where the window creation should be done
    // see AviTab::startApp()

    desktopWindow = std::make_unique<XPDesktopWindow>(prefs);
}

void XPlaneSimulatorEnvoy::Disable()
{
    // this is done in response to the XPluginDisable() entry point
    LOGI("Disable() called");

    // TODO - save the window position for next time, and destroy the window
    // see AviTab::stopApp()
    desktopWindow.reset();

    if (subMenu) {
        XPLMDestroyMenu(subMenu);
        subMenu = nullptr;
        XPLMRemoveMenuItem(XPLMFindPluginsMenu(), subMenuIdx);
        subMenuIdx = -1;
    }
}

void XPlaneSimulatorEnvoy::Stop()
{
    LOGI("Stop() called");
}

int XPlaneSimulatorEnvoy::FrameRate()
{
    // TODO needs an implementation
    return 1;
}

void XPlaneSimulatorEnvoy::onVRmodeChange(bool entering)
{
    // TODO - test this callback when XP is launched from SteamVR home in VR mode
    LOGI(fmt::format("VR mode change notified: {}", entering ? "entering" : "leaving"));
    if (desktopWindow) desktopWindow->showHide(!entering);
}

void XPlaneSimulatorEnvoy::onPlaneLoaded()
{
    std::vector<char> scratch;
    scratch.resize(1024);
    char* p = &scratch[0];
    char* f = &scratch[512];
    XPLMGetNthAircraftModel(0, f, p);
    std::filesystem::path acf(p);
    aircraftDir = acf.parent_path().lexically_normal();
    aircraftName = acf.stem().string();
    LOGI(fmt::format("Airplane loaded: {} in {}", aircraftName, aircraftDir.string()));
}

XPlaneSimulatorEnvoy::~XPlaneSimulatorEnvoy()
{
    // destruction is done in response to the XPluginStop() entry point
    if (flightLoopId) {
        XPLMDestroyFlightLoop(flightLoopId);
        flightLoopId = nullptr;
    }
    LOGI("XPlaneSimulatorEnvoy() has now been destroyed");
}

void XPlaneSimulatorEnvoy::SetPrefs(std::shared_ptr<Preferences> p)
{
    prefs = p;
}

XPLMFlightLoopID XPlaneSimulatorEnvoy::CreateFlightLoop()
{
    XPLMCreateFlightLoop_t loop;
    loop.structSize = sizeof(XPLMCreateFlightLoop_t);
    loop.phase = 0; // ignored according to docs
    loop.refcon = this;
    loop.callbackFunc = [](float f1, float f2, int c, void* ref) -> float {
        if (!ref) {
            return 0;
        }
        auto* me = reinterpret_cast<XPlaneSimulatorEnvoy*>(ref);
        return me->onFlightLoop(f1, f2, c);
    };

    XPLMFlightLoopID id = XPLMCreateFlightLoop(&loop);
    if (!id) {
        throw StartupError("Couldn't create flight loop");
    }
    return id;
}

float XPlaneSimulatorEnvoy::onFlightLoop(float elapsedSinceLastCall, float elapseSinceLastLoop, int count)
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
    if (desktopWindow) desktopWindow->onFlightLoop();
    core->onFlightLoop();

    return -1;
}

void XPlaneSimulatorEnvoy::toggleWindow()
{
    LOGI("Toggle window menu callback");
    if (desktopWindow) desktopWindow->toggle();
}

void XPlaneSimulatorEnvoy::resetWindowPosition()
{
    LOGI("Reset window position menu callback");
    if (desktopWindow) desktopWindow->recentre();
}

// Reloading the plugins is a convenience during development. This menu
// option will eventually be excluded from release builds. (TODO)
void XPlaneSimulatorEnvoy::reloadAllPlugins()
{
    // set the preference /general/reloading to indicate that the log file should be continued
    auto gp = prefs->Get("/general");
    gp["reloading"] = true;
    prefs->Put("/general", gp);

    // now ask XPlane to reload (all!) the plugins
    XPLMReloadPlugins();
}

} // namespace navitab
