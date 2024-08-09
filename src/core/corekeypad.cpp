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

#include "corekeypad.h"
#include "navitab.h"

namespace navitab {

CoreKeypad::CoreKeypad(std::shared_ptr<KeypadEvents> c)
:   core(c),
    LOG(std::make_unique<logging::Logger>("keypad")),
    visible(false)
{
}

CoreKeypad::~CoreKeypad()
{
}

void CoreKeypad::Show()
{
    visible = true;
    AsyncCall([this]() { onResize(width, height); });
}

void CoreKeypad::Hide()
{
    visible = false;
    image.reset();
    AsyncCall([this]() { Redraw(); });
}

void CoreKeypad::onResize(int w, int h)
{
    width = w; height = h;
    if (!visible) return;

    image = std::make_unique<ImageRectangle>(width, height);
    // TODO - generate the basic keypad image, depending on dimensions

    AsyncCall([this]() { Redraw(); });
}

void CoreKeypad::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
