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

// The Doodler class represents the user's doodling area. This is a transparent
// overlay to the canvas that can be drawn or typed on when it is activated in the
// mode bar. It doesn't really interact much with the rest of Navitab, so maybe
// this interface will go away.

namespace navitab {

struct Window;

// TODO - seems like there is some shared behaviour between Toolbar, Modebar, Doodler and Keypad which should go into a base class.
// TODO - they all have an interface to the window, the core, and an async call handler.

struct Doodler : public Callback
{
    // APIs called from the window
    virtual void SetWindow(std::shared_ptr<Window> window) = 0;

    void PostResize(int w, int h) {
        AsyncCall([this, w, h]() { onDoodlerResize(w, h); });
    }
    void PostMouseEvent(int x, int y, bool l, bool r) {
        AsyncCall([this, x, y, l, r]() { onMouseEvent(x, y, l, r); });
    }
    void PostKeyEvent(int code) {
        AsyncCall([this, code]() { onKeyEvent(code); });
    }

    // Navitab core calls these (originate from the Modebar click handler)
    void Enable() { 
        AsyncCall([this]() { onEnable(); });
    }
    void Disable() { 
        AsyncCall([this]() { onDisable(); });
    }

    virtual ~Doodler() = default;

protected:
    virtual void onEnable() = 0;
    virtual void onDisable() = 0;
    virtual void onDoodlerResize(int width, int height) = 0;
    virtual void onMouseEvent(int x, int y, bool l, bool r) = 0;
    virtual void onKeyEvent(int c) = 0;

};

} // namespace navitab
