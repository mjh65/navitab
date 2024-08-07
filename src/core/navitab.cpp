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
#include "prefs.h"
#include <cstdio>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include "coretoolbar.h"
#include "coremodebar.h"
#include "coredoodler.h"
#include "corekeypad.h"

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
    dataFilesPath(FindDataFilesPath()),
    enabled(false)
{
    // Early initialisation needs to do enough to get the preferences loaded
    // and the log file created. Everything else can wait! Any failures are
    // reported as thrown exceptions.

    // create the log and preferences file names - they have the same format
    auto lfp = dataFilesPath;
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

    // load the preferences
    prefs = std::make_shared<Prefs>(pfp);

    // configure the logging manager
    bool reloaded = false;
    auto gp = prefs->Get("/general");
    try {
        reloaded = gp.at("/reloading"_json_pointer);
        // remove it to avoid persisting on next run (unless overridden!)
        gp.erase("reloading");
        prefs->Put("/general", gp);
    }
    catch (...) {}

    auto lm = logging::LogManager::GetLogManager();
    lm->Configure(appClass == CONSOLE, lfp, reloaded, prefs->Get("/logging"));
}

Navitab::~Navitab()
{
    // in case these were not called properly by the app
    Disable();
    Stop();
    LOGS("~Navitab() done");
}

std::shared_ptr<SimulatorEvents> Navitab::GetSimulatorCallbacks()
{
    return shared_from_this();
}

std::shared_ptr<WindowEvents> Navitab::GetWindowCallbacks()
{
    return shared_from_this();
}

void Navitab::SetSimulator(std::shared_ptr<Simulator> s)
{
    simulator = s;
}

void Navitab::onSimFlightLoop()
{
    UNIMPLEMENTED(__func__);
    // TODO - this will be the trigger for various regular update jobs
    // zulu time and current time, update every 1s
    // FPS, update every 2s ish
    // location - on each report
}

void Navitab::SetWindow(std::shared_ptr<Window> w)
{
    window = w;
    toolbar->SetWindow(window);
    modebar->SetWindow(window);
    doodler->SetWindow(window);
    keypad->SetWindow(window);
    window->SetHandlers(toolbar, modebar, doodler, keypad);
}

std::shared_ptr<Toolbar> Navitab::GetToolbar()
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::shared_ptr<Modebar> Navitab::GetModebar()
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::shared_ptr<Doodler> Navitab::GetDoodler()
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::shared_ptr<Keypad> Navitab::GetKeypad()
{
    UNIMPLEMENTED(__func__);
    return nullptr;
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
            UNIMPLEMENTED("XPLANE:PLUGIN");
        } else if (appClass == DESKTOP) {
            UNIMPLEMENTED("XPLANE:DESKTOP");
        } else if (appClass == CONSOLE) {
            UNIMPLEMENTED("XPLANE:CONSOLE");
        }
    } else if (simProduct == MOCK) {
        if (appClass == PLUGIN) {
            UNIMPLEMENTED("MOCK:PLUGIN");
        } else if (appClass == DESKTOP) {
            toolbar = std::make_shared<CoreToolbar>(shared_from_this());
            modebar = std::make_shared<CoreModebar>(shared_from_this());
            doodler = std::make_shared<CoreDoodler>(shared_from_this());
            keypad = std::make_shared<CoreKeypad>(shared_from_this());
        } else if (appClass == CONSOLE) {
            UNIMPLEMENTED("MOCK:CONSOLE");
        }
    }

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
        AsyncCall([]() {});
        worker->join();
    }
}

std::shared_ptr<Preferences> Navitab::PrefsManager()
{
    return prefs;
}

// location of the preferences and log files, as well as any temporary file
// and cached downloads
std::filesystem::path Navitab::DataFilesPath()
{
    return dataFilesPath;
}

// browsing start for the user's resources, eg charts, docs
std::filesystem::path Navitab::UserResourcesPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

// browsing start for any aircraft documents
std::filesystem::path Navitab::AircraftResourcesPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

// browsing start for flight plans / routes
std::filesystem::path Navitab::FlightPlansPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

// directory containing the current Navitab executable
std::filesystem::path Navitab::NavitabPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

