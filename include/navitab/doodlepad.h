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

// The Doodlepad class represents the user's doodling area. This is a transparent
// overlay to the canvas that can be drawn or typed on when it is activated in the
// mode bar. It doesn't really interact much with the rest of Navitab, so maybe
// this interface will go away.

namespace navitab {

// The DoodlepadEvents interface is how the UI doodle pad implementation provides
// events to the Navitab core.

struct DoodlepadEvents
{
    // UI-triggered events notified to the Navitab core for further handling
    // this is empty, since the doodle pad is self-contained!
};

// The Doodlepad interface defines the services that the UI window provides to
// the Navitab core.

struct Doodlepad
{
    // APIs called from the window
    virtual void Enable() = 0;
    virtual void Disable() = 0;
    virtual void SelectEraser() = 0;
    virtual void SelectPencil() = 0;
    virtual void Wipe() = 0;

    virtual void MouseButtonLeft(int x, int y, bool down) = 0;
    virtual void MouseButtonRight(int x, int y, bool down) = 0;
    virtual void Keyboard(int c) = 0;

    virtual ~Doodlepad() = default;
};

} // namespace navitab
