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

#include "navitab/simulator.h"
#include "navitab/logger.h"
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMMenus.h>
#include <functional>
#include <vector>

namespace navitab {
namespace sim {

class SimXPlane : public XPlaneSimulator
{
public:
    SimXPlane(SimulatorCallbacks &cb);
    ~SimXPlane();
    
    void Enable() override;
    void Disable() override;
    void onVRmodeChange(bool entering) override;
    void onPlaneLoaded() override;

private:
    XPLMFlightLoopID CreateFlightLoop();

private:
    // called from XPlane on each flight loop
    float onFlightLoop(float elapsedSinceLastCall, float elapseSinceLastLoop, int count);

    // menu entry callbacks
    void toggleWindow();
    void resetWindowPosition();
    void reloadAllPlugins();

private:
    // access to Navitab core
    SimulatorCallbacks * const core;

    std::unique_ptr<navitab::logging::Logger> LOG;

    // versions, identifiers and paths from XPlane
    int xplaneVersion;
    int xplmVersion;
    XPLMHostApplicationID hostId;
    XPLMPluginID ourId;
    std::filesystem::path xplaneRootPath;
    std::filesystem::path pluginRootPath;

    // the flight loop ID, called on each frame
    XPLMFlightLoopID flightLoopId;
    int subMenuIdx;
    XPLMMenuID subMenu;

    using MenuCallback = std::function<void()>;
    std::vector<MenuCallback> menuCallbacks;
};


} // namespace sim
} // namespace navitab