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

std::shared_ptr<CoreServices> CoreServices::MakeNavitab(SimEngine s, WinServer w)
{
    static bool done = false;
    if (done) return nullptr;
    done = true;
    return std::make_shared<Navitab>(s,w);
}


#if defined(NAVITAB_WINDOWS)
static HostPlatform host = HostPlatform::WIN;
#elif defined(NAVITAB_LINUX)
static HostPlatform host = HostPlatform::LNX;
#elif defined(NAVITAB_MACOSX)
static HostPlatform host = HostPlatform::MAC;
#endif


Navitab::Navitab(SimEngine s, WinServer w)
:   hostPlatform(host),
    simProduct(s),
    winServer(w),
    LOG(std::make_unique<logging::Logger>("navitab")),
    running(false),
    activated(false)
{
    // Early initialisation needs to do enough to get the preferences loaded
    // and the log file created. Everything else can wait! Any failures are
    // reported as thrown exceptions.

    paths = std::make_shared<Paths>();

    // create the log and preferences file names - they have the same format
    auto lfp = paths->DataFilesPath();
    lfp /= "navitab";
    switch (winServer) {
    case PLUGIN: lfp += "_p"; break;
    case DESKTOP: lfp += "_d"; break;
    case HTTP: lfp += "_w"; break;
    }
    switch (simProduct) {
    case MSFS: lfp += "_m"; break;
    case XPLANE: lfp += "_x"; break;
    case MOCK: lfp += "_k"; break;
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
    lm->Configure(winServer == HTTP, lfp, reloaded, settings->Get("/logging"));
}

Navitab::~Navitab()
{
    // in case these were not called properly by the app
    Deactivate();
    Stop();
    LOGS("~Navitab() done");
}

std::shared_ptr<Simulator2Core> Navitab::GetSimulatorCallbacks()
{
    return shared_from_this();
}

std::shared_ptr<Toolbar2Core> Navitab::SetToolbar(std::shared_ptr<Toolbar> t)
{
    assert(!toolbar);
    toolbar = t;
    return shared_from_this();
}

std::shared_ptr<Modebar2Core> Navitab::SetModebar(std::shared_ptr<Modebar> m)
{
    assert(!modebar);
    modebar = m;
    return shared_from_this();
}

std::shared_ptr<Doodler2Core> Navitab::SetDoodler(std::shared_ptr<Doodler> d)
{
    assert(!doodler);
    doodler = d;
    return shared_from_this();
}

std::shared_ptr<Keypad2Core> Navitab::SetKeypad(std::shared_ptr<Keypad> k)
{
    assert(!keypad);
    keypad = k;
    return shared_from_this();
}

std::shared_ptr<WindowPart> Navitab::GetToolbar()
{
    if (!toolbar) toolbar = std::make_shared<CoreToolbar>(shared_from_this(), uiMgr);
    assert(toolbar);
    return toolbar;
}

std::shared_ptr<WindowPart> Navitab::GetModebar()
{
    if (!modebar) modebar = std::make_shared<CoreModebar>(shared_from_this(), uiMgr);
    assert(modebar);
    return modebar;
}

std::shared_ptr<WindowPart> Navitab::GetDoodler()
{
    if (!doodler) doodler = std::make_shared<CoreDoodler>(shared_from_this());
    assert(doodler);
    return doodler;
}

std::shared_ptr<WindowPart> Navitab::GetKeypad()
{
    if (!keypad) keypad = std::make_shared<CoreKeypad>(shared_from_this());
    assert(keypad);
    return keypad;
}

std::shared_ptr<WindowPart> Navitab::GetCanvas()
{
    if (!canvas) canvas = std::make_shared<Canvas>(shared_from_this(), uiMgr);
    assert(canvas);
    return canvas;
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

    uiMgr = std::make_shared<lvglkit::Manager>(std::static_pointer_cast<DeferredJobRunner<int>>(shared_from_this()));
    assert(uiMgr);

    // curl_global_init(CURL_GLOBAL_ALL); TODO: activate this later
}

void Navitab::Activate()
{
    if (!activated) {
        activated = true;
    }
}

void Navitab::Deactivate()
{
    if (activated) {
        activated = false;


    }
}

void Navitab::Stop()
{
    // This is called during X-Plane plugin stop
    // Avitab also calls curl_global_cleanup(), so we need to not forget that 
    // Need to review SDK docs and Avitab.
    canvas.reset();
    toolbar.reset();
    modebar.reset();
    doodler.reset();
    keypad.reset();
    uiMgr.reset();
    settings.reset();
    if (running) {
        running = false;
        int a;
        RunLater([]() {}, &a); // trigger the work loop with a null job
        worker->join();
    }
}

void Navitab::onSimFlightLoop(const SimStateData& data)
{
    if (!activated) return;

    simState = data;
    auto now = navitab::LocalTime("%H:%M:%S");
    if (toolbarStatus.find(now) != 0) {
        // local time has changed, so recreate the toolbarStatus string
        toolbarStatus = now;
        toolbarStatus += fmt::format(" | {}fps", simState.fps);
        int s = simState.zuluTime;
        int h = s / (60 * 60); s -= (h * 60 * 60);
        int m = s / 60; s -= (m * 60);
        toolbarStatus += fmt::format(" | {:02}:{:02}:{:02}Z", h, m, s);
        toolbarStatus += fmt::format(" | {:+.3f},{:+.3f}", simState.myPlane.latitude, simState.myPlane.longitude);
        toolbar->SetStausInfo(toolbarStatus);
        LOGD(fmt::format("Z:{}:{}:{}, FPS:{}", h, m, s, simState.fps));
        LOGD(fmt::format("N:{},E:{}", simState.myPlane.latitude, simState.myPlane.longitude));
    }

    canvas->UpdateProtoDevelopment(); // TODO - remove this once we have LVGL installed
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

void Navitab::RunLater(std::function<void()> j, void*)
{
    {
        std::lock_guard<std::mutex> lock(qmutex);
        jobs.push(j);
    }
    qsync.notify_one();
}

void Navitab::RunLater(std::function<void()> j, int*)
{
    void* x = nullptr;
    RunLater(j,x);
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
