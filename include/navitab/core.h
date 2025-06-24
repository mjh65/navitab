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

struct PathServices;
struct Simulator2Core;
struct WindowPart;
class Toolbar;
class Toolbar2Core;
class Modebar;
class Modebar2Core;
class Doodler;
class Doodler2Core;
class Keypad;
class Keypad2Core;
struct WindowControls;

enum HostPlatform { WIN, LNX, MAC };
enum WinServer { PLUGIN, DESKTOP, HTTP };
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

// The Settings class is a relatively small wrapper that can be used by
// Navitab components to fetch (and update) settings and user preference
// data that should be persistent between launches.

struct Settings
{
    virtual const nlohmann::json& Get(const std::string key) = 0;
    virtual void Put(const std::string key, nlohmann::json& value) = 0;
};

// The CoreServices interface is the centralised componment management interface for the
// Navitab system, and provides the connectivity by sharing interfaces needed by the
// simulation and windowing UI subsystems.

struct CoreServices
{
    // The executable / plugin's main() function should call the factory to create
    // exactly one instance of the Navitab core, and then destroy it on closure.
    static std::shared_ptr<CoreServices> MakeNavitab(SimEngine s, WinServer w);

    // Get the interface to the preferences manager
    virtual std::shared_ptr<Settings> GetSettingsManager() = 0;

    // Get the interface to the paths service
    virtual std::shared_ptr<PathServices> GetPathService() = 0;

    // Get the interface for simulation-generated events that Navitab will handle
    virtual std::shared_ptr<Simulator2Core> GetSimulatorCallbacks() = 0;

    // Register window parts for the core to interface with. This is used by the
    // http variant of the window manager which implements high-level versions of
    // the toolbar, modebar, doodler, and keypad.
    virtual std::shared_ptr<Toolbar2Core> SetToolbar(std::shared_ptr<Toolbar> t) = 0;
    virtual std::shared_ptr<Modebar2Core> SetModebar(std::shared_ptr<Modebar> m) = 0;
    virtual std::shared_ptr<Doodler2Core> SetDoodler(std::shared_ptr<Doodler> d) = 0;
    virtual std::shared_ptr<Keypad2Core> SetKeypad(std::shared_ptr<Keypad> k) = 0;

    // Get the window part for sending UI-window-generated events. All parts (toolbar,
    // modebar, doodler, keypad, canvas) have the same interface, so one function fits all.
    virtual std::shared_ptr<WindowPart> GetToolbar() = 0;
    virtual std::shared_ptr<WindowPart> GetModebar() = 0;
    virtual std::shared_ptr<WindowPart> GetDoodler() = 0;
    virtual std::shared_ptr<WindowPart> GetKeypad() = 0;
    virtual std::shared_ptr<WindowPart> GetCanvas() = 0;

    // Set the interface to the UI-window for window-level control
    virtual void SetWindowControl(std::shared_ptr<WindowControls> w) = 0;

    // Startup and shutdown control - fine-grained enough to support all app classes.
    virtual void Start() = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual void Stop() = 0;

    virtual ~CoreServices() = default;
};

// The AppServices interface provides the services used by the Navitab apps.

struct AppServices
{
    virtual void EnableTools(int toolMask, int repeatersMask) = 0;
    
    virtual ~AppServices() = default;
};

} // namespace navitab
