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
