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

#include "navitab/core.h"
#include "navitab/logger.h"
#include <nlohmann/json_fwd.hpp>
#include <filesystem>
#include <mutex>

// This header file defines a class that manages the Navitab preferences.
// The preferences are stored in a json formatted file which is loaded at
// startup and kept in memory as an nlohmann/json container object.

namespace navitab {

class PrefsManager : public Preferences
{
public:
    PrefsManager(std::filesystem::path prefsFile);
    ~PrefsManager();

    const nlohmann::json& Get(const std::string key) override;
    void Put(const std::string key, nlohmann::json& value) override;

private:
    void init();
    void load();
    void upgrade();
    void save();

private:
    std::filesystem::path prefsFile;
    std::shared_ptr<nlohmann::json> prefData;
    std::unique_ptr<logging::Logger> LOG;
    bool saveAtExit;
    std::mutex stateMutex;

};

} // namespace navitab
