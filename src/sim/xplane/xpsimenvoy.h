/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <functional>
#include <vector>
#include <filesystem>
#include <cassert>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMMenus.h>
#include <XPLM/XPLMDataAccess.h>
#include "navitab/xpplugin.h"
#include "navitab/logger.h"
#include "../../win/xplane/xpwin.h"

namespace navitab {

class XPlaneSimulatorEnvoy : public XPlaneSimulator
{
public:
    XPlaneSimulatorEnvoy();
    ~XPlaneSimulatorEnvoy();

    // Connect/disconnect the XPlane envoy from Navitab. Called when XPlane starts and stops the plugin
    void Connect(std::shared_ptr<CoreServices> core) override;
    void Disconnect() override;

    // The main plugin state machine events
    void Start() override;
    void Enable() override;
    void Disable() override;
    void Stop() override;

    void onVRmodeChange(bool entering) override;
    void onPlaneLoaded() override;

private:
    XPLMFlightLoopID CreateFlightLoop();
    bool GetFlightLoopDataRefs();

private:
    // called from XPlane on each flight loop
    float onFlightLoop(float elapsedSinceLastCall, float elapseSinceLastLoop, int count);

    // menu entry callbacks
    void showWindow();
    void resetWindow();
    void reloadAllPlugins();

private:
    // access to Navitab core
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Settings> prefs;
    std::shared_ptr<Simulator2Core> coreSimCallbacks;

    // logging
    std::unique_ptr<logging::Logger> LOG;

    // versions, identifiers and paths from XPlane
    int xplaneVersion;
    int xplmVersion;
    XPLMHostApplicationID hostId;
    XPLMPluginID ourId;
    std::filesystem::path xplaneRootPath;
    std::filesystem::path pluginRootPath;
    std::filesystem::path aircraftDir;
    std::string aircraftName;

    // the flight loop ID, called on each frame
    XPLMFlightLoopID flightLoopId;

    // menu identifiers and callbacks
    int subMenuIdx;
    XPLMMenuID subMenu;
    using MenuCallback = std::function<void()>;
    std::vector<MenuCallback> menuCallbacks;

    // simulation datarefs and per-flightloop data
    XPLMDataRef vrModeDataRef;
    XPLMDataRef zuluTimeDataRef;
    XPLMDataRef frameRateDataRef;
    std::vector<XPLMDataRef> aircraftPositionDataRefs;
    SimStateData simState[2];
    bool tiktok;

    // window manager
    std::shared_ptr<XPlaneWindow> panel;
    bool isInVRmode;

};

} // namespace navitab
