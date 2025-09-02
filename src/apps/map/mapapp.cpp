/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "mapapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"
#include "navitab/simulator.h"
#include "navitab/tiles.h"
#include "../../maps/maptileprovider.h"
#include "../../store/backingstore.h"
#include <fmt/core.h>
#include <lunasvg.h>
#include <cmath>
#include <memory>

namespace navitab {

MapApp::MapApp(std::shared_ptr<AppServices> core)
:   App("mapapp", core),
    store(core->GetStoreManager()),
    mapServer(core->GetMapsProvider()),
    followPlane(true),
    centreTYX(0,0)
{
    tileSize = mapServer->GetTileDimensions();

    mouseDrag.down = false;

    activeToolsMask = 
        (1 << ClickableTool::MENU) |
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::CENTRE) |
        (1 << ClickableTool::MAGNIFY);
    repeatingToolsMask = 
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::MAGNIFY);

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
    // On each flight loop we need to redraw the displayed map and any enabled overlays.
    // The map base is constructed from tiles which have been downloaded from
    // a slippy tile server and are cached/stored locally until no longer needed.
    // The navaid overlay is also constructed from tiles which are drawn once
    // and then cached until no longer needed, or until the overlay filters are modified.
    // The aeroplane icons are redrawn on each frame.

    // for testing, fix the plane trajectory
    //Trajectory planeTraj(Location(46.948, 7.447, Location::DEGREES), 318, Location::DEGREES);
    const Trajectory &planeTraj = data.myPlane;

    // A note about coordinates. Navitab's geographical/geometrical systems try to
    // follow the convention latitude/y followed by longitude/x.
    // But canvas/screen coordinates are normally x/width followed by y/height.
    // The MapApp deals with both systems, so ordering needs careful attention.
    // Variables with the suffix TYX are tile Y & X coordinates (pair of doubles).

    auto canvas = core->GetCanvasPixels();
    auto canvasCentreY = canvas.Height() / 2;
    auto canvasCentreX = canvas.Width() / 2;
    int tileH = tileSize.first;
    int tileW = tileSize.second;

    // The focus of the map can be the aeroplane, or some fixed point. In either case,
    // the first thing to do is to work out the tile coordinate of the canvas centre.

    auto planeTYX = mapServer->Location2TileYX(planeTraj);
    if (followPlane) {
        // When the map is following the plane then the plane is positioned on an
        // ellipse facing towards the centre of the map, since it is assumed that
        // the useful part of the map is in front of the plane.

        double insetScale = 0.65f; // TODO - make this a user setting
        // these are the tile coordinate deltas to add to position the centre point
        auto tileDeltaY = canvasCentreY * insetScale * std::cos(planeTraj.hdg_rad) / tileH;
        auto tileDeltaX = canvasCentreX * insetScale * std::sin(planeTraj.hdg_rad) / tileW;
        centreTYX.first = planeTYX.first - tileDeltaY;
        centreTYX.second = planeTYX.second + tileDeltaX;
    }

    // Identify which tile contains the centre point of the map
    // TODO - cty will exceed the tile server ranges near the poles, including infinity at the pole. needs handling.
    mapServer->NormaliseTileYX(centreTYX);
    auto& cty = centreTYX.first;
    auto& ctx = centreTYX.second;

    // Get the centre tile from the map server, and then figure out where its left-top position is within the canvas
    auto tile = mapServer->GetTile(cty, ctx);
    int mainTileOriginX = canvasCentreX - (int)std::floor((ctx - std::floor(ctx)) * tileW);
    int mainTileOriginY = canvasCentreY - (int)std::floor((cty - std::floor(cty)) * tileH);

    // Figure out the starting (left-top) most tile index
    int idx = 0;
    while (mainTileOriginX + (idx * tileW) > 0) --idx;
    int idy = 0;
    while (mainTileOriginY + (idy * tileH) > 0) --idy;

    // Iterate through the tiles which have some overlap with the canvas
    for (int iy = idy; (mainTileOriginY + (iy * tileH) < (int)canvas.Height()); ++iy) {
        for (int ix = idx; (mainTileOriginX + (ix * tileW) < (int)canvas.Width()); ++ix) {
            auto tile = mapServer->GetTile(cty + iy, ctx + ix);
            int tilePosT = mainTileOriginY + (iy * tileH);
            int tilePosL = mainTileOriginX + (ix * tileW);
            canvas.PaintRegion(tilePosL, tilePosT, *(std::static_pointer_cast<PixelBuffer>(tile)));

            // TODO - blend the NavAid overlay 'tiles'
            // Design Note: the NavAid overlay will show the currently selected NavAids, AND
            // any selected georeferenced charts that are open in the charts app. Since these
            // will not be changing on a frame-by-frame basis the Navaid overlays will be drawn
            // into 'tiles'* that can be quickly blended onto the base map and cached for subsequent
            // frames. This cache will be cleared whenever the zoom or navaid filters are modified.
        }
    }

    // Now paint all the movable stuff.
    
    // TODO - paint the copyright, bottom right
    
    // TODO - draw the scale(s), top right

    // TODO - draw the other aircraft icons
    
    // TODO - put this into a function
    // Get the plane icon from the backing store (or draw it if it's never been used before)
    // TODO - add a local cache to avoid SQL queries on each frame drawn
    auto hdg = HeadingToSteppedDegrees(planeTraj.hdg_rad);
    auto name = fmt::format("myplane{:03d}", hdg);
    auto icon = store->GetPixmap(name);
    if (!icon) {
        icon = GeneratePlaneIcon(hdg);
        store->StorePixmap(name, icon);
    }

    // Draw (blend) the plane icon in its current location
    int dpy = (int)(tileH * (planeTYX.first - cty));
    int dpx = (int)(tileW * (planeTYX.second - ctx));
    int py = canvasCentreY + dpy - (icon->Height() / 2);
    int px = canvasCentreX + dpx - (icon->Width() / 2);
    canvas.BlendRegion(px, py, *icon);
}

