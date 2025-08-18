/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "navitab/geometrics.h"

namespace navitab {

// Some of these functions use great circle geometry equations.
// See https://www.movable-type.co.uk/scripts/latlong.html for sources.

double Location::angDistanceTo(Location& targ) const
{
    auto& phi1 = ypos_rad;
    auto& lambda1 = xpos_rad;
    auto& phi2 = targ.ypos_rad;
    auto& lambda2 = targ.xpos_rad;
    double deltaPhi = phi2 - phi1;
    double deltaLambda = lambda2 - lambda1;

    double a = std::sin(deltaPhi / 2.0) * std::sin(deltaPhi / 2.0) +
        std::cos(phi1) * std::cos(phi2) *
        std::sin(deltaLambda / 2.0) * std::sin(deltaLambda / 2.0);
    return 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
}

Trajectory::Trajectory(const Location& here, const Location& targ)
:   Location(here)
{
    auto& phi1 = ypos_rad;
    auto& lambda1 = xpos_rad;
    auto& phi2 = targ.ypos_rad;
    auto& lambda2 = targ.xpos_rad;
    double deltaPhi = phi2 - phi1;
    double deltaLambda = lambda2 - lambda1;

    double Y = std::sin(deltaLambda) * std::cos(phi2);
    double X = std::cos(phi1) * std::sin(phi2) - std::sin(phi1) * std::cos(phi2) * std::cos(deltaLambda);

    hdg_rad = std::atan2(Y, X);
}

Trajectory Trajectory::getWaypoint(double angle, AngleUnits units) const
{
    if (units == DEGREES) {
        angle *= DEG_TO_RAD;
    }
    if (fmod(angle, 2 * M_PI) == 0.0) return *this;

    auto& phi1 = ypos_rad;
    auto& lambda1 = xpos_rad;

    double phi2 = std::asin(std::sin(phi1) * std::cos(angle) + std::cos(phi1) * std::sin(angle) * std::cos(hdg_rad));
    double lambda2 = lambda1 + std::atan2(std::sin(hdg_rad) * std::sin(angle) * std::cos(phi1), std::cos(angle) - std::sin(phi1) * std::sin(phi2));

    // construct a trajectory, initially pointing back to this location ...
    Trajectory result(Location(phi2, lambda2), *this);
    // ... and then invert the direction
    result.hdg_rad += M_PI;
    while (result.hdg_rad >= M_PI) result.hdg_rad -= 2 * M_PI;
    return result;
}

std::pair<double, double> Location::toMercator() const
{
    double y = (1 - (log(tan(ypos_rad) + (1 / cos(ypos_rad))) / M_PI)) / 2;
    double x = (xpos_rad + M_PI) / (2 * M_PI);
    return std::make_pair(y, x);
}

}