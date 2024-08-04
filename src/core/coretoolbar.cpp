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

#include "coretoolbar.h"
#include "navitab.h"
#include "../win/imagerect.h"

namespace navitab {

CoreToolbar::CoreToolbar(std::shared_ptr<ToolbarEvents> c)
:   core(c),
    LOG(std::make_unique<logging::Logger>("toolbar"))
{
}

CoreToolbar::~CoreToolbar()
{
}

void CoreToolbar::SetSimZuluTime(int h, int m, int s)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::EnableTools(int selectMask)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::DisableTools(int selectMask)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::SetWindow(std::shared_ptr<Window> w)
{
    window = w;
}

void CoreToolbar::SetFrameRate(float fps)
{

}

void CoreToolbar::AsyncCall(std::function<void()> f)
{
    core->AsyncCall(f);
}

void CoreToolbar::onToolbarResize(int width)
{
    image = std::make_unique<ImageRectangle>(width, Window::TOOLBAR_HEIGHT);
    dirty = true;
    core->AsyncCall([this]() { Redraw(); });
}

void CoreToolbar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::Redraw()
{
    if (!dirty) return;

    image->Clear(0xffd0d0d0);
    // TODO - do the drawing work here
    dirty = false;

    // do the image buffer swap with the window
    image = window->RefreshToolbar(std::move(image));
}



} // namespace navitab