unsigned MapApp::HeadingToSteppedDegrees(double hrad)
{
    // returns the heading in degrees, rounded to the nearest step, ie each one covers an arc of 6deg
    int headDeg = (int)std::floor(hrad * 180 / M_PI);
    return 6 * (((headDeg + 363) % 360) / 6);
}

static const char *planeIconFormat = R"SVG(
<svg width="300" height="300" xmlns="http://www.w3.org/2000/svg">
<g transform="translate(150,150)">
<g transform="scale({})">
<g transform="rotate({})" style="fill:{};stroke:none">
<ellipse rx="15" ry="25" cx="0" cy="-100"/>
<rect width="30" height="190" x="-15" y="-100" />
<polygon points="0,-50 -110,30 -110,40 0,10 110,40 110, 30" />
<polygon points="0,50 -40,100 -40,110 0,100 40,110 40,100"/>
</g></g></g></svg>
)SVG";

std::shared_ptr<ImageBuffer> MapApp::GeneratePlaneIcon(unsigned rotation, bool myPlane, bool above)
{
    const unsigned wh = myPlane ? 30 : 24;
    auto planeIconSvg = fmt::format(planeIconFormat, 1.0f, rotation, "red");
    auto document = lunasvg::Document::loadFromData(planeIconSvg);
    assert(document);
    auto bitmap = document->renderToBitmap(wh, wh, 0);
    assert(bitmap.valid());
    bitmap.convertToRGBA();
    auto p = bitmap.data();

    auto icon = std::make_shared<ImageBuffer>(wh, wh);
    memcpy(icon->Row(0), p, wh * wh * sizeof(uint32_t));

    return icon;
}

void MapApp::ToolClick(ClickableTool t)
{
    // The centre of the map is stored in tile coordinates, which change at
    // each zoom level. Convert to lat/lon, which is consistent ...
    auto clatlon = mapServer->TileYX2Location(centreTYX);
    auto z = mapServer->GetZoom();
    switch (t) {
    case ClickableTool::REDUCE:
        mapServer->SetZoom(z - 1);
        break;
    case ClickableTool::MAGNIFY:
        mapServer->SetZoom(z + 1);
        break;
    case ClickableTool::CENTRE:
        followPlane = !followPlane;
        break;
    default:
        UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
        break;
    }
    // ...  and then convert back to tile coordinates afterwards.
    if (z != mapServer->GetZoom()) {
        centreTYX = mapServer->Location2TileYX(clatlon);
    }
}

void MapApp::MouseEvent(int x, int y, bool l)
{
    if (l && !mouseDrag.down) {
        // button pressed - start following the mouse, assume drag rather than click
        mouseDrag.down = true;
        mouseDrag.dragDistance = 0;
        mouseDrag.startX = x;
        mouseDrag.startY = y;
        mouseDrag.startFollow = followPlane;
        followPlane = false;
        mouseDrag.startTYX = centreTYX;
    } else if (!l && mouseDrag.down) {
        // button released - test for drag or click
        mouseDrag.down = false;
        if (mouseDrag.dragDistance < 10) {
            // treat this as a click, restore the pre-click state
            followPlane = mouseDrag.startFollow;
            centreTYX = mouseDrag.startTYX;
            UNIMPLEMENTED(__func__ + fmt::format("(click at {},{})", x, y));
        } else {
            // treat this as the end of a drag, basically nothing more to do
        }
    } else if (l && mouseDrag.down) {
        // figure out where the new map centre should be
        auto dx = x - mouseDrag.startX;
        auto dy = y - mouseDrag.startY;
        mouseDrag.dragDistance += std::abs(dx + dy);
        double tdx = (double)dx / tileSize.second;
        double tdy = (double)dy / tileSize.first;
        centreTYX = std::make_pair(mouseDrag.startTYX.first - tdy, mouseDrag.startTYX.second - tdx);
    }
}

} // namespace navitab
