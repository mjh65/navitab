/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <map>
#include "navitab/geometrics.h"
#include "navitab/logger.h"

// This header file defines the interface for the maps provider which
// manages local images and downloaded slippy tiles.

namespace navitab {

class RasterTile;
struct CoreServices;
class DocumentManager;
class TileProviderConfigLoader;
struct OnlineSlippyMapConfig;
struct Settings;

class MapTileProvider
{
public:
    MapTileProvider(std::shared_ptr<CoreServices>, std::shared_ptr<DocumentManager>);
    ~MapTileProvider();

    void SetZoom(unsigned z);
    unsigned GetZoom();

    std::shared_ptr<RasterTile> GetTile(double y, double x);
    std::pair<unsigned, unsigned> GetTileDimensions() const; // height,width

    std::pair<double, double> GetTileMaxYX() const; // vertical,horizontal
    std::pair<double, double> Location2TileYX(const Location &loc);
    Location TileYX2Location(double y, double x);

    std::pair<double, double> GetTileSpanRadians(double y); // angular span is independent of x
    //double GetTileHeightRadians(double y);
    //double GetTileWidthRadians(); // tile width is only dependent on zoom level

    void MaintenanceTick();

private:
    struct CachedTile {
        CachedTile() = default;
        CachedTile(std::shared_ptr<RasterTile> t) : tile(t), useCount(1) { }
        std::shared_ptr<RasterTile> tile;
        int useCount;
    };

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<TileProviderConfigLoader> providerCfg;
    std::shared_ptr<OnlineSlippyMapConfig> smapConfig;
    std::shared_ptr<Settings> prefs;
    std::shared_ptr<DocumentManager> docMgr;
    std::map<std::pair<int, int>, CachedTile> tileCache;
    std::shared_ptr<RasterTile> missingTile;
    unsigned zoom;
};

} // namespace navitab
