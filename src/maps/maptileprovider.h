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
class DocumentManager;

class MapTileProvider
{
public:
    MapTileProvider(std::shared_ptr<DocumentManager>);

    std::shared_ptr<RasterTile> GetTile(int x, int y);

    void MaintenanceTick();

    void SetZoom(unsigned z);
    unsigned GetZoom();

    std::pair<double, double> Location2TileYX(const Location &loc);
    Location TileYX2Location(double x, double y);

    double GetTileCentreWidthDegrees();
    double GetTileCentreWidthMetres();
    double GetTileHeightDegrees();
    double GetTileHeightMetres();

    virtual ~MapTileProvider() = default;

private:
    struct CachedTile {
        CachedTile() = default;
        CachedTile(std::shared_ptr<RasterTile> t) : tile(t), useCount(1) { }
        std::shared_ptr<RasterTile> tile;
        int useCount;
    };

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<DocumentManager> docMgr;
    std::map<std::pair<int, int>, CachedTile> tileCache;
    std::shared_ptr<RasterTile> missingTile;
    unsigned zoom;
};

} // namespace navitab
