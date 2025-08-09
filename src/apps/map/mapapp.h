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
