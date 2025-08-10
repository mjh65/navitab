/* This file is part of the Navitab project. See the README and LICENSE for details. */

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

class SettingsManager : public Settings
{
public:
    SettingsManager(std::filesystem::path settingsFile);
    ~SettingsManager();

    const nlohmann::json& Get(const std::string key) override;
    void Put(const std::string key, nlohmann::json& value) override;

private:
    void init();
    void load();
    void upgrade();
    void save();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::filesystem::path settingsFilePath;
    std::shared_ptr<nlohmann::json> settingsJson;
    bool saveAtExit;
    std::mutex stateMutex;

};

} // namespace navitab
