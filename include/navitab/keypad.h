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

// The Keypad class represents an onscreen keyboard. This is a semi-transparent
// overlay to the canvas that generates key codes in reponse to mouse-clicks.
// The keypad can be displayed/hidden through the mode bar.

namespace navitab {

// The KeypadEvents interface is how the UI doodle pad implementation provides
// events to the Navitab core.

struct KeypadEvents
{
    // UI-triggered events notified to the Navitab core for further handling

    // Called when key events occur.
    virtual void onKeyEvent(char code) = 0;
};

// The Keypad interface defines the services that the UI window provides to
// the Navitab core.

struct Keypad
{
    // APIs called from the application/plugin
    virtual void Show() = 0;
    virtual void Hide() = 0;
    // other stuff, like setting any multi-character keys (nearest airports etc)

    virtual ~Keypad() = default;
};

} // namespace navitab
