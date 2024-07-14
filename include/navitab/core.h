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

// This header file defines a class that manages the startup and use of the
// Navitab subsystems. Each of the executable/plugin's main() function should
// instantiate one of these 

namespace navitab {
namespace core {

enum HostPlatform { WIN, LNX, MAC };
enum AppClass { PLUGIN, DESKTOP, CONSOLE };
enum Simulation { NONE, MSFS, XPLANE };

class SubSystems
{
public:
    SubSystems(Simulation s, AppClass c);
    ~SubSystems() = default;

    void early_init();
    void full_init();

private:
    const HostPlatform  mHost;
    const AppClass      mAppClass;
    const Simulation    mSim;

};

} // namespace core
} // namespace navitab
