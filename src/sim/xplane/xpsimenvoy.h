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

#pragma once

#include "navitab/xpplugin.h"
#include <functional>
#include <vector>
#include <filesystem>
#include <cassert>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMMenus.h>
#include "xpwin.h"
#include "navitab/logger.h"

namespace navitab {

class XPlaneSimulatorEnvoy : public XPlaneSimulator
{
public:
    XPlaneSimulatorEnvoy();
    ~XPlaneSimulatorEnvoy();

    // Connect/disconnect the XPlane liaison from Navitab. Called when XPlane starts and stops the plugin
    void SetPrefs(std::shared_ptr<Preferences> prefs) override;
    void Connect(std::shared_ptr<SimulatorEvents> scb, std::shared_ptr<WindowEvents> wcb) override;
    void Connect(std::shared_ptr<SimulatorEvents>) override { assert(0); }
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

private:
    // called from XPlane on each flight loop
    float onFlightLoop(float elapsedSinceLastCall, float elapseSinceLastLoop, int count);

    // menu entry callbacks
    void showWindow();
    void recentreWindow();
    void reloadAllPlugins();

private:
    // access to Navitab core
    std::shared_ptr<SimulatorEvents> coreSimCallbacks;
    std::shared_ptr<WindowEvents> coreWinCallbacks;
    std::shared_ptr<Preferences> prefs;

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

    // window manager
    std::unique_ptr<XPlaneWindow> win;
    bool isInVRmode;

};

} // namespace navitab
