/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/logger.h"

// This header file defines the interface for the Nav provider which
// manages the navigation database, including generation of the MySQL
// database from the simulator data files.

namespace navitab {

class NavProvider
{
public:
    NavProvider();

    void MaintenanceTick();

    virtual ~NavProvider() = default;

private:
    std::unique_ptr<logging::Logger> LOG;
};

} // namespace navitab
