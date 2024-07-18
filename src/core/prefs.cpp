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
#include <sstream>


using json = nlohmann::json;

namespace navitab {
namespace core {

static const int PREFS_VERSION = 1;

Preferences::Preferences(std::filesystem::path pf)
:   prefsFile(pf),
    saveAtExit(true)
{
    log = std::make_unique<navitab::logging::Logger>("prefs");
    prefData = std::make_shared<json>();
    init();
    load();
    upgrade();
}

Preferences::~Preferences()
{
    if (saveAtExit) save();
}

void Preferences::init()
{
    // these are the default preferences, ie anything that's not null, false, or zero.
    // anything in the real preferences file will override entries in here, and
    // further entries may be added via the UI or code later.
    const char* jsonDefault = R"({
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
    })";

    std::istringstream iss(jsonDefault);
    json defaultData;
    iss >> defaultData;

    for (const auto& i : defaultData.items()) {
        (*prefData)[i.key()] = i.value();
    }
}

void Preferences::load()
{
    bool fileHasContent = false;
    json filedata;
    try {
        char x;
        std::ifstream fin(prefsFile);
        if (fin >> x) {
            fileHasContent = true;
            fin.clear();
            fin.seekg(0, std::ios_base::beg);
        }
        fin >> filedata;
    }
    catch (const std::exception& e) {
        if (fileHasContent) {
            zWARN((*log), fmt::format("Parsing error in preferences file {}", prefsFile.string()));
            zWARN((*log), fmt::format("Default preferences will be used, and any updates will not be saved."));
            saveAtExit = false;
        } else {
            zWARN((*log), fmt::format("Non-existent or empty preferences file {}", prefsFile.string()));
            zWARN((*log), fmt::format("Default preferences will be used and saved on exit."));
        }
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
