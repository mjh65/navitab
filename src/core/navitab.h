/*
 *  Navitab - Navigation tablet for VR flight simulation
 *  Copyright (C) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (C) 2018-2024 Folke Will <folko@solhost.org>
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

#include <memory>
#include "navitab/core.h"
#include "navitab/logger.h"
#include "navitab/simulator.h"
#include "navitab/window.h"

// This header file defines a class that manages the startup and use of the
// Navitab subsystems. Each of the executable/plugin's main() function should
// instantiate exactly one of these, and destroy it on closure. The class
// also implements the interface to the simulator.

namespace navitab {

class Navitab : public std::enable_shared_from_this<Navitab>, public System, public SimulatorEvents, public WindowEvents
{
public:
    // Constructing the Navitab object also does enough initialisation to
    // get the logging working. Any errors during this phase are likely to
    // be unrecoverable and will cause a StartupError exception to be thrown.
    Navitab(SimEngine s, AppClass c);
    virtual ~Navitab();

    // System base class overrides
    
    // hook up with simulator and window
    std::shared_ptr<SimulatorEvents> GetSimulatorInterface() override;
    std::shared_ptr<WindowEvents> GetWindowInterface() override;

    // Startup and shutdown control - fine-grained enough to support all app classes.
    void Start() override;    // TODO - called from XPluginStart - review this in SDK and Avitab
    void Enable() override;  // TODO - called from XPluginEnable - review this in SDK and Avitab
    void Disable() override; // TODO - called from XPluginDisable - review this in SDK and Avitab
    void Stop() override;    // TODO - called from XPluginStop - review this in SDK and Avitab

    // access to preferences
    std::shared_ptr<Preferences> PrefsManager() override;
    
    // location of the preferences and log files, as well as any temporary file
    // and cached downloads
    std::filesystem::path DataFilesPath() override;

    // browsing start for the user's resources, eg charts, docs
    std::filesystem::path UserResourcesPath() override;

    // browsing start for any aircraft documents
    std::filesystem::path AircraftResourcesPath() override;

    // browsing start for flight plans / routes
    std::filesystem::path FlightPlansPath() override;

    // directory containing the current Navitab executable
    std::filesystem::path NavitabPath() override;

    // SimulatorEvents implementation
    
    void SetSimulator(std::shared_ptr<Simulator>) override;
    void onFlightLoop() override;

    // WindowEvents implementation
    
    void SetWindow(std::shared_ptr<Window>) override;
    void onWindowResize(int width, int height) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override;
    void onKeyEvent(char code) override;

    // TODO the frame buffer interface needs some thought, just have a placeholder for now
    // called to check for redraw
    // TODO - define parameters
    bool getUpdateRegion() override;

protected:
    std::filesystem::path FindDataFilesPath();

private:
    const HostPlatform              hostPlatform;
    const AppClass                  appClass;
    const SimEngine                 simProduct;

    // logging
    std::unique_ptr<logging::Logger> LOG;

    std::filesystem::path           dataFilesPath;

    bool                            started;
    bool                            enabled;

    std::shared_ptr<Simulator> simEnv;
    std::shared_ptr<Preferences>    prefs;
};

} // namespace navitab
