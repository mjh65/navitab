/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/platform.h"
#include "navitab/logger.h"

/*
 * This header file defines the interface to stuff like where files are stored, etc
*/

namespace navitab {

class Paths : public PathServices
{
public:
    Paths();

    // location of the preferences and log files, as well as any temporary file
    // and cached downloads
    std::filesystem::path DataFilesPath() override;

    // browsing start for the user's resources, eg charts, docs
    std::filesystem::path UserResourcesPath() override;

    // browsing start for any aircraft documents
    std::filesystem::path AircraftResourcesPath() override;

    // browsing start for flight plans / routes
    std::filesystem::path FlightPlansPath() override;

    // directory containing the current Navitab executable
    std::filesystem::path NavitabPath() override;

private:
    std::filesystem::path FindDataFilesPath();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::filesystem::path dataFilesPath;
    std::filesystem::path userResourcesPath;
    std::filesystem::path aircraftResourcesPath;
    std::filesystem::path flightPlansPath;
    std::filesystem::path navitabPath;

};

} // namespace navitab
