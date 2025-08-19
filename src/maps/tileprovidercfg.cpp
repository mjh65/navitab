#include "tileprovidercfg.h"
#include "tileprovidercfg.h"
/* This file is part of the Navitab project. See the README and LICENSE for details. */
/* Portions Copyright (C) 2023 Vangelis Tasoulas <cyberang3l@gmail.com> */

#include "tileprovidercfg.h"
#include <nlohmann/json.hpp>
#include <fmt/core.h>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace navitab {

TileProviderConfigLoader::TileProviderConfigLoader(std::filesystem::path cfg)
:   LOG(std::make_unique<logging::Logger>("tilecfg")),
    configFilePath(cfg)
{
    // use C++17 flag x to create the file if it does not exist
    FILE* fp = nullptr;
    fp = fopen(cfg.string().c_str(), "wx");
    if (fp) {
        const std::string def = defaultConfig();
        fwrite(def.c_str(), 1, def.size(), fp);
        fclose(fp);
    }
    
    auto jcfg = LoadConfig();
    Populate(jcfg);

    // if there were no valid tile servers, then we create a bogus one that will keep the
    // map server happy, but effectively does nothing.
    if (smConfigs.empty()) {
        LOGE("No valid tile server configurations were found. Maps will be blank.");
        auto tscfg = std::make_shared<OnlineSlippyMapConfig>();
        tscfg->copyright = "";
        tscfg->servers.push_back("127.0.0.1");
        tscfg->protocol = "http";
        tscfg->url = "{z}/{y}/{x}.png";
        tscfg->minZoomLevel = 3;
        tscfg->maxZoomLevel = 12;
        tscfg->tileWidthPx = 256;
        tscfg->tileHeightPx = 256;
        smConfigs[""] = tscfg;
    }
}

TileProviderConfigLoader::~TileProviderConfigLoader()
{
}

std::vector<std::string> TileProviderConfigLoader::GetNames()
{
    std::vector<std::string> names;
    for (auto& it : smConfigs) {
        names.push_back(it.first);
    }
    assert(names.size());
    return names;
}

std::shared_ptr<OnlineSlippyMapConfig> TileProviderConfigLoader::GetConfig(const std::string& name)
{
    if (name.empty()) return smConfigs.begin()->second;
    return smConfigs.at(name); // might throw, should be caught!
}

json TileProviderConfigLoader::LoadConfig()
{
    json filedata;
    try {
        std::ifstream fin(configFilePath);
        filedata = json::parse(fin);
    }
    catch (const std::exception& e) {
        LOGE(fmt::format("Invalid json format: {}", configFilePath.string()));
    }
    return filedata;
}

template<typename T>
static void getKey(json j, std::string key, T& value)
{
    try {
        value = j[key];
    }
    catch(const std::exception& e) {
        if (j.find(key) != j.end()) {
            throw std::runtime_error(fmt::format("key '{}' has bad value"));
        } else {
            throw std::runtime_error(fmt::format("mandatory key '{}' is missing"));
        }
    }
}

template<typename V, typename D>
static void getKey(json j, std::string key, V& value, D d)
{
    try {
        value = j[key];
    }
    catch(const std::exception& e) {
        value = d;
    }
}

void TileProviderConfigLoader::Populate(json cfg)
{
    // top-level item should be an array.
    if (!cfg.is_array()) {
        LOGE("Tile servers config top-level is not an array, giving up now");
    }
    // iterate over the array checking each member.
    // if the entries are sufficient to construct a usable configuration
    // then add that to the collection, otherwise report errors but dont throw exceptions
    unsigned i = 0;
    for (auto& ts : cfg) {
        std::string name = fmt::format("Entry #{}", ++i);
        auto tscfg = std::make_shared<OnlineSlippyMapConfig>();
        try {
            if (!ts.is_object()) {
                throw std::runtime_error(fmt::format("{} is not a json object"));
            }
            getKey(ts, "name", name);
            if (name.empty()) throw std::runtime_error("name cannot be blank");
            bool disabled = false;
            getKey(ts, "disabled", disabled, false);
            if (disabled) {
                LOGI(fmt::format("Ignoring disabled tile server {}", name));
                continue;
            }
            getKey(ts, "copyright", tscfg->copyright, "No copyright declared");
            getKey(ts, "protocol", tscfg->protocol, "https");
            getKey(ts, "url", tscfg->url);
            getKey(ts, "min_zoom_level", tscfg->minZoomLevel, 1);
            getKey(ts, "max_zoom_level", tscfg->maxZoomLevel, 12);
            getKey(ts, "tile_width_px", tscfg->tileWidthPx, 256);
            getKey(ts, "tile_height_px", tscfg->tileHeightPx, 256);
            auto tsit = ts.find("servers");
            if (tsit == ts.end()) {
                throw std::runtime_error("namdatory key 'servers' is missing");
            }
            if (!tsit->is_array()) {
                throw std::runtime_error("key 'servers' has bad value");
            }
            for (auto& tshost : *tsit) {
                if (!tshost.is_string()) {
                    // log, ignore, continue
                    LOGE(fmt::format("Ignoring nonstring item in servers list for {}", name));
                } else {
                    tscfg->servers.push_back(tshost);
                }
            }
            // now sanitize/validate the values that were read. exceptions may be
            // thrown, which will result in this item being ignored.
            tscfg->Validate();
        }
        catch (const std::exception& e) {
            LOGE(fmt::format("Tile server {} configuration error: {}", name, e.what()));
        }
        // if we reached this point then the config passed all of our checks
        // and is good enough to use.
        smConfigs[name] = tscfg;
    }
}

