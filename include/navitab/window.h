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
#include "callback.h"

// The Window class acts as a container for the 5 parts of the UI:
// the Canvas, Toolbar, Modebar, Doodler, and Keypad.
// The architecture allows the Toolbar, Modebar, Doodler and Keypad
// to be implemented remotely (specifically directly in the html/jscript
// of the MSFS panel), so there are separate interfaces defined for these.
// But the Canvas represents the generic drawing area for the Navitab
// apps/modes. It doesn't have enough special behaviour, so its interface
// is just wrapped into the Window class.

namespace navitab {

struct CoreServices;
class Preferences;
struct Window;
struct PartPainter;
struct Toolbar;
struct Modebar;
struct Doodler;
struct Keypad;
class ImageRectangle;

// Each window part (toolbar, modebar, canvas, doodler, keypad) implements
// this interface so that the window manager can pass on UI events of interest.
struct WindowPart : public Callback
{
    // Set the interface to the painter this window part should work with.
    // This may be called 'mid-flight' as the client window can change when
    // switching to/from VR.
    void SetPainter(std::shared_ptr<PartPainter> p) {
        AsyncCall([this, p]() { onSetPainter(p); });
    }
    void PostResize(int w, int h) {
        AsyncCall([this, w, h]() { onResize(w, h); });
    }
    void PostMouseEvent(int x, int y, bool l, bool r) {
        AsyncCall([this, x, y, l, r]() { onMouseEvent(x, y, l, r); });
    }
    void PostWheelEvent(int x, int y, int xdir, int ydir) {
        AsyncCall([this, x, y, xdir, ydir]() { onWheelEvent(x, y, xdir, ydir); });
    }
    void PostKeyEvent(int code) {
        AsyncCall([this, code]() { onKeyEvent(code); });
    }

protected:
    void onSetPainter(std::shared_ptr<PartPainter> p) {
        painter = p;
    }

    // Called at start, and then whenever the window part is resized.
    virtual void onResize(int width, int height) = 0;

    // Called when a mouse event occurs. Includes movement while a button is down.
    // Position coordinates are relative to canvas top-left.
    virtual void onMouseEvent(int x, int y, bool l, bool r) = 0;

    // Called when scroll wheel events occur.
    virtual void onWheelEvent(int x, int y, int xdir, int ydir) = 0;

    // Called when key events occur.
    virtual void onKeyEvent(int code) = 0;

protected:
    std::shared_ptr<PartPainter> painter;

};

// The PartPainter interface defines the services the UI window provides to the
// window parts.

struct PartPainter
{
    // This is called whenever one of the window parts has been updated and needs repainting.
    // The design uses double-buffering, with the window owning the ImageRectangle
    // that is currently being drawn, and the part owning the other one that is being
    // updated. It is the responsibility of the part to size the ImageRectangle to match
    // the display aperture. The window will simply stretch/compress the ImageRectangle
    // to fit.
    virtual std::unique_ptr<ImageRectangle> RefreshPart(int part, std::unique_ptr<ImageRectangle>) = 0;
};

// The WindowControl interface defines the services the UI window provides to the
// Navitab core system.

struct WindowControl
{
    // Adjust the brightness of the display
    virtual void Brightness(int percent) = 0;
};

// The Window interface defines the services that the UI window provides
// to the application / plugin.

struct Window
{
    enum {
        TOOLBAR_HEIGHT = 32,
        MODEBAR_WIDTH = 40,
        MODEBAR_HEIGHT = MODEBAR_WIDTH * 8, // 8 mode selectors
        KEYPAD_HEIGHT = 200,
        WIN_MIN_WIDTH = 400,
        WIN_STD_WIDTH = 800,
        WIN_MAX_WIDTH = 1600,
        WIN_MIN_HEIGHT = TOOLBAR_HEIGHT + MODEBAR_HEIGHT,
        WIN_STD_HEIGHT = 480,
        WIN_MAX_HEIGHT = 1000,
    };

    enum {
        PART_CANVAS,
        PART_TOOLBAR,
        PART_MODEBAR,
        PART_DOODLER,
        PART_KEYPAD,
        PART_COUNT
    };


    // ===============================================================
    // Factory function to create a GUI window object. There will be
    // one of these in each of the simulator-specific libraries.
    static std::shared_ptr<Window> Factory();

    // ===============================================================
    // APIs called from the application/plugin

    // Initialisation and shutdown of the window.
    virtual void Connect(std::shared_ptr<CoreServices> core) = 0;
    virtual void Disconnect() = 0;

    // Run some iterations of the event loop
    // returns the number of events in the queue, or -1 if finished
    virtual int EventLoop(int maxLoops = 1) = 0;

    virtual ~Window() = default;
};

} // namespace navitab
