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

#include "maps.h"
#include <cmath>

double sec(double a) { return 1.0 / cos(a); }
double deg2rad(double d) { return d * M_PI / 180.0; }

namespace navitab {

MapsProvider::MapsProvider()
:   LOG(std::make_unique<logging::Logger>("maps")),
    zoom(5)
{
}

std::shared_ptr<RasterTile> MapsProvider::GetTile(int x, int y)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

std::shared_ptr<RasterTile> MapsProvider::GetTile(unsigned page, int x, int y)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}

void MapsProvider::SetZoom(unsigned z)
{
    if (z < 12) {
        zoom = z;
    }
}

unsigned MapsProvider::GetZoom()
{
    return zoom;
}

void MapsProvider::LatLon2TileXY(Location loc, double &x, double &y)
{
    double n = 1 << zoom;
    x = n * ((loc.longitude + 180.0) / 360.0);
    double latr = deg2rad(loc.latitude);
    y = n * (1 - (log(tan(latr) + sec(latr)) / M_PI)) / 2;
}

void MapsProvider::TileXY2LatLon(double x, double y, Location &loc)
{
    UNIMPLEMENTED(__func__);
}

double MapsProvider::GetTileCentreWidthDegrees()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

double MapsProvider::GetTileCentreWidthMetres()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

double MapsProvider::GetTileHeightDegrees()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

double MapsProvider::GetTileHeightMetres()
{
    UNIMPLEMENTED(__func__);
    return 0.0;
}

}
