/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <cmath>

// This header file defines structures and classes related to
// locations, trajectories and spherical geometry.

namespace navitab {

// Some conversion multipliers and other useful constants
constexpr const double M_TO_FT = 3.28084;

// Locations represent a position on the globe surface
struct Location
{
    double latitude;                // -90 .. +90
    double longitude;               // -180 .. +180
    Location() = default;
    Location(double lat, double lon);
    Location& operator=(const Location&);
    double toMercatorLat() const;   // -1.0 .. 1.0
    double toMercatorLon() const;   // -1.0 .. 1.0
    double angDistanceTo(Location& l) const;
};

// A Trajectory is a location with a heading
struct Trajectory : public Location
{
    double heading;                 // 0 .. 360, true heading
    Trajectory() = default;
    Trajectory(double lat, double lon, double head);
    Trajectory(double lat, double lon, double destLat, double destLon);
    Trajectory(double lat, double lon, const Location &dest);
    Trajectory& operator=(const Trajectory&);
    Trajectory getWaypoint(double angle) const;
};

// A Position is a location, heading and altitude
struct Position : public Trajectory
{
    double altitude;                // metres above sea-level
    Position() = default;
    Position(double lat, double lon, double head, double alt);
    Position& operator=(const Position&);
    Position& operator=(const Trajectory&);
};


inline Location::Location(double lat, double lon)
:   latitude(lat), longitude(lon)
{
    if (latitude > 90.0f) latitude = 90.0f;
    if (latitude < -90.0f) latitude = -90.0f;
    while (longitude >= 180.0f) longitude -= 360.0f;
    while (longitude < -180.0f) longitude += 360.0f;
}

inline Location& Location::operator=(const Location& x)
{
    latitude = x.latitude;
    longitude = x.longitude;
    return *this;
}

inline Trajectory::Trajectory(double lat, double lon, double head)
:   Location(lat, lon), heading(head)
{
    while (heading >= 360.0f) heading -= 360.0f;
    while (heading < 0.0f) heading += 360.0f;
}

inline Trajectory& Trajectory::operator=(const Trajectory& x)
{
    latitude = x.latitude;
    longitude = x.longitude;
    heading = x.heading;
    return *this;
}

inline Position::Position(double lat, double lon, double head, double alt)
:   Trajectory(lat, lon, head), altitude(alt)
{
}

inline Position& Position::operator=(const Position& x)
{
    latitude = x.latitude;
    longitude = x.longitude;
    heading = x.heading;
    altitude = x.altitude;
    return *this;
}

inline Position& Position::operator=(const Trajectory& x)
{
    latitude = x.latitude;
    longitude = x.longitude;
    heading = x.heading;
    return *this;
}

inline double Location::toMercatorLat() const
{
    return latitude / 90.0f;
}

inline double Location::toMercatorLon() const
{
    return longitude / 180.0f;
}

inline Trajectory::Trajectory(double lat, double lon, double destLat, double destLon)
:   Trajectory(lat, lon, Location(destLat, destLon))
{
}


} // namespace navitab
