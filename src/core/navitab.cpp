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
#include <fmt/core.h>

namespace navitab {

std::unique_ptr<System> System::GetSystem(Simulation s, AppClass c)
{
    static bool done = false;
    if (done) return nullptr;
    done = true;
    return std::make_unique<core::Navitab>(s,c);
}

namespace core {


#if defined(NAVITAB_WINDOWS)
static HostPlatform host = HostPlatform::WIN;
#elif defined(NAVITAB_LINUX)
static HostPlatform host = HostPlatform::LNX;
#elif defined(NAVITAB_MACOSX)
static HostPlatform host = HostPlatform::MAC;
#endif


Navitab::Navitab(Simulation s, AppClass c)
:   hostPlatform(host),
    simProduct(s),
    appClass(c),
    dataFilesPath(FindDataFilesPath())
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
    case NONE: break;
    }
    auto pfp = lfp;
    lfp += "_log.txt";
    pfp += "_prefs.json";

    // initialise the log file
    auto lm = navitab::logging::LogManager::GetLogManager();
    lm->UseConsole(appClass == CONSOLE);
    lm->SetLogFile(lfp);

    // load the preferences
    prefs = std::make_shared<Prefs>(pfp);

    // set the logging preferences (using the json directly)
    lm->Configure(prefs->Get("/logging"));
}

Navitab::~Navitab()
{
    // in case these were not called properly by the app
    Disable();
    Stop();
}

void Navitab::Start()
{
    // Further initialisation is done here once the basic preference and
    // logging services have been started.
    // This is called during X-Plane plugin start, and probably does relatively little
    // Need to review SDK docs and Avitab.
}

void Navitab::Enable()
{
    // This is called during X-Plane plugin enable, and probably does a bit more
    // Need to review SDK docs and Avitab.
}

void Navitab::Disable()
{
    // This is called during X-Plane plugin disable
    // Need to review SDK docs and Avitab.
}

void Navitab::Stop()
{
    // This is called during X-Plane plugin stop
    // Avitab also calls curl_global_cleanup(), so we need to not forget that 
    // Need to review SDK docs and Avitab.
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
    return std::filesystem::path(); // TODO
}

// browsing start for any aircraft documents
std::filesystem::path Navitab::AircraftResourcesPath()
{
    return std::filesystem::path(); // TODO
}

// browsing start for flight plans / routes
std::filesystem::path Navitab::FlightPlansPath()
{
    return std::filesystem::path(); // TODO
}

// directory containing the current Navitab executable
std::filesystem::path Navitab::NavitabPath()
{
    return std::filesystem::path(); // TODO
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



} // namespace core
} // namespace navitab
