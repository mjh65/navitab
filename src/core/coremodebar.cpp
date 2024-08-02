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

#include "coremodebar.h"
#include "navitab.h"
#include "../win/imagerect.h"

namespace navitab {

CoreModebar::CoreModebar(std::shared_ptr<Navitab> c)
:   core(c)
{
    image = std::make_unique<ImageRectangle>(Window::MODEBAR_WIDTH, Window::MODEBAR_HEIGHT);
}

CoreModebar::~CoreModebar()
{
}

void CoreModebar::SetWindow(std::shared_ptr<Window> w)
{
    window = w;
    dirty = true;
    core->AsyncCall([this]() { Redraw(); });
}

void CoreModebar::DisableDoodler()
{
}

void CoreModebar::ShowKeypad()
{
}

void CoreModebar::HideKeypad()
{
}

void CoreModebar::AsyncCall(std::function<void()> f)
{
    core->AsyncCall(f);
}

void CoreModebar::onMouseEvent(int x, int y, bool l, bool r)
{
}

void CoreModebar::Redraw()
{
    if (!dirty) return;

    image->Clear(0x40f0f0f0);
    // TODO - do the drawing work here
    dirty = false;

    // do the image buffer swap with the window
    image = window->RefreshModebar(std::move(image));
}

} // namespace navitab
