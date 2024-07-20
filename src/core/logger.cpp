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

#include "navitab/logger.h"
#include "logmanager.h"

namespace navitab {
namespace logging {

Logger::Logger(const char *name)
:   lmgr(LogManager::GetLogManager()),
    fid(lmgr->GetFilterId(name))
{
}

void Logger::Log(const char *file, const int line, Severity s, const std::string msg)
{
    lmgr->Log(fid, file, line, s, msg);
}


} // namespace logging
} // namesapce navitab
