/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
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

#include "navitab/prefs.h"
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <fstream>


using json = nlohmann::json;

namespace navitab {
namespace core {

static const int PREFS_VERSION = 1;

Preferences::Preferences(std::filesystem::path pf)
:   prefsFile(pf)
{
    log = std::make_unique<navitab::logging::Logger>("prefs");
    prefData = std::make_shared<json>();
    init();
    load();
    upgrade();
}

Preferences::~Preferences()
{
    save();
}

void Preferences::init()
{
    // full init not required, just defaults that aren't zero/false/empty
#if 0 // TODO - change this to inline string data and stream it into the json (like the load() fn)
{
    "version": 1,
    "general" : {
        "show_fps": true
    },
    "logging" : {
        "filters": [
            {
                "pattern": "*",
                "FATAL" : "F+2",
                "ERROR" : "F+2",
                "STATUS" : "F+1",
                "WARN" : "F+1",
                "INFO" : "F",
                "DETAIL" : "F"
            }
        ]
    }
}
#endif
    *prefData = {
        { "general",
            {
                { "prefs_version", PREFS_VERSION },
                { "show_fps", true }
            }
        }
    };
}

void Preferences::load()
{
    json filedata;
    try {
        std::ifstream fin(prefsFile);
        fin >> filedata;
    }
    catch (const std::exception& e) {
        zWARN((*log), fmt::format("Prefs file {} could not be read, defaults will be used", prefsFile.string()));
        return;
    }
    for (const auto& i : filedata.items()) {
        (*prefData)[i.key()] = i.value();
    }
    zSTATUS((*log), fmt::format("Loaded preferences from {}", prefsFile.string()));
}

void Preferences::upgrade()
{
}

void Preferences::save()
{
    try {
        std::ofstream fout(prefsFile);
        fout << std::setw(4) << *prefData;
    }
    catch (const std::exception& e) {
        zWARN((*log), fmt::format("Prefs file {} could not be saved", prefsFile.string()));
    }
    zSTATUS((*log), fmt::format("Saved preferences to {}", prefsFile.string()));
}


} // namespace core
} // namespace navitab
