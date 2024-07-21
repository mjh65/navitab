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

#include "prefs.h"
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <fstream>
#include <sstream>


using json = nlohmann::json;

namespace navitab {
namespace core {

static const int PREFS_VERSION = 1;

Prefs::Prefs(std::filesystem::path pf)
:   prefsFile(pf),
    saveAtExit(true)
{
    LOG = std::make_unique<navitab::logging::Logger>("prefs");
    prefData = std::make_shared<json>();
    init();
    load();
    upgrade();
}

Prefs::~Prefs()
{
    if (saveAtExit) save();
}

const nlohmann::json& Prefs::Get(const std::string key)
{
    json::json_pointer k(key);
    auto& v = (*prefData)[k];
    return v;
}

void Prefs::Put(const std::string key, nlohmann::json& value)
{
    json::json_pointer k(key);
    (*prefData)[k] = value;
}

void Prefs::init()
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
                    "FATAL" : "F&2",
                    "ERROR" : "F&2",
                    "STATUS" : "F&1",
                    "WARN" : "F&1",
                    "INFO" : "N",
                    "DETAIL" : "N"
                }
            ]
        }
    })";

    *prefData = json::parse(jsonDefault);
}

void Prefs::load()
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
        filedata = json::parse(fin);
    }
    catch (const std::exception& e) {
        if (fileHasContent) {
            LOGW(fmt::format("Parsing error in preferences file {}", prefsFile.string()));
            LOGW(fmt::format("Default preferences will be used, and any updates will not be saved."));
            saveAtExit = false;
        } else {
            LOGW(fmt::format("Non-existent or empty preferences file {}", prefsFile.string()));
            LOGW(fmt::format("Default preferences will be used and saved on exit."));
        }
        return;
    }
    // TODO - use flatten and unflatten to improve the overriding granularity
    for (const auto& i : filedata.items()) {
        (*prefData)[i.key()] = i.value();
    }
    LOGS(fmt::format("Loaded preferences from {}", prefsFile.string()));
}

void Prefs::upgrade()
{
}

void Prefs::save()
{
    try {
        // flatten the preferences json and iterate through it to remove
        // any null values that might have crept in.
        auto flatPrefs = (*prefData).flatten();
        auto i = flatPrefs.begin();
        while (i != flatPrefs.end()) {
            if (i.value().is_null()) {
                i = flatPrefs.erase(i);
            } else {
                ++i;
            }
        }
        // then rebuild the original structure and save it
        std::ofstream fout(prefsFile);
        fout << std::setw(4) << flatPrefs.unflatten();
    }
    catch (const std::exception& e) {
        LOGW(fmt::format("Prefs file {} could not be saved", prefsFile.string()));
    }
    LOGS(fmt::format("Saved preferences to {}", prefsFile.string()));
}


} // namespace core
} // namespace navitab
