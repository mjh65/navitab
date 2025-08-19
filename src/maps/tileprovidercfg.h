/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/logger.h"
#include <nlohmann/json_fwd.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <map>

namespace navitab {

struct OnlineSlippyMapConfig {
    std::string copyright;
    std::vector<std::string> servers;
    std::string protocol;
    std::string url;
    unsigned minZoomLevel;
    unsigned maxZoomLevel;
    unsigned tileWidthPx;
    unsigned tileHeightPx;
    void Validate();
    std::string FormatUrl(unsigned zoom, int x, int y) const;
    // internal working state
    enum { XYZ, XZY, YXZ, YZX, ZXY, ZYX } fmtOrder;
    std::string u0, u1, u2, u3;
    void splitUrl(size_t p1, size_t p2, size_t p3);
};

class TileProviderConfigLoader
{
public:
    TileProviderConfigLoader(std::filesystem::path cfg);
    ~TileProviderConfigLoader();

    std::vector<std::string> GetNames();
    std::shared_ptr<OnlineSlippyMapConfig> GetConfig(const std::string &name);
    
private:
    nlohmann::json LoadConfig();
    void Populate(nlohmann::json);

    std::string defaultConfig();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::filesystem::path configFilePath;
    std::map<std::string, std::shared_ptr<OnlineSlippyMapConfig> > smConfigs;

};

} // namespace navitab
