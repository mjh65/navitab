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

#include "navitab/tiles.h"
#include "navitab/logger.h"

// This header file defines the interface for the maps provider which
// manages local images and downloaded slippy tiles.

namespace navitab {

class MapsProvider : public TileProvider
{
public:
    MapsProvider();

    std::shared_ptr<RasterTile> GetTile(int x, int y) override;
    std::shared_ptr<RasterTile> GetTile(unsigned page, int x, int y) override;

    virtual ~MapsProvider() = default;

private:
    std::unique_ptr<logging::Logger> LOG;

};

} // namespace navitab
