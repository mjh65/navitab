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
#include <nlohmann/json_fwd.hpp>

// This header file defines abstract interfaces to the main components of
// Navitab, and a single factory function to make the central core object.

namespace navitab {


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

// The classes in here should be mostly interfaces!!!

// The Preferences class is a relatively small wrapper that can be used
// by Navitab components to fetch (and update) preference data that is
// stored between runs.

class Preferences
{
public:
    virtual const nlohmann::json& Get(const std::string key) = 0;
    virtual void Put(const std::string key, nlohmann::json& value) = 0;
};

// The executable/plugin's main() function should call the factory to create
// exactly one instance of the Navitab core, and then destroy it on closure.

class System
{
public:
    static std::unique_ptr<System> GetSystem(Simulation s, AppClass c);

    virtual ~System() = default;

    // Startup and shutdown control - fine-grained enough to support all app classes.
    virtual void Start() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;
    virtual void Stop() = 0;

    // access to preferences
    virtual std::shared_ptr<Preferences> PrefsManager() = 0;
    
    // location of the preferences and log files, as well as any temporary file
    // and cached downloads
    virtual std::filesystem::path DataFilesPath() = 0;

    // browsing start for the user's resources, eg charts, docs
    virtual std::filesystem::path UserResourcesPath() = 0;

    // browsing start for any aircraft documents
    virtual std::filesystem::path AircraftResourcesPath() = 0;

    // browsing start for flight plans / routes
    virtual std::filesystem::path FlightPlansPath() = 0;

    // directory containing the current Navitab executable
    virtual std::filesystem::path NavitabPath() = 0;

};

} // namespace navitab
