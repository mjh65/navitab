/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <vector>
#include "geometrics.h"

namespace navitab {
namespace navdata {

class NavItem
{
public:
    virtual ~NavItem() = default;
};

class NavItem2D : public NavItem
{
public:
    virtual double lonx() const = 0;
    virtual double laty() const = 0;
};

class NavItem3D : public NavItem2D
{
public:
    virtual float elevation() const = 0;
};

class NavItem4D : public NavItem3D
{
public:
    virtual float heading() const = 0;
};

class Com : public NavItem
{
public:
    virtual std::string type() const = 0;
    virtual std::string name() const = 0;
    virtual float freq() const = 0;
};

class Surface : public NavItem4D
{
public:
    enum Type {
        Unknown,
        Concrete, Asphalt, Tarmac, Macadam, Bituminous,
        Grass, Dirt, Clay, Gravel, Brick, Wood,
        Sand, Shale, Coral,
        Water, Snow, Ice
    };
    virtual Type surface() const = 0;
};

class Runway : public Surface
{
public:
    virtual std::string name() const = 0;
    virtual float length() const = 0;
    virtual float width() const = 0;
    virtual float threshold() const = 0; // overrun is pair's threshold
    virtual std::string ilsIcao() const = 0; // or ptr to ILS object??
    virtual std::shared_ptr<Runway> pair() const = 0;
};

class Helipad : public Surface
{
    // no additional interfaces
};

class Start : public NavItem4D
{
public:
    enum Type {
        Unknown, Runway, Helipad, Water
    };
    virtual Type type() const = 0;
    virtual std::string name() const = 0;
};

class Airport : public NavItem3D
{
public:
    virtual ~Airport() = default;

    virtual std::string icao() const = 0;
    virtual std::string name() const = 0;
    virtual std::string region() const = 0;
    virtual std::string country() const = 0;

    virtual std::vector<std::shared_ptr<Com>> coms() const = 0;
    virtual std::vector<std::shared_ptr<Runway>> runways() const = 0;
    virtual std::vector<std::shared_ptr<Helipad>> helipads() const = 0;
    virtual std::vector<std::shared_ptr<Start>> starts() const = 0;
};



} // namespace navdata
} // namespace navitab
