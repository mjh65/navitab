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
#include <functional>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>

// This header file defines abstract interfaces to the main components of
// Navitab, and a single factory function to make the central core object.

namespace navitab {

struct SimulatorEvents;
struct WindowPart;
struct WindowControl;

enum HostPlatform { WIN, LNX, MAC };
enum AppClass { PLUGIN, DESKTOP, CONSOLE };
enum SimEngine { MOCK, MSFS, XPLANE };

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

// The Preferences class is a relatively small wrapper that can be used
// by Navitab components to fetch (and update) preference data that is
// stored between runs.

struct Preferences
{
    virtual const nlohmann::json& Get(const std::string key) = 0;
    virtual void Put(const std::string key, nlohmann::json& value) = 0;
};

// The CoreServices class is the central management and interface object for the Navitab
// system, and provides the connectivity by sharing interfaces needed by the simulation
// and windowing UI subsystems.

struct CoreServices
{
    // The executable / plugin's main() function should call the factory to create
    // exactly one instance of the Navitab core, and then destroy it on closure.
    static std::shared_ptr<CoreServices> MakeNavitab(SimEngine s, AppClass c);

    // Get the interface to the preferences manager
    virtual std::shared_ptr<Preferences> GetPrefsManager() = 0;

    // Get the interface for simulation-generated events that Navitab will handle
    virtual std::shared_ptr<SimulatorEvents> GetSimulatorCallbacks() = 0;

    // Get the interfaces for UI-window-generated events that each of the window
    // parts will handle.
    virtual std::shared_ptr<WindowPart> GetPartCallbacks(int part) = 0;

    // Set the interface to the UI-window for window-level control
    virtual void SetWindowControl(std::shared_ptr<WindowControl> w) = 0;

    // Startup and shutdown control - fine-grained enough to support all app classes.
    virtual void Start() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;
    virtual void Stop() = 0;

    // TODO - perhaps these should go to the (currently empty) platform header?
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

    virtual ~CoreServices() = default;

};

} // namespace navitab
