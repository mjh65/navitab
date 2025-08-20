/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include "navitab/geometrics.h"
#include "../app.h"

namespace navitab {

class AppServices;
class BackingStore;
class MapTileProvider;

class MapApp : public App
{
public:
    MapApp(std::shared_ptr<AppServices> core);

    void FlightLoop(const SimStateData& data) override;
    void ToolClick(ClickableTool t) override;
    void MouseEvent(int x, int y, bool l) override;

protected:
    void Assemble() override;
    void Demolish() override;

private:
    unsigned HeadingToSteppedDegrees(double hrad);
    std::shared_ptr<ImageBuffer> GeneratePlaneIcon(unsigned rotation, bool myPlane = true, bool above = true);

private:
    std::shared_ptr<BackingStore> store;
    std::shared_ptr<MapTileProvider> mapServer;
    // true if map is moving to follow plane
    bool followPlane;
    // tile dimensions (TODO - may change if the tile server is changed)
    std::pair<unsigned, unsigned> tileSize;
    // tile coordinates of the canvas centre
    std::pair<double, double> centreTYX;

    // mouse click/drag state
    struct {
        bool down;
        int dragDistance;
        int startX;
        int startY;
        bool startFollow;
        std::pair<double, double> startTYX;
    } mouseDrag;

};


} // namespace navitab
