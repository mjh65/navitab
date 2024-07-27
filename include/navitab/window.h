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

 /*
  * This header file defines the interface to the UI window, which handles the
  * drawing of updated frame buffers, or fragments, and is the source of UI
  * events.
 */

namespace navitab {

class Preferences;

// The WindowEvents interface is how the UI window implementation provides
// events to and gets UI updates from the Navitab core.

struct WindowEvents
{

    // called when the window is resized.
    virtual void onWindowResize(int width, int height) = 0;

    // called when mouse events occur
    virtual void onMouseEvent(int x, int y, bool l, bool r) = 0;

    // called when scroll wheel events occur
    virtual void onWheelEvent(int x, int y, int xdir, int ydir) = 0;

    // called when key events occur
    virtual void onKeyEvent(char code) = 0;

    // TODO the frame buffer interface needs some thought, just have a placeholder for now
    // called to check for redraw
    // TODO - define parameters
    virtual bool getUpdateRegion() = 0;
};


// The Window interface defines the services that the UI window provides to
// the Navitab core.

struct Window
{

    // Factory function to create a GUI window object. There will be
    // one of these in each of the simulator-specific libraries.
    // TODO - use a shared_ptr for the callbacks
    static std::shared_ptr<Window> New(std::shared_ptr<Preferences> prefs);

    virtual void Disconnect() = 0;

    virtual ~Window() = default;
};

} // namespace navitab
