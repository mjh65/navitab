/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "navitab/geometrics.h"

namespace navitab {

constexpr const double DEG_TO_RAD = M_PI / 180.0f;
constexpr const double RAD_TO_DEG = 180.0f / M_PI;

// Some of these functions use great circle geometry equations.
// See https://www.movable-type.co.uk/scripts/latlong.html for sources.

double Location::angDistanceTo(Location& targ) const
{
    double phi1 = latitude * DEG_TO_RAD;
    double phi2 = targ.latitude * DEG_TO_RAD;
    double lambda1 = longitude * DEG_TO_RAD;
    double lambda2 = targ.longitude * DEG_TO_RAD;
    double deltaPhi = phi2 - phi1;
    double deltaLambda = lambda2 - lambda1;

    double a = std::sin(deltaPhi / 2.0) * std::sin(deltaPhi / 2.0) +
        std::cos(phi1) * std::cos(phi2) *
        std::sin(deltaLambda / 2.0) * std::sin(deltaLambda / 2.0);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return c * RAD_TO_DEG; // result in angular degrees
}

Trajectory::Trajectory(double lat, double lon, const Location& targ)
    : Location(lat, lon)
{
    double phi1 = latitude * DEG_TO_RAD;
    double phi2 = targ.latitude * DEG_TO_RAD;
    double lambda1 = longitude * DEG_TO_RAD;
    double lambda2 = targ.longitude * DEG_TO_RAD;
    double deltaPhi = phi2 - phi1;
    double deltaLambda = lambda2 - lambda1;

    double Y = std::sin(deltaLambda) * std::cos(phi2);
    double X = std::cos(phi1) * std::sin(phi2) - std::sin(phi1) * std::cos(phi2) * std::cos(deltaLambda);

    double headingRad = std::atan2(Y, X);

    heading = headingRad * RAD_TO_DEG;
    while (heading >= 360.0f) heading -= 360.0f;
    while (heading < 0.0f) heading += 360.0f;
}

Trajectory Trajectory::getWaypoint(double angle) const
{
    if (fmod(angle, 360.0) == 0.0) return *this;

    double phi1 = latitude * DEG_TO_RAD;
    double lambda1 = longitude * DEG_TO_RAD;
    double headRad = heading * DEG_TO_RAD;
    double angRad = angle * DEG_TO_RAD;

    double phi2 = std::asin(std::sin(phi1) * std::cos(angRad) + std::cos(phi1) * std::sin(angRad) * std::cos(headRad));
    double lambda2 = lambda1 + std::atan2(std::sin(headRad) * std::sin(angRad) * std::cos(phi1), std::cos(angRad) - std::sin(phi1) * std::sin(phi2));

    // construct a trajectory, initially pointing back to this location ...
    Trajectory result(phi2 * RAD_TO_DEG, lambda2 * RAD_TO_DEG, *this);
    // ... and then invert the direction
    result.heading += 180.0f;
    while (result.heading >= 360.0f) result.heading -= 360.0f;
    return result;
}



}