/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglnavdata.h"
#include <fmt/core.h>

namespace navbuilder {

Airport::Airport(unsigned uid)
:   _uid(uid), _lonx(0.0f), _laty(0.0f), _elevation(0.0f)
{
}

Helipad::Helipad(unsigned uid)
:   _uid(uid), _lonx(0.0f), _laty(0.0f), _elevation(0.0f), _heading(0.0f), _surface(Unknown)
{
}

Start::Start(unsigned uid)
:   _uid(uid), _lonx(0.0f), _laty(0.0f), _elevation(0.0f), _heading(0.0f), _type(Unknown)
{
}



} // namespace navbuilder
