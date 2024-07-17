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

#include <exception>
#include <filesystem>

// This header file defines a class that manages the startup and use of the
// Navitab subsystems. Each of the executable/plugin's main() function should
// instantiate exactly one of these, and destroy it on closure.

namespace navitab {
namespace core {


enum HostPlatform { WIN, LNX, MAC };
enum AppClass { PLUGIN, DESKTOP, CONSOLE };
enum Simulation { NONE, MSFS, XPLANE };


struct Exception : public std::exception
{
    std::string const what;
    Exception(std::string e) : what(e) {}
    std::string What() { return what; }
};
struct StartupError : public Exception
{
    StartupError(std::string e) : Exception(e) {}
};
struct LogFatal : public Exception
{
    LogFatal(std::string e) : Exception(e) {}
};


class Preferences;

class Navitab
{
public:
    // Constructing the Navitab object also does enough initialisation to
    // get the logging working. Any errors during this phase are likely to
    // be unrecoverable and will cause a StartupError exception to be thrown.
    Navitab(Simulation s, AppClass c);
    ~Navitab();

    // do the remaining initialisation
    void init();

    // shutdown the subsystems in an orderly manner
    void shutdown();

    // location of the preferences and log files, as well as any temporary file
    // and cached downloads
    std::filesystem::path DataFilesPath();

    // browsing start for the user's resources, eg charts, docs
    std::filesystem::path UserResourcesPath();

    // browsing start for any aircraft documents
    std::filesystem::path AircraftResourcesPath();

    // browsing start for flight plans / routes
    std::filesystem::path FlightPlansPath();

    // directory containing the current Navitab executable
    std::filesystem::path NavitabPath();

protected:
    std::filesystem::path FindDataFilesPath();

private:
    const HostPlatform              hostPlatform;
    const AppClass                  appClass;
    const Simulation                simProduct;

    std::filesystem::path           dataFilesPath;

    std::unique_ptr<Preferences>    prefs;
};

} // namespace core
} // namespace navitab
