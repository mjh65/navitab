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

#include "navitab/logging.h"
#include <nlohmann/json_fwd.hpp>
#include <filesystem>

// This header file defines a class that manages the Navitab preferences.
// The preferences are stored in a json formatted file which is loaded at
// startup and kept in memory as an nlohmann/json container object.

namespace navitab {
namespace core {

class Preferences
{
public:
    Preferences(std::filesystem::path prefsFile);
    ~Preferences();

private:
    void init();
    void load();
    void upgrade();
    void save();

private:
    std::unique_ptr<navitab::logging::Logger> log;
    std::filesystem::path prefsFile;
    std::shared_ptr<nlohmann::json> prefData;
    bool saveAtExit;

};

} // namespace core
} // namespace navitab
