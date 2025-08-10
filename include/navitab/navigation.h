/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
