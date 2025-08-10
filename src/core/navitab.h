/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/core.h"
#include "navitab/logger.h"
#include "navitab/platform.h"
#include "navitab/simulator.h"
#include "navitab/window.h"
#include "navitab/toolbar.h"
#include "navitab/modebar.h"
#include "navitab/doodler.h"
#include "navitab/keypad.h"
#include "appcanvas.h"
#include "../docs/docmanager.h"
#include "../maps/maptileprovider.h"
#include "../navdb/navdb.h"
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

// This header file defines a class that manages the startup and use of the
// Navitab subsystems. Each of the executable/plugin's main() function should
// instantiate exactly one of these, and destroy it on closure. The class
// also implements the interface to the simulator.

namespace lvglkit {
    class Manager;
}

namespace navitab {

class App;
class AboutApp;
class MapApp;
class AirportApp;
class RouteApp;
class ReaderApp;
class SettingsApp;

class Navitab : public std::enable_shared_from_this<Navitab>,
                public CoreServices,
                public AppServices,
                public Simulator2Core,
                public DeferredJobRunner<int>,
                public Toolbar2Core, public Modebar2Core, public Doodler2Core, public Keypad2Core, public AppCanvas2Core
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
    std::shared_ptr<Toolbar2Core> SetToolbar(std::shared_ptr<Toolbar> t) override;
    std::shared_ptr<Modebar2Core> SetModebar(std::shared_ptr<Modebar> m) override;
    std::shared_ptr<Doodler2Core> SetDoodler(std::shared_ptr<Doodler> d) override;
    std::shared_ptr<Keypad2Core> SetKeypad(std::shared_ptr<Keypad> k) override;
    //std::shared_ptr<WindowPart> GetWindowPart(int part) override;
    std::shared_ptr<WindowPart> GetToolbar() override;
    std::shared_ptr<WindowPart> GetModebar() override;
    std::shared_ptr<WindowPart> GetDoodler() override;
    std::shared_ptr<WindowPart> GetKeypad() override;
    std::shared_ptr<WindowPart> GetAppCanvas() override;
    void SetWindowControl(std::shared_ptr<WindowControls> w) override;

    // Startup and shutdown control - fine-grained enough to support all app classes.
    void Start() override;
    void Activate() override;
    void Deactivate() override;
    void Stop() override;

    // ======================================================================
    // Implementation of AppServices
    std::shared_ptr<DocumentManager> GetDocsProvider() override;
    std::shared_ptr<MapTileProvider> GetMapsProvider() override;
    std::shared_ptr<NavProvider> GetNavProvider() override;
    void EnableTools(int toolMask, int repeatMask) override;
    PixelBuffer GetCanvasPixels() override;

    // ======================================================================
    // Implementation of Simulator2Core
    void onSimFlightLoop(const SimStateData& data) override;

    // ======================================================================
    // Implementation of Toolbar2Core
    void onToolClick(ClickableTool t) override;

    // ======================================================================
    // Implementation of Modebar2Core
    void onAppSelect(Mode m) override;
    void onDoodlerToggle() override;
    void onKeypadToggle() override;

    // ======================================================================
    // Implementation of Keypad2Core
    void onKeypadEvent(int code) override;

    // ======================================================================
    // Implementation of Canvas2Core
    void StartApps() override;
    void onFoo() override {}

    // ======================================================================
    // Implementation of DeferredJobRunner (via several other intermediate base classes)
    void RunLater(std::function<void ()>, void* s = nullptr) override;
    void RunLater(std::function<void ()>, int* s = nullptr) override;

private:
    void AsyncWorker();
    
private:
    std::shared_ptr<App> FindApp(Mode m);

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
    std::shared_ptr<AppCanvas>          appcanvas;
    std::shared_ptr<lvglkit::Manager>   uiMgr;

    std::shared_ptr<MapTileProvider>       maptileProvider;
    std::shared_ptr<DocumentManager>       docManager;
    std::shared_ptr<NavProvider>        navProvider;

    std::shared_ptr<AboutApp>           aboutApp;
    std::shared_ptr<MapApp>             mapApp;
    std::shared_ptr<AirportApp>         airportApp;
    std::shared_ptr<RouteApp>           routeApp;
    std::shared_ptr<ReaderApp>          readerApp;
    std::shared_ptr<SettingsApp>        settingsApp;
    std::shared_ptr<App>                activeApp;

    bool                                running;
    bool                                activated;
    SimStateData                        simState;

    // TODO - this pattern appears in a few places. turn into a base class?
    std::unique_ptr<std::thread>        worker;
    std::queue<std::function<void()>>   jobs;
    std::condition_variable             qsync;
    std::mutex                          qmutex;

};

} // namespace navitab
