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

namespace navitab {

CoreToolbar::CoreToolbar(std::shared_ptr<Toolbar2Core> c, std::shared_ptr<lvglkit::Manager> u)
:   LOG(std::make_unique<logging::Logger>("toolbar")),
    core(c), uiMgr(u)
{
    uiDisplay = uiMgr->MakeDisplay();
}

CoreToolbar::~CoreToolbar()
{
}

void CoreToolbar::SetSimZuluTime(int h, int m, int s)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::SetFrameRate(int fps)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::SetEnabledTools(int selectMask)
{
    UNIMPLEMENTED(__func__);
}

void CoreToolbar::onResize(int w, int)
{
    width = w; height = Window::TOOLBAR_HEIGHT;

    // if the toolbar is resized then the previous image is just abandoned
    // and a new one is created and scheduled for redrawing
    image = std::make_unique<FrameBuffer>(width, height);
    image->Clear(0xffd0d0d0);
    // this is temporary code during initial development
    for (int y = 0; y < Window::TOOLBAR_HEIGHT; ++y) {
        auto r = image->Row(y);
        for (int i = 1; i < 10; ++i) {
            auto x = (width - (i * Window::TOOLBAR_HEIGHT));
            *(r + x) = 0xff00ff00;
        }
    }

    // the toolbar uses LVGL to draw the text that's displayed across the top
    uiDisplay->Resize(width, height, image->Row(0));

    dirtyBits.push_back(FrameRegion(0, 0, width, height));
    RunLater([this]() { Redraw(); });
}

void CoreToolbar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
