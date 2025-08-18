/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <cassert>
#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMWeather.h>
#include <XPLM/XPLMProcessing.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include "xpsimenvoy.h"
#include "navitab/config.h"
#include "navitab/core.h"
#include "../../win/xplane/xpdesktopwin.h"
#include "../../win/xplane/xpvrwin.h"

std::shared_ptr<navitab::XPlaneSimulator> navitab::XPlaneSimulator::Factory()
{
    return std::make_shared<navitab::XPlaneSimulatorEnvoy>();
}

namespace navitab {

XPlaneSimulatorEnvoy::XPlaneSimulatorEnvoy()
:   LOG(std::make_unique<logging::Logger>("simxp")),
    xplaneVersion(0), xplmVersion(0), hostId(0), ourId(0),
    flightLoopId(nullptr),
    subMenuIdx(-1),
    subMenu(nullptr),
    tiktok(false),
    isInVRmode(false)
{
    LOGI("Constructing XPlaneSimulatorEnvoy()");
    vrModeDataRef = XPLMFindDataRef("sim/graphics/VR/enabled");
    if (!vrModeDataRef) throw LogFatal("required XPlane dataref not found");
}

void XPlaneSimulatorEnvoy::Connect(std::shared_ptr<CoreServices> c)
{
    LOGI("Connect() called");
    core = c;
    prefs = core->GetSettingsManager();
    coreSimCallbacks = core->GetSimulatorCallbacks();
}

void XPlaneSimulatorEnvoy::Disconnect()
{
    LOGI("Disconnect() called");
    coreSimCallbacks.reset();
    prefs.reset();
    core.reset();
}

void XPlaneSimulatorEnvoy::Start()
{
    LOGI("Start() called");
    assert(coreSimCallbacks);
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

    // get all the datarefs we will be reading on each flight loop
    if (!GetFlightLoopDataRefs()) {
        LOGE("Could not get all datarefs from XPlane - Navitab will not finish loading");
        return;
    }

    // create and schedule the flight loop - only if all datarefs were retrieved
    flightLoopId = CreateFlightLoop();
    XPLMScheduleFlightLoop(flightLoopId, -1, true);
}

void XPlaneSimulatorEnvoy::Enable()
{
    // this is done in response to the XPluginStart() entry point
    LOGI("Enable() called");
    assert(coreSimCallbacks);

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
    menuCallbacks.push_back([this] { showWindow(); });
    XPLMAppendMenuItem(subMenu, "Show window", reinterpret_cast<void*>(idx), 0);

    idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { resetWindow(); });
    XPLMAppendMenuItem(subMenu, "Reset window", reinterpret_cast<void*>(idx), 0);

    // TODO - we probably want to add this menu item only in debug builds??
    idx = menuCallbacks.size();
    menuCallbacks.push_back([this] { reloadAllPlugins(); });
    XPLMAppendMenuItem(subMenu, "Reload all plugins", reinterpret_cast<void*>(idx), 0);

    // find out what plane is being flown
    onPlaneLoaded();

    // create the window
    bool showNow = false;
    auto& xwdp = prefs->Get("/xplane/window");
    try {
        showNow = xwdp.at("/open_at_start"_json_pointer);
    }
    catch (...) {}
    LOGD(fmt::format("Read open_at_start preference as {}", showNow));
    isInVRmode = (XPLMGetDatai(vrModeDataRef) != 0);
    if (isInVRmode) {
        panel = std::make_shared<XPVRWindow>();
    } else {
        panel = std::make_shared<XPDesktopWindow>();
    }
    panel->Create(core);
    if (showNow) panel->Show();
}

