/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "maptileprovider.h"
#include "tileprovidercfg.h"
#include "navitab/core.h"
#include "navitab/platform.h"
#include "navitab/tiles.h"
#include "../docs/docmanager.h"
#include "../docs/document.h"
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <cmath>


namespace navitab {

MapTileProvider::MapTileProvider(std::shared_ptr<CoreServices> cs, std::shared_ptr<DocumentManager> d)
:   LOG(std::make_unique<logging::Logger>("maps")),
    docMgr(d),
    missingTile(nullptr),
    zoom(8)
{
    auto ps = cs->GetPathService();
    std::filesystem::path cfg = ps->DataFilesPath();
    cfg /= "tileserverconfig.json";
    providerCfg = std::make_unique<TileProviderConfigLoader>(cfg);

    prefs = cs->GetSettingsManager();
    auto mtp = prefs->Get("/maps");
    std::string preferred;
    try {
        preferred = mtp.at("/tileprovider"_json_pointer);
        smapConfig = providerCfg->GetConfig(preferred);
    }
    catch (...) {
        preferred = providerCfg->GetNames().front();
        mtp["tileprovider"] = preferred;
        prefs->Put("/maps", mtp);
    }
    
    missingTile = std::make_shared<RasterTile>();
    for (unsigned r = 0; r < missingTile->Height(); ++r) {
        uint32_t *rs = missingTile->Row(r);
        for (unsigned c = 0; c < missingTile->Width(); ++c) {
            auto dark = ((r / 32) ^ (c / 32)) & 0x1;
            *(rs + c) = dark ? 0xff404040 : 0xffc0c0c0;
        }
    }
}

MapTileProvider::~MapTileProvider()
{
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

std::shared_ptr<RasterTile> MapTileProvider::GetTile(double ty, double tx)
{
    int y = (int)std::floor(ty);
    int x = (int)std::floor(tx);

    // the x and y indices have not been constrained to the tiling limits.
    // adjust the x index in case of overflow around the date line
    // (we let the y value overflow, it doesn't wrap)
    const int xn = 1 << zoom;
    while (x < 0) x += xn;
    while (x >= xn) x -= xn;

    // do we have the requested tile in the cache?
    auto tci = tileCache.find(std::make_pair(y, x));
    if (tci != tileCache.end()) {
        ++(tci->second.useCount);
        return tci->second.tile;
    }

    // TODO - might want to iterate to lower zoom levels and then draw more
    // blurred map until the detailed one appears. the idea here would be to
    // enhance the cache indexing from a simple x,y to x,y,z where z ranges from 0 (natural zoom) to (eg) 4.

    // tile is not in the cache. request it from the Document Manager.
    std::string url = smapConfig->FormatUrl(zoom, y, x);
    auto doc = docMgr->GetDocument(url);
    if (doc && (doc->Status() == Document::DocStatus::OK)) {
        unsigned &twpx = smapConfig->tileWidthPx;
        unsigned &thpx = smapConfig->tileHeightPx;
        // work out a scaling factor to get a 256x256 tile from whatever MuPDF thinks is the doc size
        auto ps = doc->PageSize();
        float sx = (float)twpx / ps.first;
        float sy = (float)thpx / ps.second;
        auto tile = doc->GetTile(0, sx, sy, 0, 0, twpx, thpx);
        CachedTile ct(tile);
        tileCache[std::make_pair(y, x)] = ct;
        return tile;
    }

    // finally if no tile was available then return the chessboard
    return missingTile;
}

std::pair<unsigned, unsigned> MapTileProvider::GetTileDimensions() const
{
    return std::make_pair(smapConfig->tileHeightPx, smapConfig->tileWidthPx);
}

std::pair<double, double> MapTileProvider::GetTileMaxYX() const
{
    return std::make_pair(1 << zoom, 1 << zoom);
}

void MapTileProvider::SetZoom(unsigned z)
{
    if ((z >= smapConfig->minZoomLevel) && (z <= smapConfig->maxZoomLevel)) {
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

std::pair<double, double> MapTileProvider::Location2TileYX(const Location& loc)
{
    auto unscaled = loc.toMercator();
    double n = 1 << zoom;
    return std::make_pair(n * unscaled.first, n * unscaled.second);
}

Location MapTileProvider::TileYX2Location(double y, double x)
{
    double n = 1 << zoom;
    double lat = std::atan(std::sinh(M_PI * (1 - 2 * y / n)));
    double lon = ((x / n) - 0.5f) * 2 * M_PI;
    return Location(lat, lon);
}

std::pair<double, double> MapTileProvider::GetTileSpanRadians(double y)
{
    auto topEdge = TileYX2Location(std::floor(y), 0);
    auto bottomEdge = TileYX2Location(std::floor(y) - 1.0, 0);
    auto spanY = topEdge.ypos_rad - bottomEdge.ypos_rad;
    auto spanX = 2 * M_PI / (1 << zoom);
    return std::make_pair(spanY, spanX);
}

#if 0
double MapTileProvider::GetTileHeightRadians(double y)
{
    auto topEdge = TileYX2Location(std::floor(y), 0);
    auto bottomEdge = TileYX2Location(std::floor(y) - 1.0, 0);
    return topEdge.ypos_rad - bottomEdge.ypos_rad;
}

double MapTileProvider::GetTileWidthRadians()
{
    auto n = 1 << zoom;
    return 2 * M_PI / n;
}
#endif

}
