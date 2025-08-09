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

#include <memory>
#include <map>
#include "navitab/navigation.h"
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

    void LatLon2TileXY(Location loc, double &x, double &y);
    void TileXY2LatLon(double x, double y, Location &loc);

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
