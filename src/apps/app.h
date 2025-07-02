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
#include <lvgl.h>
#include "navitab/logger.h"
#include "navitab/toolbar.h"

namespace lvglkit {
class Display;
}

namespace navitab {

struct AppServices;

class App
{
public:
    App(const char *name, std::shared_ptr<AppServices> core);
    virtual ~App();

    void Activate(std::shared_ptr<lvglkit::Display> display);
    void Deactivate();
    
    virtual void ToolClick(ClickableTool t) = 0;

protected:
    virtual void Assemble() = 0;
    virtual void Demolish() = 0;

    // switch the LVGL screen to make it active
    void Show();

protected:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<AppServices> core;
    std::shared_ptr<lvglkit::Display> display;

    // every app will have an LVGL screen as its root widget
    lv_obj_t * root;

    // every app will have a set of tools that it uses, some of
    // which will automatically repeat post if they are 'held down'
    int activeToolsMask;
    int repeatingToolsMask;
};

} // namespace navitab
