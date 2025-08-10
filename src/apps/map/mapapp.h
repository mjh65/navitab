/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include "navitab/navigation.h"
#include "../app.h"
#include "../../maps/maptileprovider.h"

namespace navitab {

class MapApp : public App
{
public:
    MapApp(std::shared_ptr<AppServices> core);

    void FlightLoop(const SimStateData& data) override;
    void ToolClick(ClickableTool t) override;

protected:
    void Assemble() override;
    void Demolish() override;

private:
    std::shared_ptr<MapTileProvider> mapServer;
    bool centredOnPlane;
    Location mapCentre;

};


} // namespace navitab
