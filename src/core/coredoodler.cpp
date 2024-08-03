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

#include "coredoodler.h"
#include "navitab.h"
#include "../win/imagerect.h"

namespace navitab {

CoreDoodler::CoreDoodler(std::shared_ptr<Navitab> c)
:   core(c),
    LOG(std::make_unique<logging::Logger>("doodler"))
{
}

CoreDoodler::~CoreDoodler()
{
}

void CoreDoodler::SetWindow(std::shared_ptr<Window> w)
{
    window = w;
}

void CoreDoodler::AsyncCall(std::function<void()> f)
{
    core->AsyncCall(f);
}

void CoreDoodler::onEnable()
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::onDisable()
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::onDoodlerResize(int width, int height)
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::onKeyEvent(int c)
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::Redraw()
{
    if (!dirty) return;

    // TODO - do the drawing work here
    dirty = false;

    // do the image buffer swap with the window
    image = window->RefreshDoodler(std::move(image));
}

} // namespace navitab
