/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "maptileprovider.h"
#include "navitab/tiles.h"
#include "../docs/docmanager.h"
#include "../docs/document.h"
#include <fmt/core.h>
#include <cmath>


double sec(double a) { return 1.0 / cos(a); }
double deg2rad(double d) { return d * M_PI / 180.0; }

namespace navitab {

MapTileProvider::MapTileProvider(std::shared_ptr<DocumentManager> d)
:   LOG(std::make_unique<logging::Logger>("maps")),
    docMgr(d),
    missingTile(nullptr),
    zoom(8)
{
    missingTile = std::make_shared<RasterTile>();
    for (unsigned r = 0; r < missingTile->Height(); ++r) {
        uint32_t *rs = missingTile->Row(r);
        for (unsigned c = 0; c < missingTile->Width(); ++c) {
            auto dark = ((r / 32) ^ (c / 32)) & 0x1;
            *(rs + c) = dark ? 0xff404040 : 0xffc0c0c0;
        }
    }
}

void MapTileProvider::MaintenanceTick()
{
    // called periodically so that the maps provider can drop tiles from
    // the cache if they are not being used.
    auto ci = tileCache.begin();
    while (ci != tileCache.end()) {
        auto uc = --(ci->second.useCount);
        // remove from the cache if unused for some time.
        if (uc < 0) {
            tileCache.erase(ci++);
        } else {
            ++ci;
        }
    }
}

std::shared_ptr<RasterTile> MapTileProvider::GetTile(int x, int y)
{
    // adjust the x index in case of overflow around the date line
    // (we let the y value overflow, it doesn't wrap)
    int xn = 1 << zoom;
    while (x < 0) x += xn;
    while (x >= xn) x -= xn;

    // do we have the requested tile in the cache?
    auto tci = tileCache.find(std::make_pair(x, y));
    if (tci != tileCache.end()) {
        ++(tci->second.useCount);
        return tci->second.tile;
    }

    // TODO - might want to iterate to lower zoom levels and then draw more
    // blurred map until the detailed one appears. the idea here would be to
    // enhance the cache indexing from a simple x,y to x,y,z where z ranges from 0 (natural zoom) to (eg) 4.
    
    // TODO - obviously this needs to use the mapconfig stuff from Avitab rather than hard coding

    // tile is not in the cache. request it from the Document Manager.
    std::string url = fmt::format("https://tile.openstreetmap.org/{}/{}/{}.png", zoom, x, y);
    auto doc = docMgr->GetDocument(url);
    if (doc && (doc->Status() == Document::DocStatus::OK)) {
        // work out a scaling factor to get a 256x256 tile from whatever MuPDF thinks is the doc size
        auto ps = doc->PageSize();
        float sx = 256.0f / ps.first;
        float sy = 256.0f / ps.second;
        auto tile = doc->GetTile(0, sx, sy, 0, 0, 256, 256); // TODO - obviously this needs to use the mapconfig stuff from Avitab
        CachedTile ct(tile);
        tileCache[std::make_pair(x, y)] = ct;
        return tile;
    }

    // finally if no tile was available then return the chessboard
    return missingTile;
}

void MapTileProvider::SetZoom(unsigned z)
{
    // TODO - obviously this needs to use the mapconfig stuff from Avitab
    if (z < 15) { // since z is unsigned, this catches overflow and underflow
        if (zoom != z) {
            zoom = z;
            tileCache.clear();  // cache is only for one zoom level at a time
        }
    }
}

unsigned MapTileProvider::GetZoom()
{
    return zoom;
}

void MapTileProvider::LatLon2TileXY(Location loc, double &x, double &y)
{
    // TODO - around the polar regions we will change to a basic overhead
    // projection showing only a simple latitude/longitude lattice.
    double n = 1 << zoom;
    x = n * ((loc.longitude + 180.0) / 360.0);
    double latr = deg2rad(loc.latitude);
    y = n * (1 - (log(tan(latr) + sec(latr)) / M_PI)) / 2;
}

void MapTileProvider::TileXY2LatLon(double x, double y, Location &loc)
{
    UNIMPLEMENTED(__func__);
}

double MapTileProvider::GetTileCentreWidthDegrees()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

double MapTileProvider::GetTileCentreWidthMetres()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

double MapTileProvider::GetTileHeightDegrees()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

double MapTileProvider::GetTileHeightMetres()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

}