void Navitab::onCanvasResize(int width, int height)
{
    UNIMPLEMENTED(__func__);
#if 0
    // TODO - this is just here for development and testing. of course it will get
    // replaced eventually!
    auto& canvas = *(partImages[CANVAS]);
    if (rand() % 60) {
        // write random pixels
        for (int i = 0; i < 8; ++i) {
            size_t rp = ((rand() << 20) + rand()) % canvas.imageBuffer.size();
            // red in 7:0, green in 15:8, blue in 23:16, alpha in 31:24
            canvas.imageBuffer[rp] = (rand() % 0xffffff);
        }
    } else {
        // draw one of our generated SVG icons to test the generator
        auto y0 = rand() % (canvas.Height() - sample_64x64_HEIGHT);
        auto x0 = rand() % (canvas.Width() - sample_64x64_WIDTH);
        for (int y=0; y < sample_64x64_HEIGHT; ++y) {
            for (int x=0; x < sample_64x64_WIDTH; ++x) {
                auto si = y * sample_64x64_WIDTH + x;
                auto di = (y + y0) * canvas.Width() + (x + x0);
                canvas.imageBuffer[di] = sample_64x64[si];
            }
        }
    }

    if (bDelta > 0.0f) {
        brightness += bDelta;
        if (brightness >= 1.0f) {
            bDelta = 0.0 - bDelta;
            brightness = 1.0f;
        }
    } else {
        brightness += bDelta;
        if (brightness <= 0.2f) {
            bDelta = 0.0 - bDelta;
            brightness = 0.2f;
        }
    }
#endif

}

void Navitab::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void Navitab::onWheelEvent(int x, int y, int xdir, int ydir)
{
    UNIMPLEMENTED(__func__);
}

void Navitab::onKeyEvent(int code)
{
    UNIMPLEMENTED(__func__);
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

std::filesystem::path Navitab::FindDataFilesPath()
{
    // The data files path is where the log file, preferences, downloads
    // and cached files are stored. The location is system dependent, and
    // a number of options are tried in order until one is successful. The
    // first pass looks for an existing directory. If none are found the
    // next pass attempts to create the directory. If that doesn't work the
    // game is abandoned.

    std::vector<std::filesystem::path> options;
#if defined(NAVITAB_WINDOWS)
    // try these environment variables in turn, use the first one that's defined
    const char* e;
    if (e = std::getenv("LOCALAPPDATA")) options.push_back(e);
    if (e = std::getenv("APPDATA")) options.push_back(e);
    if (e = std::getenv("USERPROFILE")) options.push_back(e);
    if (e = std::getenv("TEMP")) options.push_back(e);
    options.push_back("C:\\"); // clutching at straws
#elif defined(NAVITAB_LINUX)
    // on Linux the preferred location is ~/.navitab - probably!
    const char *e;
    if ((e = std::getenv("HOME")) != nullptr) {
        std::filesystem::path home(e);
        std::filesystem::path d1(home); d1 /= ".navitab";
        options.push_back(d1);
        std::filesystem::path d2(home); d2 /= ".config"; d2 /= "navitab";
        options.push_back(d2);
    }
#elif defined(NAVITAB_MACOSX)
    // on Mac the preferred location is ~/Library/Application Support/Navitab
    const char *e;
    if ((e = std::getenv("HOME")) != nullptr) {
        std::filesystem::path home(e);
        std::filesystem::path as(home); as /= "Library"; as /= "Application Support";
        options.push_back(as);
        options.push_back(home);
    }
    if ((e = std::getenv("TMPDIR"))!= nullptr) options.push_back(e);
    options.push_back("/tmp"); // clutching at straws
#endif

    // first pass, directory must exist and be useable
    // second pass, try to make the directory before testing
    for (auto pass : { 1,2 }) {
        for (auto& p : options) {
            std::filesystem::path d(p);
#if !defined(NAVITAB_LINUX)
            d /= "Navitab";
#endif
            if (pass == 2) (void)std::filesystem::create_directory(d);
            if (std::filesystem::exists(d) || std::filesystem::is_directory(d)) {
                // check we can create files
                std::filesystem::path tmpf(d / "navitab.tmp");
                bool failed = !std::ofstream(tmpf).put('x');
                std::remove(tmpf.string().c_str());
                if (!failed) return d;
            }
        }
    }
    throw StartupError(fmt::format("Unable to find or create directory for Navitab data files, before line {} in {}", __LINE__, __FILE__));
}

void Navitab::AsyncCall(std::function<void()> j)
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
