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
#include <functional>

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
    // Tell Navitab the interface to the window it will work with.
    // This may be called 'mid-flight' as the client window can change when
    // switching to/from VR.
    virtual void SetWindow(std::shared_ptr<Window>) = 0;

    // Get the interfaces for each of the window parts
    virtual std::shared_ptr<Toolbar> GetToolbar() = 0;
    virtual std::shared_ptr<Modebar> GetModebar() = 0;
    virtual std::shared_ptr<Doodlepad> GetDoodlepad() = 0;
    virtual std::shared_ptr<Keypad> GetKeypad() = 0;

    // UI-triggered events notified to the Navitab core.
    // These wrapper functions ensure that the UI thread is not stalled while
    // the event is being handled.
    void PostWindowResize(int w, int h) { 
        AsyncCall([this, w, h]() { this->onWindowResize(w, h); });
    }
    void PostMouseEvent(int x, int y, bool l, bool r) {
        AsyncCall([this, x, y, l, r]() { this->onMouseEvent(x, y, l, r); });
    }
    void PostWheelEvent(int x, int y, int xdir, int ydir) {
        AsyncCall([this, x, y, xdir, ydir]() { this->onWheelEvent(x, y, xdir, ydir); });
    }
    void PostKeyEvent(int code) {
        AsyncCall([this, code]() { this->onKeyEvent(code); });
    }

protected:
    // Most callbacks are wrapped in AsyncCall() to avoid stalling the UI.
    virtual void AsyncCall(std::function<void ()>) = 0;

    // Called whenever the window is resized, including at start to initialise.
    virtual void onWindowResize(int width, int height) = 0;

    // Called when a mouse event occurs. Includes movement while a button is down
    virtual void onMouseEvent(int x, int y, bool l, bool r) = 0;

    // Called when scroll wheel events occur.
    virtual void onWheelEvent(int x, int y, int xdir, int ydir) = 0;

    // Called when key events occur.
    virtual void onKeyEvent(int code) = 0;

};

// The Window interface defines the services that the UI window provides to
// the Navitab core. Mainly these are high-level connectivity and event loop

struct Window
{
    enum {
        WIN_MIN_WIDTH = 400,
        WIN_STD_WIDTH = 800,
        WIN_MAX_WIDTH = 1600,
        WIN_MIN_HEIGHT = 300,
        WIN_STD_HEIGHT = 400,
        WIN_MAX_HEIGHT = 1000,
        TOOLBAR_HEIGHT = 40,
        MODEBAR_WIDTH = 50,
        MODEBAR_HEIGHT = 400,
        KEYPAD_HEIGHT = 300
    };

    // ===============================================================
    // Factory function to create a GUI window object. There will be
    // one of these in each of the simulator-specific libraries.
    static std::shared_ptr<Window> Factory();

    // ===============================================================
    // APIs called from the application/plugin

    // Initialisation and shutdown of the window.
    virtual void SetPrefs(std::shared_ptr<Preferences> prefs) = 0;
    virtual void Connect(std::shared_ptr<WindowEvents> core) = 0;
    virtual void Disconnect() = 0;

    // Run some iterations of the event loop
    // returns the number of events in the queue, or -1 if finished
    virtual int EventLoop(int maxLoops = 1) = 0;

    // ===============================================================
    // APIs called from Navitab core

    // Adjust the brightness of the display
    virtual void Brightness(int percent) = 0;

    // Run a job in the Window's event loop.
    // MJH: Not sure if this will even be required?
    //virtual void RunInEventLoop(std::function<void ()>) = 0;

    virtual ~Window() = default;
};

} // namespace navitab
