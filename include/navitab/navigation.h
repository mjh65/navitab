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

// This header file defines structures and classes related to navigation.

namespace navitab {

enum {
    MAX_OTHER_AIRCRAFT = 32
};

struct Location
{
    float latitude;         // -90 .. 90
    float longitude;        // -180 .. 180
    double mercatorX();     // -1.0 .. 1.0
    double mercatorY();     // -1.0 .. 1.0
};

struct AircraftPosition
{
    Location loc;
    float elevation;    // metres above sea-level
    float heading;      // 0 .. 360, true heading
};


} // namespace navitab