void XPlaneSimulatorEnvoy::Disable()
{
    // this is done in response to the XPluginDisable() entry point
    LOGI("Disable() called");

    // save the window state for next time
    auto xwdp = prefs->Get("/xplane/window");
    xwdp["open_at_start"] = panel->isActive();
    prefs->Put("/xplane/window", xwdp);
    LOGD(fmt::format("Wrote open_at_start preference to {}", panel->isActive()));

    panel->Destroy();
    panel.reset();

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

void XPlaneSimulatorEnvoy::onVRmodeChange(bool entering)
{
    LOGI(fmt::format("VR mode change notified: {}", entering ? "entering" : "leaving"));
    isInVRmode = entering;

    auto active = panel->isActive();
    panel->Destroy();

    if (isInVRmode) {
        panel = std::make_shared<XPVRWindow>();
    }
    else {
        panel = std::make_shared<XPDesktopWindow>();
    }
    panel->Create(core);
    if (active) panel->Show();
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
    LOGD("XPlaneSimulatorEnvoy() has now been destroyed");
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

bool XPlaneSimulatorEnvoy::GetFlightLoopDataRefs()
{
    if (!(zuluTimeDataRef = XPLMFindDataRef("sim/time/zulu_time_sec"))) return false;
    if (!(frameRateDataRef = XPLMFindDataRef("sim/time/framerate_period"))) return false;

    aircraftPositionDataRefs.push_back(XPLMFindDataRef("sim/flightmodel/position/latitude"));
    aircraftPositionDataRefs.push_back(XPLMFindDataRef("sim/flightmodel/position/longitude"));
    aircraftPositionDataRefs.push_back(XPLMFindDataRef("sim/flightmodel/position/psi"));
    aircraftPositionDataRefs.push_back(XPLMFindDataRef("sim/flightmodel/position/elevation"));
    std::string b0("sim/multiplayer/position/plane");
    for (int i = 1; i < 10; ++i) {
        char d = '0' + i;
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b0 + d + "_lat").c_str()));
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b0 + d + "_lon").c_str()));
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b0 + d + "_psi").c_str()));
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b0 + d + "_el").c_str()));
    }
    std::string b1("sim/multiplayer/position/plane1");
    for (int i = 0; i < 10; ++i) {
        char d = '0' + i;
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b1 + d + "_lat").c_str()));
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b1 + d + "_lon").c_str()));
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b1 + d + "_psi").c_str()));
        aircraftPositionDataRefs.push_back(XPLMFindDataRef((b1 + d + "_el").c_str()));
    }
    assert(aircraftPositionDataRefs.size() == 80);

    for (auto& dr : aircraftPositionDataRefs) {
        if (!dr) return false;
    }

    return true;
}

float XPlaneSimulatorEnvoy::onFlightLoop(float elapsedSinceLastCall, float elapseSinceLastLoop, int count)
{
    auto& fld = simState[tiktok];
    tiktok = !tiktok;

    int tmp1, active;
    XPLMPluginID tmp2;
    XPLMCountAircraft(&tmp1, &active, &tmp2);
    assert(active > 0);
    fld.nOtherPlanes = std::min(active - 1, (int)SimStateData::MAX_OTHER_AIRCRAFT);

    float lat, lon, alt, hdg;
    auto dri = aircraftPositionDataRefs.begin();
    lat = XPLMGetDataf(*dri++);
    lon = XPLMGetDataf(*dri++);
    hdg = XPLMGetDataf(*dri++);
    alt = XPLMGetDataf(*dri++);
    fld.myPlane = Position(Trajectory(Location(lat, lon, Location::DEGREES), hdg, Location::DEGREES), alt);
    for (int p = 0; p < SimStateData::MAX_OTHER_AIRCRAFT; ++p) {
        if (p < fld.nOtherPlanes) {
            lat = XPLMGetDataf(*dri++);
            lon = XPLMGetDataf(*dri++);
            hdg = XPLMGetDataf(*dri++);
            alt = XPLMGetDataf(*dri++);
            fld.otherPlanes[p] = Position(Trajectory(Location(lat, lon, Location::DEGREES), hdg, Location::DEGREES), alt);
        } else {
            fld.otherPlanes[p] = Position(Trajectory(Location(0, 0, Location::DEGREES), 0, Location::DEGREES), -1000);
        }
    }

    fld.zuluTime = (int)XPLMGetDataf(zuluTimeDataRef);
    fld.fps = (int)(1.0f / std::max(XPLMGetDataf(frameRateDataRef), 0.01f));
    fld.loopCount = (unsigned long)count;
    coreSimCallbacks->PostSimUpdates(fld);

    if (panel) panel->CheckVitalSigns();

    return -1;
}

void XPlaneSimulatorEnvoy::showWindow()
{
    LOGD("Show window menu callback");
    if (panel) panel->Show();
}

void XPlaneSimulatorEnvoy::resetWindow()
{
    LOGD("Reset window menu callback");
    if (panel) panel->Reset();
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
