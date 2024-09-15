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
#include "navitab/logger.h"

namespace lvglkit {
class Manager;
}

namespace navitab {

class CoreServices;

class App
{
public:
    App(const char *name, std::shared_ptr<CoreServices> core, std::shared_ptr<lvglkit::Manager> gui);
    virtual ~App() = default;

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

protected:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<lvglkit::Manager> gui;

};

} // namespace navitab
