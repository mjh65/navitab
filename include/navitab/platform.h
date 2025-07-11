/*
 *  Navitab - Navigation tablet for VR flight simulation
 *  Copyright (C) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (C) 2018-2024 Folke Will <folko@solhost.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
