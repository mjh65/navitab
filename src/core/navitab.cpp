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

#include "navitab/core.h"
#include "navitab/prefs.h"
#include "logmanager.h"
#include <filesystem>
#include <fmt/core.h>

namespace navitab {
namespace core {


#if defined(NAVITAB_WINDOWS)
static navitab::core::HostPlatform host = navitab::core::HostPlatform::WIN;
#elif defined(NAVITAB_LIUNX)
static navitab::core::HostPlatform host = navitab::core::HostPlatform::LNX;
#elif defined(NAVITAB_MACOSX)
static navitab::core::HostPlatform host = navitab::core::HostPlatform::MAC;
#endif


Navitab::Navitab(Simulation s, AppClass c)
:   hostPlatform(host),
    simProduct(s),
    appClass(c)
{
    // Early initialisation needs to do enough to get the preferences loaded
    // and the log file created. Everything else can wait! Any failures are
    // reported as thrown exceptions.

    FindDataFilesPath();

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
    }
    auto pfp = lfp;
    lfp += "_log.txt";
    pfp += "_prefs.json";

    // initialise the log file
    auto lm = navitab::logging::LogManager::GetLogManager();
    lm->SetConsole(appClass == CONSOLE);
    lm->SetLogFile(lfp);

    // load the preferences
    prefs = std::make_unique< Preferences>(pfp);

    // TODO - get the logging preferences from the json and use it
    // to configure the logging filters

}

Navitab::~Navitab()
{
}

void Navitab::init()
{
    // Further initialisation is done here once the basic preference and
    // logging services have been started.
}

void Navitab::FindDataFilesPath()
{
    // The data files path is where the log file, preferences, downloads
    // and cached files are stored. It is system dependent, but usually
    // relative to the users home directory.

#if defined(NAVITAB_WINDOWS)
    // try these environment variables in turn, use the first one that's defined
    const char* e = 0;
    if (!e) e = std::getenv("LOCALAPPDATA");
    if (!e) e = std::getenv("APPDATA");
    if (!e) e = std::getenv("USERPROFILE");
    if (!e) e = std::getenv("TEMP");
    if (!e) throw StartupError(fmt::format("Unable to determine path for Navitab data files, somewhere near line {} in {}", __LINE__, __FILE__));

    std::filesystem::path candidate(e);
    candidate /= "Navitab";

#elif defined(NAVITAB_LINUX)
#error TBD
#elif defined(NAVITAB_MACOSX)
#error TBD
#endif

    (void)std::filesystem::create_directories(candidate);
    if (!std::filesystem::exists(candidate) || !std::filesystem::is_directory(candidate)) {
        throw StartupError(fmt::format("Unable to create directory for Navitab data files, somewhere near line {} in {}", __LINE__, __FILE__));
    }
    dataFilesPath = candidate;
}


} // namespace core
} // namespace navitab
