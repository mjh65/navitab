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

#include "navitab.h"
#include "logmanager.h"
#include "settingsmgr.h"
#include <cstdio>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include "coretoolbar.h"
#include "coremodebar.h"
#include "coredoodler.h"
#include "corekeypad.h"
#include "canvas.h"
#include "../platform/paths.h"
#include "../lvglkit/toolkit.h"

namespace navitab {

std::shared_ptr<CoreServices> CoreServices::MakeNavitab(SimEngine s, AppClass c)
{
    static bool done = false;
    if (done) return nullptr;
    done = true;
    return std::make_shared<Navitab>(s,c);
}


#if defined(NAVITAB_WINDOWS)
static HostPlatform host = HostPlatform::WIN;
#elif defined(NAVITAB_LINUX)
static HostPlatform host = HostPlatform::LNX;
#elif defined(NAVITAB_MACOSX)
static HostPlatform host = HostPlatform::MAC;
#endif


Navitab::Navitab(SimEngine s, AppClass c)
:   hostPlatform(host),
    simProduct(s),
    appClass(c),
    LOG(std::make_unique<logging::Logger>("navitab")),
    running(false),
    enabled(false)
{
    // Early initialisation needs to do enough to get the preferences loaded
    // and the log file created. Everything else can wait! Any failures are
    // reported as thrown exceptions.

    paths = std::make_shared<Paths>();

    // create the log and preferences file names - they have the same format
    auto lfp = paths->DataFilesPath();
    lfp /= "navitab";
    switch (appClass) {
    case PLUGIN: lfp += "_p"; break;
    case DESKTOP: lfp += "_d"; break;
    case CONSOLE: lfp += "_c"; break;
    }
    switch (simProduct) {
    case MSFS: lfp += "_m"; break;
    case XPLANE: lfp += "_x"; break;
    case MOCK: break;
    }
    auto pfp = lfp;
    lfp += "_log.txt";
    pfp += "_prefs.json";

    // load the settings
    settings = std::make_shared<SettingsManager>(pfp);

    // configure the logging manager
    bool reloaded = false;
    auto gp = settings->Get("/general");
    try {
        reloaded = gp.at("/reloading"_json_pointer);
        // remove it to avoid persisting on next run (unless overridden!)
        gp.erase("reloading");
        settings->Put("/general", gp);
    }
    catch (...) {}

    auto lm = logging::LogManager::GetLogManager();
    lm->Configure(appClass == CONSOLE, lfp, reloaded, settings->Get("/logging"));
}

Navitab::~Navitab()
{
    // in case these were not called properly by the app
    Disable();
    Stop();
    LOGS("~Navitab() done");
}

std::shared_ptr<Simulator2Core> Navitab::GetSimulatorCallbacks()
{
    return shared_from_this();
}

std::shared_ptr<WindowPart> Navitab::GetPartCallbacks(int part)
{
    switch (part) {
    case WindowPart::TOOLBAR: assert(toolbar); return toolbar;
    case WindowPart::MODEBAR: assert(modebar); return modebar;
    case WindowPart::DOODLER: assert(doodler); return doodler;
    case WindowPart::KEYPAD: assert(keypad); return keypad;
    case WindowPart::CANVAS: assert(canvas); return canvas;
    default: assert(0); return nullptr;
    }
}

void Navitab::SetWindowControl(std::shared_ptr<WindowControls> w)
{
    winCtrl = w;
}

void Navitab::Start()
{
    // Further initialisation is done here once the basic preference and
    // logging services have been started.

    if (running) return;
    running = true;

    // Start the background worker thread. Most of the actual work done in
    // the Navitab core is triggered by jobs posted to this thread, and most
    // of those jobs are UI interactions or simulator updates.

    worker = std::make_unique<std::thread>([this]() { AsyncWorker(); });

    uiMgr = std::make_shared<lvglkit::Manager>();
    assert(uiMgr);

    // Create the toolbar, modebar, doodler and keypad. Which implementation
    // depends on the combination of simulator and application type.

    if (simProduct == MSFS) {
        if (appClass == PLUGIN) {
            UNIMPLEMENTED("MSFS:PLUGIN");
        } else if (appClass == DESKTOP) {
            UNIMPLEMENTED("MSFS:DESKTOP");
        } else if (appClass == CONSOLE) {
            UNIMPLEMENTED("MSFS:CONSOLE");
        }
    } else if (simProduct == XPLANE) {
        if (appClass == PLUGIN) {
            canvas = std::make_shared<Canvas>(shared_from_this(), uiMgr);
            toolbar = std::make_shared<CoreToolbar>(shared_from_this(), uiMgr);
            modebar = std::make_shared<CoreModebar>(shared_from_this());
            doodler = std::make_shared<CoreDoodler>(shared_from_this());
            keypad = std::make_shared<CoreKeypad>(shared_from_this());
        } else if (appClass == DESKTOP) {
            UNIMPLEMENTED("XPLANE:DESKTOP");
        } else if (appClass == CONSOLE) {
            UNIMPLEMENTED("XPLANE:CONSOLE");
        }
    } else if (simProduct == MOCK) {
        if (appClass == PLUGIN) {
            UNIMPLEMENTED("MOCK:PLUGIN");
        } else if (appClass == DESKTOP) {
            canvas = std::make_shared<Canvas>(shared_from_this(), uiMgr);
            toolbar = std::make_shared<CoreToolbar>(shared_from_this(), uiMgr);
            modebar = std::make_shared<CoreModebar>(shared_from_this());
            doodler = std::make_shared<CoreDoodler>(shared_from_this());
            keypad = std::make_shared<CoreKeypad>(shared_from_this());
        } else if (appClass == CONSOLE) {
            UNIMPLEMENTED("MOCK:CONSOLE");
        }
    }
    assert(canvas);
    assert(toolbar);
    assert(modebar);
    assert(doodler);
    assert(keypad);

    // curl_global_init(CURL_GLOBAL_ALL); TODO: activate this later
}

void Navitab::Enable()
{
    // This is called during X-Plane plugin enable, and probably does a bit more
    // Need to review SDK docs and Avitab.
    if (!enabled) {
        enabled = true;
    }
}

void Navitab::Disable()
{
    // This is called during X-Plane plugin disable
    // Need to review SDK docs and Avitab.
    if (enabled) {
        enabled = false;
    }
}

void Navitab::Stop()
{
    // This is called during X-Plane plugin stop
    // Avitab also calls curl_global_cleanup(), so we need to not forget that 
    // Need to review SDK docs and Avitab.
    if (running) {
        running = false;
        RunLater([]() {});
        worker->join();
    }
    canvas.reset();
    toolbar.reset();
    modebar.reset();
    doodler.reset();
    keypad.reset();
    uiMgr.reset();
    settings.reset();
}

void Navitab::onSimFlightLoop(const SimStateData& data)
{
    auto prevZulu = simState.zuluTime;
    simState = data;

    // only update the toolbar display each second (TODO: use wallclock for this, in case sim pause stops ZT incrementing)
    if (simState.zuluTime != prevZulu) {
        int s = simState.zuluTime;
        int h = s / (60 * 60); s -= (h * 60 * 60);
        int m = s / 60; s -= (m * 60);
        toolbar->SetSimZuluTime(h, m, s);
        toolbar->SetFrameRate(simState.fps);
        LOGD(fmt::format("Z:{}:{}:{}, FPS:{}", h, m, s, simState.fps));
        LOGD(fmt::format("N:{},E:{}", simState.myPlane.latitude, simState.myPlane.longitude));
    }

    canvas->UpdateProtoDevelopment();
}

void Navitab::onToolClick(Tool t)
{
    UNIMPLEMENTED(__func__);
}

void Navitab::onModeSelect(Mode m)
{
    UNIMPLEMENTED(__func__);
}

void Navitab::onKeypadEvent(int code)
{
    UNIMPLEMENTED(__func__);
}

void Navitab::RunLater(std::function<void()> j)
{
    {
        std::lock_guard<std::mutex> lock(qmutex);
        jobs.push(j);
    }
    qsync.notify_one();
}

void Navitab::AsyncWorker()
{
    while (1) {
        std::unique_lock<std::mutex> lock(qmutex);
        qsync.wait(lock, [this]() { return !running || !jobs.empty(); });
        if (!running) break;
        auto job = jobs.front();
        jobs.pop();
        lock.unlock();

        // run the job
        job();
    }
}

} // namespace navitab
