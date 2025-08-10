/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "navdb.h"

namespace navitab {

NavProvider::NavProvider()
:   LOG(std::make_unique<logging::Logger>("nav"))
{
}

void NavProvider::MaintenanceTick()
{
}

}
