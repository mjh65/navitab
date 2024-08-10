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

#include <fstream>
#include <fmt/core.h>
#include "paths.h"
#include "navitab/core.h"

namespace navitab {

Paths::Paths()
{
    dataFilesPath = FindDataFilesPath();
}

// location of the preferences and log files, as well as any temporary file
// and cached downloads
std::filesystem::path Paths::DataFilesPath()
{
    return dataFilesPath;
}

// browsing start for the user's resources, eg charts, docs
std::filesystem::path Paths::UserResourcesPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

// browsing start for any aircraft documents
std::filesystem::path Paths::AircraftResourcesPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

// browsing start for flight plans / routes
std::filesystem::path Paths::FlightPlansPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

// directory containing the current Navitab executable
std::filesystem::path Paths::NavitabPath()
{
    UNIMPLEMENTED(__func__);
    return std::filesystem::path();
}

std::filesystem::path Paths::FindDataFilesPath()
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
    const char* e;
    if ((e = std::getenv("HOME")) != nullptr) {
        std::filesystem::path home(e);
        std::filesystem::path d1(home); d1 /= ".navitab";
        options.push_back(d1);
        std::filesystem::path d2(home); d2 /= ".config"; d2 /= "navitab";
        options.push_back(d2);
    }
#elif defined(NAVITAB_MACOSX)
    // on Mac the preferred location is ~/Library/Application Support/Navitab
    const char* e;
    if ((e = std::getenv("HOME")) != nullptr) {
        std::filesystem::path home(e);
        std::filesystem::path as(home); as /= "Library"; as /= "Application Support";
        options.push_back(as);
        options.push_back(home);
    }
    if ((e = std::getenv("TMPDIR")) != nullptr) options.push_back(e);
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

} // namespace navitab