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
#include "navitab/callback.h"
#include "navitab/window.h"

// The Modebar class represents the mode/app chooser which is drawn down the
// left hand side of the window. It is a fixed size, and contains a set of
// icons which represent the different modes of Navitab. Clicking on an icon
// changes the mode.

namespace navitab {

struct Window;

// The ModebarEvents interface is how the UI doodle pad implementation provides
// events to the Navitab core.

struct ModebarEvents : public Callback
{
    // UI-triggered events notified to the Navitab core for further handling

    enum Mode {
        // maybe we don't need these as bitmaskable?
        ABOUT_HELP  = 0b1,
        MAP         = 0b10,
        AIRPORT     = 0b100,
        ROUTE       = 0b1000,
        DOC_VIEWER  = 0b10000,
        SETTINGS    = 0b100000,
        DOODLER     = 0b1000000,
        KEYPAD      = 0b10000000
    };

    void PostModeSelect(Mode m) {
        AsyncCall([this, m]() { onModeSelect(m); });
    }

protected:
    // Called when a mode icon is clicked
    virtual void onModeSelect(Mode) = 0;
};

// The Modebar interface defines the services that the UI window provides to
// the Navitab core.

struct Modebar : public WindowPart
{
    // APIs called from the Navitab core
    virtual void SetHighlights(int selectMask) = 0;

    virtual ~Modebar() = default;
};

} // namespace navitab
