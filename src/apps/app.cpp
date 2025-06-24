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

#include "app.h"
#include "../lvglkit/display.h"

namespace navitab {

App::App(const char *name, std::shared_ptr<AppServices> c)
:   LOG(std::make_unique<logging::Logger>(name)),
    core(c),
    root(nullptr),
    defaultToolMask(0)
{
}

App::~App()
{
    if (root) {
        lv_obj_del(root);
        root = nullptr;
    }
}

void App::Activate(std::shared_ptr<lvglkit::Display> d)
{
    display = d;
    if (!root) {
        display->Select();
        root = lv_obj_create(nullptr);
        Assemble();
    }
    Show();
}

void App::Deactivate()
{
    Demolish();
}

void App::Show()
{
    display->Select();
    lv_scr_load(root);
}

} // namespace navitab
