/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <cmath>
#include <utility>

// This header file defines structures and classes related to
// locations, trajectories and spherical geometry.
// The members of these classes are stored as radians since this
// matches the expectations of the cmath library and reduces the
// number of conversions that will be required.

namespace navitab {

// Some conversion multipliers and other useful constants
constexpr const double M_TO_FT = 3.28084;
constexpr const double DEG_TO_RAD = M_PI / 180.0f;
constexpr const double RAD_TO_DEG = 180.0f / M_PI;


// Locations represent a point on the globe surface
struct Location
{
    enum AngleUnits { RADIANS, DEGREES };

    double ypos_rad;                // -PI/2 .. +PI/2
    double xpos_rad;                // -PI .. +PI

    Location() = default;
    Location(const Location&) = default;
    Location(double lat, double lon, AngleUnits units = RADIANS);
    Location& operator=(const Location&);

    double latDegrees() const { return ypos_rad * RAD_TO_DEG; }
    double lonDegrees() const { return xpos_rad * RAD_TO_DEG; }
    std::pair<double, double> toMercator() const;   // y,x
    double angDistanceTo(Location& l) const;
};

// A Trajectory is the combination of a Location with a direction of travel
struct Trajectory : public Location
{
    double hdg_rad;                 // -PI .. +PI
 
    Trajectory() = default;
    Trajectory(const Trajectory&) = default;
    Trajectory(const Location& here, double head, AngleUnits units = RADIANS);
    Trajectory(const Location& here, const Location &dest);
    Trajectory& operator=(const Trajectory&);

    Trajectory getWaypoint(double angle, AngleUnits units = RADIANS) const;
    double hdgDegrees() const;
};

// A Position is a location, heading and altitude
struct Position : public Trajectory
{
    enum AltitudeUnits { METRES, FEET };

    double alt_metres;              // metres above sea-level

    Position() = default;
    Position(const Position&) = default;
    Position(const Trajectory&, double alt, AltitudeUnits units = METRES);
    Position& operator=(const Position&);
    Position& operator=(const Trajectory&);

    double altFeet() const;
};


inline Location::Location(double lat, double lon, AngleUnits units)
:   ypos_rad(lat), xpos_rad(lon)
{
    if (units == DEGREES) {
        ypos_rad *= DEG_TO_RAD;
        xpos_rad *= DEG_TO_RAD;
    }
    if (ypos_rad > M_PI / 2) ypos_rad = M_PI / 2;
    if (ypos_rad < -(M_PI / 2)) ypos_rad = -(M_PI / 2);
    while (xpos_rad >= M_PI) xpos_rad -= 2 * M_PI;
    while (xpos_rad < -M_PI) xpos_rad += 2 * M_PI;
}

inline Trajectory::Trajectory(const Location& ref, double head, AngleUnits units)
:   Location(ref), hdg_rad(head)
{
    if (units == DEGREES) {
        hdg_rad *= DEG_TO_RAD;
    }
    while (hdg_rad >= M_PI) hdg_rad -= 2 * M_PI;
    while (hdg_rad < -M_PI) hdg_rad += 2 * M_PI;
}

inline Position::Position(const Trajectory& ref, double alt, AltitudeUnits units)
:   Trajectory(ref), alt_metres(alt)
{
    if (units == FEET) {
        alt_metres /= M_TO_FT;
    }
}

inline Location& Location::operator=(const Location& l)
{
    ypos_rad = l.ypos_rad;
    xpos_rad = l.xpos_rad;
    return *this;
}

inline Trajectory& Trajectory::operator=(const Trajectory& t)
{
    Location::operator=(t);
    hdg_rad = t.hdg_rad;
    return *this;
}

inline Position& Position::operator=(const Trajectory& t)
{
    Trajectory::operator=(t);
    return *this;
}

inline Position& Position::operator=(const Position& p)
{
    Trajectory::operator=(p);
    alt_metres = p.alt_metres;
    return *this;
}

} // namespace navitab