void OnlineSlippyMapConfig::Validate()
{
    // remove any trailing /s in the servers list
    for (auto s: servers) {
        s.erase(s.find_last_not_of('/') + 1, std::string::npos);
    }
    // remove any blank entries
    std::sort(servers.begin(), servers.end(),
             [](const std::string& a, const std::string& b) { return a.size() > b.size(); });
    while (servers.size() && servers.back().empty()) {
        servers.pop_back();
    }
    if (servers.empty()) {
        throw std::runtime_error("array 'servers' has no usable entries");
    }
    // remove any leading /s from the url
    url.erase(0, std::min(url.find_first_not_of('/'), url.size() - 1));
    // convert url to a set of strings suitable for quick assembly, and save
    // the order of the x, y, z placeholders for when FormatUrl is used.
    auto xpos = url.find("{x}");
    auto ypos = url.find("{y}");
    auto zpos = url.find("{z}");
    if (xpos == std::string::npos || ypos == std::string::npos || zpos == std::string::npos) {
        throw std::runtime_error("url requires all placeholders {z}, {x}, {y}");
    }
    // enum { XYZ, XZY, YXZ, YZX, ZXY, ZYX } fmtOrder;
    if (xpos < ypos) {
        // one of XYZ, XZY, ZXY
        if (zpos < xpos) {
            fmtOrder = ZXY;
            splitUrl(zpos, xpos, ypos);
        } else if (zpos < ypos) {
            fmtOrder = XZY;
            splitUrl(xpos, zpos, ypos);
        } else {
            fmtOrder = XYZ;
            splitUrl(xpos, ypos, zpos);
        }
    } else {
        // one of YXZ, YZX, ZYX
        if (zpos < ypos) {
            fmtOrder = ZYX;
            splitUrl(zpos, ypos, xpos);
        } else if (zpos < xpos) {
            fmtOrder = YZX;
            splitUrl(ypos, zpos, xpos);
        } else {
            fmtOrder = YXZ;
            splitUrl(ypos, xpos, zpos);
        }
    }
    // convert protocol to lower case and then check supported
    std::transform(protocol.begin(), protocol.end(), protocol.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if ((protocol != "https") && (protocol != "http")) {
        throw std::runtime_error("protocol must be one of https or http");
    }
    if ((minZoomLevel < 0) || (minZoomLevel > maxZoomLevel) || (maxZoomLevel > 18)) {
        throw std::runtime_error("zoom level ranges are not sensible");
    }
    if ((tileWidthPx != 256) || (tileHeightPx != 256)) {
        throw std::runtime_error("Navitab does not currently support non-standard tile dimensions");
    }
}

void OnlineSlippyMapConfig::splitUrl(size_t p1, size_t p2, size_t p3)
{
    assert(p2 > (p1+3));
    assert(p3 > (p2+3));
    u0 = url.substr(0, p1);
    u1 = url.substr(p1+3, p2-(p1+3));
    u2 = url.substr(p2+3, p3-(p2+3));
    u3 = url.substr(p3+3);
}

std::string OnlineSlippyMapConfig::FormatUrl(unsigned zoom, int x, int y) const
{
    size_t si = 0;
    if (servers.size() > 1) si = rand() % servers.size();
    std::string base = protocol + "://" + servers[si] + "/";
    auto zs = std::to_string(zoom);
    auto xs = std::to_string(x);
    auto ys = std::to_string(y);
    switch(fmtOrder) {
        case XYZ:
            return base + u0 + xs + u1 + ys + u2 + zs + u3;
            break;
        case XZY:
            return base + u0 + xs + u1 + zs + u2 + ys + u3;
            break;
        case YXZ:
            return base + u0 + ys + u1 + xs + u2 + zs + u3;
            break;
        case YZX:
            return base + u0 + ys + u1 + zs + u2 + xs + u3;
            break;
        case ZXY:
            return base + u0 + zs + u1 + xs + u2 + ys + u3;
            break;
        case ZYX:
            return base + u0 + zs + u1 + ys + u2 + xs + u3;
            break;
    }
    return "";
}

std::string TileProviderConfigLoader::defaultConfig()
{
    return R"z([
    {
        "name": "OpenTopoMap",
        "servers": [
            "a.tile.opentopomap.org",
            "b.tile.opentopomap.org",
            "c.tile.opentopomap.org"
        ],
        "protocol": "https",
        "copyright": "Map Data (c) OpenStreetMap, SRTM - Map Style (c) OpenTopoMap (CC-BY-SA)",
        "url": "{z}/{x}/{y}.png",
        "min_zoom_level": 1,
        "max_zoom_level": 17,
        "tile_width_px": 256,
        "tile_height_px": 256
    },
    {
        "name": "OpenStreetMap",
        "servers": [
            "tile.openstreetmap.org"
        ],
        "protocol": "https",
        "copyright": "Map tiles (c) OpenStreetMap (ODbL)",
        "url": "{z}/{x}/{y}.png",
        "min_zoom_level": 1,
        "max_zoom_level": 17,
        "tile_width_px": 256,
        "tile_height_px": 256
    }
])z";
}

}
