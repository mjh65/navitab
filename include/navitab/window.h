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
struct Window;

// The WindowEvents interface is how the UI window implementation provides
// events to and gets UI updates from the Navitab core.

struct WindowEvents
{
    // Set the window that Navitab will work with. May be called several times
    // if the client window changes, eg switching to/from VR.
    virtual void SetWindow(std::shared_ptr<Window>) = 0;

    // UI-triggered events notified to the Navitab core for further handling

    // Called at start, and then whenever the window is resized.
    virtual void onWindowResize(int width, int height) = 0;

    // Called when a mouse event occurs. Includes movement while a button is down
    virtual void onMouseEvent(int x, int y, bool l, bool r) = 0;

    // Called when scroll wheel events occur.
    virtual void onWheelEvent(int x, int y, int xdir, int ydir) = 0;

    // Called when key events occur.
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
    static std::shared_ptr<Window> Factory();

    // APIs called from the application/plugin
    virtual void SetPrefs(std::shared_ptr<Preferences> prefs) = 0;
    virtual void Connect(std::shared_ptr<WindowEvents> core) = 0;
    virtual void Disconnect() = 0;

    // Access to the window from Navitab core
    // ...
    virtual int FrameRate() = 0;

    virtual ~Window() = default;
};

} // namespace navitab
