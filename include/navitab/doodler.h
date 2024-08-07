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

// The Doodler class represents the user's doodling area. This is a transparent
// overlay to the canvas that can be drawn or typed on when it is activated in the
// mode bar.

namespace navitab {

struct Window;

struct DoodlerEvents : public Callback
{
    // The doodler doesn't really interact much with the rest of Navitab, so there's
    // nothing in here. Perhaps the interface will disappear?
};

struct Doodler : public WindowPart
{
    // API calls from Navitab core (these will be triggered from the Modebar click handler)
    virtual void Enable() = 0;
    virtual void Disable() = 0;

    virtual ~Doodler() = default;
};

} // namespace navitab
