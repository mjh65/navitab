/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <fmt/core.h>
#include "mapapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"
#include "navitab/simulator.h"
#include "navitab/tiles.h"
#include <cmath>
#include <memory>

namespace navitab {

MapApp::MapApp(std::shared_ptr<AppServices> core)
:   App("mapapp", core),
    mapServer(nullptr),
    centredOnPlane(true),
    mapCentre(0,0)
{
    activeToolsMask = 
        (1 << ClickableTool::MENU) |
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::CENTRE) |
        (1 << ClickableTool::MAGNIFY);
    repeatingToolsMask = 
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::MAGNIFY);

    mapServer = core->GetMapsProvider();

#if 0
    // some development code for sanity testing
    for (unsigned zoom = 1; zoom < 10; ++zoom) {
        mapServer->SetZoom(zoom);
        auto maxYX = mapServer->GetTileMaxYX();
        for (int lat = -90; lat <= 90; ++lat) {
            Location l0(lat, 2 * lat, Location::DEGREES);
            auto cyx = mapServer->Location2TileYX(l0);
            double ty = cyx.first;
            double tx = cyx.second;
            if (std::isinf(ty) || (ty < 0) || (ty >= maxYX.first)) continue;
            Location l1 = mapServer->TileYX2Location(ty, tx);
            double ad = l0.angDistanceTo(l1);
            assert(ad < 0.000000001);
        }
    }
#endif

}

void MapApp::Assemble()
{
    // The MapApp mostly does canvas style drawing, but we don't use an
    // LVGL canvas since that just adds an extra layer of buffer and pixel copying.
    // There is an options menu that can be popped-up. This does use LVGL, and so
    // we probably will construct this here but make it invisible until needed.
    // TODO - build the options dialog - see above
    UNIMPLEMENTED(__func__);
}

void MapApp::Demolish()
{
    UNIMPLEMENTED(__func__);
}

void MapApp::FlightLoop(const SimStateData& data)
{
    // A note about coordinates. Geographical/geometrical systems try to
    // follow the convention y/latitude followed by x/longitude.
    // Screen coordinates are normally x/width followed by y/height.
    // The MapApp deals with both systems, so ordering needs careful attention.

    auto canvas = core->GetCanvasPixels();
    auto canvasCentreX = canvas.Width() / 2;
    auto canvasCentreY = canvas.Height() / 2;

    // On each flight loop we need to redraw the displayed map and any enabled overlays.
    if (centredOnPlane) {
        // We actually centre the map on a point somewhere in front
        // of the plane, so work out where that might be.
        auto tileNumPixels = mapServer->GetTileDimensions();
        // Work out where the aircraft track would cross an ellipse whose
        // major and minor axes are some fraction of the window dimensions
        double insetScale = 0.8; // TODO - could make this a user setting
        double ellipseX = insetScale * std::sin(data.myPlane.hdg_rad) * canvasCentreX / tileNumPixels.second;
        double ellipseY = insetScale * std::cos(data.myPlane.hdg_rad) * canvasCentreY / tileNumPixels.first;
        auto cyx = mapServer->Location2TileYX(data.myPlane);
        mapCentre = mapServer->TileYX2Location(cyx.first + ellipseY, cyx.second + ellipseX);
    }

    // Identify which tile contains the centre point of the map
    auto cyx = mapServer->Location2TileYX(mapCentre);
    auto& cty = cyx.first; // TODO - this value can exceed the tile server ranges near the poles, including infinity at the pole. needs handling.
    auto& ctx = cyx.second;
    LOGD(fmt::format("latlon ({},{}) -> tile ({},{})", mapCentre.latDegrees(), mapCentre.lonDegrees(), cty, ctx));

    // Get the indexed tile from the map server, and then figure out where it should be drawn
    auto tile = mapServer->GetTile(cty, ctx);
    auto tileW = tile->Width();
    auto tileH = tile->Height();

    // Figure out the number of tiles in each direction from the centre
    int xRange = (canvasCentreX + tileW - 1) / tileW;
    int yRange = (canvasCentreY + tileH - 1) / tileH;

    int tilesOriginX = canvasCentreX - (int)std::floor((ctx - std::floor(ctx)) * tileW);
    int tilesOriginY = canvasCentreY - (int)std::floor((cty - std::floor(cty)) * tileH);

    ImageRegion canvasArea(0, 0, canvas.Width(), canvas.Height());

    for (int iy = 0 - yRange; iy <= 0 + yRange; ++iy) {
        for (int ix = 0 - xRange; ix <= 0 + xRange; ++ix) {
            auto tile = mapServer->GetTile(cty + iy, ctx + ix);
            int tilePosL = tilesOriginX + (ix * tileW);
            int tilePosT = tilesOriginY + (iy * tileH);
            // This is where the tile maps onto the canvas - it might extend beyond the edges
            ImageRegion canvasTargetArea(tilePosL, tilePosT, tilePosL + tileW, tilePosT + tileH);
            // This is the region of the canvas that will be painted - clipped to the canvas edges
            ImageRegion canvasPaintArea(canvasArea, canvasTargetArea);
            if (!canvasPaintArea.Empty()) {
                auto leftD = canvasPaintArea.left - canvasTargetArea.left;
                auto topD = canvasPaintArea.top - canvasTargetArea.top;
                auto rightD = canvasTargetArea.right - canvasPaintArea.right;
                auto bottomD = canvasTargetArea.bottom - canvasPaintArea.bottom;
                ImageRegion tileSourceArea(leftD, topD, tileW - rightD, tileH - bottomD);
                canvas.PaintArea(canvasPaintArea, *(std::static_pointer_cast<PixelBuffer>(tile)), tileSourceArea);
                LOGD(fmt::format("tile ({},{}) painted at ({},{}->{},{})",
                    (int)std::floor(ctx), (int)std::floor(cty), canvasPaintArea.left, canvasPaintArea.top, canvasPaintArea.right, canvasPaintArea.bottom));
            }

            // TODO - repeat the painting operation but with the navAid overlay tile


        }
    }

    // Now paint all the movable stuff - starting with the aeroplane!
    // TODO

}

void MapApp::ToolClick(ClickableTool t)
{
    switch (t) {
    case ClickableTool::REDUCE:
        mapServer->SetZoom(mapServer->GetZoom() - 1);
        break;
    case ClickableTool::MAGNIFY:
        mapServer->SetZoom(mapServer->GetZoom() + 1);
        break;
    case ClickableTool::CENTRE:
        centredOnPlane = !centredOnPlane;
        break;
    default:
        UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
        break;
    }
}

} // namespace navitab
