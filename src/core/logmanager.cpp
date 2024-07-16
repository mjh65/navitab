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

#include "logmanager.h"
#include <fmt/core.h>
#include <iostream>

namespace navitab {
namespace logging {

std::shared_ptr<LogManager> LogManager::GetLogManager()
{
    // The LogManager uses a singleton pattern, and is created on first use
    static std::shared_ptr<LogManager> me(new LogManager());
    return me;
}

void LogManager::Configure()
{
    // update the current filters based on the json object provided,
    // and store the filters to apply to any new loggers
}

int LogManager::GetFilterId(const char *name)
{
    // there probably won't be all that many filters, so initially just
    // a linear search
    for (auto& f: filters) {
        if (f.name == name) return f.id;
    }
    Filter f(name, (int)filters.size());
    //f.Configure(); // TODO - once we have some filtering rules implemented
    filters.push_back(f);
    return f.id;
}

LogManager::Filter::Filter(const std::string n, int i)
:   name(n), id(i)
{
    // the logger severity codes are used to index the lists of log destinations
    logs.resize(1 + Logger::Severity::D);
    // by default fatal, errors and warnings get sent to stderr,
    // status goes to stdout, info and detail are discarded
    logs[Logger::Severity::F].push_back(&std::cerr);
    logs[Logger::Severity::E].push_back(&std::cerr);
    logs[Logger::Severity::S].push_back(&std::cout);
    logs[Logger::Severity::W].push_back(&std::cerr);
}

void LogManager::Filter::Configure()
{
    // TODO - (re)set up this filter based on the json configuration
}

LogManager::LogManager()
{
}

LogManager::~LogManager()
{
}

void LogManager::Log(int filterId, const char *file, const int line, Logger::Severity s, const std::string msg)
{
    // verify the filter id
    if ((filterId < 0) || (filterId >= filters.size())) return;
    auto& f = filters[filterId];
    auto& logs = f.logs[s];
    const char *sch = "FESWID";
    auto logline = fmt::format("{},{},{},{},{}", f.name, sch[s], file, line, msg);
    for (auto& l: logs) {
        if (l) (*l) << logline << std::endl;
    }
}



} // namespace logging
} // namesapce navitab
