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

// The Window class acts as a container for the 5 parts of the UI:
// the Canvas, Toolbar, Modebar, Doodlepad, and Keypad.
// The architecture allows the Toolbar, Modebar, Doodlepad and Keypad
// to be implemented remotely (specifically directly in the html/jscript
// of the MSFS panel), so there are separate interfaces defined for these.
// But the Canvas represents the generic drawing area for the Navitab
// apps/modes. It doesn't have enough special behaviour, so its interface
// is just wrapped into the Window class.

namespace navitab {

class Preferences;
struct Window;
struct Toolbar;
struct Modebar;
struct Doodlepad;
struct Keypad;

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
// the Navitab core. Mainly these are high-level connectivity and event loop

struct Window
{
    enum {
        WIN_MIN_WIDTH = 400,
        WIN_STD_WIDTH = 600,
        WIN_MAX_WIDTH = 1000,
        WIN_MIN_HEIGHT = 200,
        WIN_STD_HEIGHT = 300,
        WIN_MAX_HEIGHT = 600,
        TOOLBAR_HEIGHT = 40,
        MODEBAR_WIDTH = 60
    };

    // Factory function to create a GUI window object. There will be
    // one of these in each of the simulator-specific libraries.
    static std::shared_ptr<Window> Factory();

    // APIs called from the application/plugin

    virtual void SetPrefs(std::shared_ptr<Preferences> prefs) = 0;
    virtual void Connect(std::shared_ptr<WindowEvents> core) = 0;
    virtual void Disconnect() = 0;

    // Run some iterations of the event loop
    // returns the number of events in the queue, or -1 if finished
    virtual int EventLoop(int maxLoops = 1) = 0;

    // APIs called from Navitab core

    // Get the different parts of the window to talk to
    virtual std::shared_ptr<Toolbar> GetToolbar() = 0;
    virtual std::shared_ptr<Modebar> GetModebar() = 0;
    virtual std::shared_ptr<Doodlepad> GetDoodlepad() = 0;
    virtual std::shared_ptr<Keypad> GetKeypad() = 0;
    virtual int FrameRate() = 0;
    virtual void Brightness(int percent) = 0;

    virtual ~Window() = default;
};

} // namespace navitab
