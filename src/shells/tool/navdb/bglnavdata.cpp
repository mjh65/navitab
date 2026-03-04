/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglnavdata.h"
#include <fmt/core.h>

namespace navbuilder {

Airport::Airport(unsigned uid)
:   _uid(uid), _lonx(0.0f), _laty(0.0f), _elevation(0.0f)
{
}

std::shared_ptr<navitab::navdata::Runway> Airport::RunwayPair(unsigned uid) const
{
    for (auto ri : _runways) {
        std::shared_ptr<Runway> r = std::dynamic_pointer_cast<Runway>(ri);
        if (r && r->_pairid == uid) {
            return ri;
        }
    }
    return nullptr;
}

Runway::Runway(unsigned uid, Airport* owner)
    : _uid(uid), _pairid(0), _owner(owner), _lonx(0.0f), _laty(0.0f), _elevation(0.0f), _heading(0.0f), _surface(Unknown)
{
}

std::shared_ptr<navitab::navdata::Runway> Runway::pair() const
{
    return _owner->RunwayPair(_uid);
}

Helipad::Helipad(unsigned uid)
:   _uid(uid), _lonx(0.0f), _laty(0.0f), _elevation(0.0f), _heading(0.0f), _surface(Unknown)
{
}

Start::Start(unsigned uid)
:   _uid(uid), _lonx(0.0f), _laty(0.0f), _elevation(0.0f), _heading(0.0f), _type(Unknown)
{
}

Com::Com(unsigned uid)
    : _uid(uid), _type(Unknown), _freq(0.0f)
{
}




} // namespace navbuilder
