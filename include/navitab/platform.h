/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <filesystem>
#include <string>

/*
 * This header file defines the interface to the platform, which will
 * probably be stuff like the filesystem etc.
*/

namespace navitab {

struct PathServices
{
    // location of the preferences and log files, as well as any temporary file
    // and cached downloads
    virtual std::filesystem::path DataFilesPath() = 0;

    // browsing start for the user's resources, eg charts, docs
    virtual std::filesystem::path UserResourcesPath() = 0;

    // browsing start for any aircraft documents
    virtual std::filesystem::path AircraftResourcesPath() = 0;

    // browsing start for flight plans / routes
    virtual std::filesystem::path FlightPlansPath() = 0;

    // directory containing the current Navitab executable
    virtual std::filesystem::path NavitabPath() = 0;

};

std::string LocalTime(const char *format);

} // namespace navitab
