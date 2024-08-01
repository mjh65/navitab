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

#include <memory>

// The Toolbar class represents the toolbar which is drawn across the top
// of the window. It is a fixed height, and displays some current status
// information on the left hand side, and some tool action icons from the
// right hand side which trigger behaviours in the currently active mode/app.

namespace navitab {

// The ToolbarEvents interface is how the UI toolbar implementation provides
// events to the Navitab core.

struct ToolbarEvents
{
    // UI-triggered events notified to the Navitab core for further handling

    enum Tool {
        MENU        = 0b1,
        MAGNIFY     = 0b10,
        REDUCE      = 0b100,
        CENTRE      = 0b1000,
        NEXT        = 0b10000,
        PREVIOUS    = 0b100000,
        DOWN        = 0b1000000,
        UP          = 0b10000000
    };

    // Called when a tool icon is clicked
    virtual void onToolClick(Tool) = 0;
};

// The Toolbar interface defines the services that the UI window provides to
// the Navitab core.

struct Toolbar
{
    // APIs called from the application/plugin
    virtual void SetFrameRate(float fps) = 0;
    virtual void SetSimZuluTime(int h, int m, int s) = 0;
    virtual void EnableTools(int selectMask) = 0;
    virtual void DisableTools(int selectMask) = 0;

    // APIs called from the window
    virtual void MouseButtonLeft(int x, int y, bool down) = 0;

    virtual ~Toolbar() = default;
};

} // namespace navitab