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

#include "settingsmgr.h"
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <fstream>
#include <sstream>


using json = nlohmann::json;

namespace navitab {

static const int PREFS_VERSION = 1;

SettingsManager::SettingsManager(std::filesystem::path pf)
:   settingsFilePath(pf),
    settingsJson(std::make_shared<json>()),
    LOG(std::make_unique<logging::Logger>("prefs")),
    saveAtExit(true)
{
    init();
    load();
    upgrade();
}

SettingsManager::~SettingsManager()
{
    if (saveAtExit) save();
    LOGS("~SettingsManager() done");
}

const nlohmann::json& SettingsManager::Get(const std::string key)
{
    std::lock_guard<std::mutex> lock(stateMutex);
    json::json_pointer k(key);
    auto& v = (*settingsJson)[k];
    return v;
}

void SettingsManager::Put(const std::string key, nlohmann::json& value)
{
    std::lock_guard<std::mutex> lock(stateMutex);
    json::json_pointer k(key);
    (*settingsJson)[k] = value;
}

void SettingsManager::init()
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

    *settingsJson = json::parse(jsonDefault);
}

void SettingsManager::load()
{
    bool fileHasContent = false;
    json filedata;
    try {
        char x;
        std::ifstream fin(settingsFilePath);
        if (fin >> x) {
            fileHasContent = true;
            fin.clear();
            fin.seekg(0, std::ios_base::beg);
        }
        filedata = json::parse(fin);
    }
    catch (const std::exception& e) {
        if (fileHasContent) {
            LOGW(fmt::format("Parsing error in preferences file {}", settingsFilePath.string()));
            LOGW(fmt::format("Default preferences will be used, and any updates will not be saved."));
            saveAtExit = false;
        } else {
            LOGW(fmt::format("Non-existent or empty preferences file {}", settingsFilePath.string()));
            LOGW(fmt::format("Default preferences will be used and saved on exit."));
        }
        return;
    }
    // TODO - use flatten and unflatten to improve the overriding granularity
    for (const auto& i : filedata.items()) {
        (*settingsJson)[i.key()] = i.value();
    }
    LOGS(fmt::format("Loaded preferences from {}", settingsFilePath.string()));
}

void SettingsManager::upgrade()
{
}

void SettingsManager::save()
{
    try {
        // flatten the preferences json and iterate through it to remove
        // any null values that might have crept in.
        auto flatPrefs = (*settingsJson).flatten();
        auto i = flatPrefs.begin();
        while (i != flatPrefs.end()) {
            if (i.value().is_null()) {
                i = flatPrefs.erase(i);
            } else {
                ++i;
            }
        }
        // then rebuild the original structure and save it
        std::ofstream fout(settingsFilePath);
        fout << std::setw(4) << flatPrefs.unflatten();
    }
    catch (const std::exception& e) {
        LOGW(fmt::format("SettingsManager file {} could not be saved", settingsFilePath.string()));
    }
    LOGS(fmt::format("Saved preferences to {}", settingsFilePath.string()));
}

} // namespace navitab
