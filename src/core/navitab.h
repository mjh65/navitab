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
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "navitab/core.h"
#include "navitab/logger.h"
#include "navitab/platform.h"
#include "navitab/simulator.h"
#include "navitab/window.h"
#include "navitab/toolbar.h"
#include "navitab/modebar.h"
#include "navitab/doodler.h"
#include "navitab/keypad.h"
#include "canvas.h"

// This header file defines a class that manages the startup and use of the
// Navitab subsystems. Each of the executable/plugin's main() function should
// instantiate exactly one of these, and destroy it on closure. The class
// also implements the interface to the simulator.

namespace lvglkit {
    class Manager;
}

namespace navitab {

class Navitab : public std::enable_shared_from_this<Navitab>,
                public CoreServices,
                public Simulator2Core,
                public DeferredJobRunner<int>,
                public Toolbar2Core, public Modebar2Core, public Doodler2Core, public Keypad2Core, public CanvasEvents
{
public:
    // Constructing the Navitab object also does enough initialisation to
    // get the logging working. Any errors during this phase are likely to
    // be unrecoverable and will cause a StartupError exception to be thrown.
    Navitab(SimEngine s, WinServer w);
    virtual ~Navitab();

    // ======================================================================
    // Implementation of CoreServices
    
    // access to platform and settings
    std::shared_ptr<PathServices> GetPathService() override { return paths; }
    std::shared_ptr<Settings> GetSettingsManager() override { return settings; }

    // Interfaces used by simulator and UI window
    std::shared_ptr<Simulator2Core> GetSimulatorCallbacks() override;
    std::shared_ptr<WindowPart> GetPartCallbacks(int part) override;
    void SetWindowControl(std::shared_ptr<WindowControls> w) override;

    // Startup and shutdown control - fine-grained enough to support all app classes.
    void Start() override;    // TODO - called from XPluginStart - review this in SDK and Avitab
    void Enable() override;  // TODO - called from XPluginEnable - review this in SDK and Avitab
    void Disable() override; // TODO - called from XPluginDisable - review this in SDK and Avitab
    void Stop() override;    // TODO - called from XPluginStop - review this in SDK and Avitab

    // ======================================================================
    // Implementation of Simulator2Core
    void onSimFlightLoop(const SimStateData& data) override;

    // ======================================================================
    // Implementation of Toolbar2Core
    void onToolClick(Tool t) override;

    // ======================================================================
    // Implementation of Modebar2Core
    void onModeSelect(Mode m) override;

    // ======================================================================
    // Implementation of Keypad2Core
    void onKeypadEvent(int code) override;

    // ======================================================================
    // Implementation of CanvasEvents
    void onFoo() override {}

    // ======================================================================
    // Implementation of DeferredJobRunner (via several other intermediate base classes)
    void RunLater(std::function<void ()>, void* s = nullptr) override;
    void RunLater(std::function<void ()>, int* s = nullptr) override;

private:
    void AsyncWorker();

private:
    const HostPlatform                  hostPlatform;
    const WinServer                     winServer;
    const SimEngine                     simProduct;

    std::unique_ptr<logging::Logger>    LOG;
    std::shared_ptr<PathServices>       paths;
    std::shared_ptr<Settings>           settings;

    std::shared_ptr<WindowControls>     winCtrl;
    std::shared_ptr<Toolbar>            toolbar;
    std::shared_ptr<Modebar>            modebar;
    std::shared_ptr<Doodler>            doodler;
    std::shared_ptr<Keypad>             keypad;
    std::shared_ptr<Canvas>             canvas;
    std::shared_ptr<lvglkit::Manager>   uiMgr;

    bool                                running;
    bool                                enabled;
    SimStateData                        simState;
    std::string                         toolbarStatus;

    std::unique_ptr<std::thread>        worker;
    std::queue<std::function<void()>>   jobs;
    std::condition_variable             qsync;
    std::mutex                          qmutex;

};

} // namespace navitab
