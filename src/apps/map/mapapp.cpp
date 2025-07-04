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

#include <fmt/core.h>
#include "mapapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"
#include "navitab/simulator.h"
#include <cmath>
#include <memory>

namespace navitab {

MapApp::MapApp(std::shared_ptr<AppServices> core)
:   App("mapapp", core),
    mapServer(nullptr),
    centredOnPlane(true)
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
    // On each flight loop we need to redraw the displayed map and any enabled overlays.
    if (centredOnPlane) {
        mapCentre = data.myPlane.loc;
    }
    // Identify which tile contains the centre point of the map
    double ctx, cty;
    mapServer->LatLon2TileXY(mapCentre, ctx, cty);
    int itx = (int)std::floor(ctx);
    int ity = (int)std::floor(cty);
    LOGD(fmt::format("latlon ({},{}) -> tile ({},{})", mapCentre.latitude, mapCentre.longitude, ctx, cty));

    auto canvas = core->GetCanvasPixels();
    auto canvasCentreX = canvas.Width() / 2;
    auto canvasCentreY = canvas.Height() / 2;

    // Get the indexed tile from the map server, and then figure out where it should be drawn
    auto tile = mapServer->GetTile(itx, ity);
    auto tileW = tile->Width();
    auto tileH = tile->Height();

    // Figure out the number of tiles in each direction from the centre
    int xRange = (canvasCentreX + tileW - 1) / tileW;
    int yRange = (canvasCentreY + tileH - 1) / tileH;

    int tilesOriginX = canvasCentreX - (int)std::floor((ctx - itx) * tileW);
    int tilesOriginY = canvasCentreY - (int)std::floor((cty - ity) * tileH);

    ImageRegion canvasArea(0, 0, canvas.Width(), canvas.Height());

    for (int iy = 0 - yRange; iy <= 0 + yRange; ++iy) {
        for (int ix = 0 - xRange; ix <= 0 + xRange; ++ix) {
            auto tile = mapServer->GetTile(itx + ix, ity + iy);
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
                    (int)itx, (int)ity, canvasPaintArea.left, canvasPaintArea.top, canvasPaintArea.right, canvasPaintArea.bottom));
            }
        }
    }
}

void MapApp::ToolClick(ClickableTool t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
}

} // namespace navitab
