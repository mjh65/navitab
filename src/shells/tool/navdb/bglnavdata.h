/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/navdata.h"

namespace navbuilder {

class Com : public navitab::navdata::Com
{
};

class Runway : public navitab::navdata::Runway
{
};

class Helipad : public navitab::navdata::Helipad
{
public:
    Helipad(unsigned uid);
    virtual ~Helipad() = default;

    double lonx() const override { return _lonx; }
    double laty() const override { return _laty; }
    float elevation() const override { return _elevation; }
    float heading() const override { return _heading; }
    navitab::navdata::Surface::Type surface() const override { return _surface; }

    unsigned _uid;
    double _lonx, _laty;
    float _elevation;
    float _heading;
    navitab::navdata::Surface::Type _surface;
};

class Start : public navitab::navdata::Start
{
public:
    Start(unsigned uid);
    virtual ~Start() = default;

    double lonx() const override { return _lonx; }
    double laty() const override { return _laty; }
    float elevation() const override { return _elevation; }
    float heading() const override { return _heading; }
    navitab::navdata::Start::Type type() const override { return _type; }
    std::string name() const override { return _name; }

    unsigned _uid;
    double _lonx, _laty;
    float _elevation;
    float _heading;
    navitab::navdata::Start::Type _type;
    std::string _name;
};

class Airport : public navitab::navdata::Airport
{
public:
    Airport(unsigned uid);
    virtual ~Airport() = default;

    std::string icao() const override { return _icao; }
    std::string name() const override { return _name; }
    std::string region() const override { return _region; }
    std::string country() const override { return _country; }
    double lonx() const override { return _lonx; }
    double laty() const override { return _laty; }
    float elevation() const override { return _elevation; }

    std::vector<std::shared_ptr<navitab::navdata::Com>> coms() const override { return _coms; }
    std::vector<std::shared_ptr<navitab::navdata::Runway>> runways() const override { return _runways; }
    std::vector<std::shared_ptr<navitab::navdata::Helipad>> helipads() const override { return _helipads; }
    std::vector<std::shared_ptr<navitab::navdata::Start>> starts() const override { return _starts; }

    unsigned _uid;           // db locally unique ID
    std::string _icao;       // airport ICAO
    std::string _name;       // friendly name
    std::string _region;
    std::string _country;
    double _lonx, _laty;
    float _elevation;
    std::vector<std::shared_ptr<navitab::navdata::Com>> _coms;
    std::vector<std::shared_ptr<navitab::navdata::Runway>> _runways;
    std::vector<std::shared_ptr<navitab::navdata::Helipad>> _helipads;
    std::vector<std::shared_ptr<navitab::navdata::Start>> _starts;
};


} // namespace navbuilder
