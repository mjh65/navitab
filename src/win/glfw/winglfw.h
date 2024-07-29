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

#pragma once

#include "navitab/window.h"

namespace navitab {

class WindowGLFW : public Window
{
public:
    WindowGLFW();
    ~WindowGLFW();

    // APIs called from the application/plugin
    void SetPrefs(std::shared_ptr<Preferences> prefs) override;
    void Connect(std::shared_ptr<WindowEvents> core) override;
    void Disconnect() override;

    // Access to the window from Navitab core
    // ...
    int FrameRate() override;


};

}
