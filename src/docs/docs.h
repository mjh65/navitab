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
#include "../imgkit/rasterizer.h"

// This header file defines the interface for the document provider which
// manages local and downloaded documents and rendering to raster tiles.

namespace navitab {

class DocsProvider : public TileProvider
{
public:
    DocsProvider();

    std::shared_ptr<RasterTile> GetTile(int x, int y) override;
    std::shared_ptr<RasterTile> GetTile(unsigned page, int x, int y) override;

    virtual ~DocsProvider() = default;

private:
    std::unique_ptr<logging::Logger> LOG;

    // a reference just to test 3rd-party library build and link
    // eventually each document will have its own Rasterizer ...
    std::unique_ptr<Rasterizer> r;
};

} // namespace navitab
