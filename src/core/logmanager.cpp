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
#include "navitab/core.h"
#include <fmt/core.h>
#include <algorithm>
#include <iostream>

namespace navitab {
namespace logging {

std::shared_ptr<LogManager> LogManager::GetLogManager()
{
    // The LogManager uses a singleton pattern, and is created on first use
    static std::shared_ptr<LogManager> me(new LogManager());
    return me;
}

LogManager::LogManager()
:   srcFilePrefixLength(0),
    isConsole(false)
{
    std::string filePP(__FILE__);
    std::replace(filePP.begin(), filePP.end(), '\\', '/');
    std::string fileRel("src/core/logmanager.cpp");
    auto p = filePP.find(fileRel);
    if (p != std::string::npos) {
        srcFilePrefixLength = p;
    }
}

LogManager::~LogManager()
{
}

void LogManager::SetLogFile(std::filesystem::path path)
{
    // open the new log in append mode if there was one already open
    std::ios_base::openmode mode = {};
    if (logFile) mode |= std::ios::app;
    // attempt to open the new one
    std::unique_ptr<std::ofstream> lf(new std::ofstream(path, mode));
    if (lf->good()) std::swap(logFile, lf);
}

void LogManager::Configure(const nlohmann::json& prefs)
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
    Filter f(name, (int)filters.size(), isConsole);
    //f.Configure(); // TODO - once we have some filtering rules implemented
    filters.push_back(f);
    return f.id;
}

LogManager::Filter::Filter(const std::string n, int i, bool console)
:   name(n), id(i)
{
    // the logger severity codes are used to index the lists of log destinations
    dests.resize(1 + Logger::Severity::D);
    // by default fatal, errors and warnings get sent to stderr,
    // status goes to stdout, info and detail are discarded
    dests[Logger::Severity::F] = Dest::FILE | Dest::STDERR;
    dests[Logger::Severity::E] = Dest::FILE | Dest::STDERR;
    if (console) {
        dests[Logger::Severity::S] = Dest::FILE | Dest::STDOUT;
        dests[Logger::Severity::W] = Dest::FILE | Dest::STDERR;
    }
    else {
        dests[Logger::Severity::S] = Dest::FILE;
        dests[Logger::Severity::W] = Dest::FILE;
    }
    dests[Logger::Severity::I] = 0;
    dests[Logger::Severity::D] = 0;
}

void LogManager::Filter::Configure()
{
    // TODO - (re)set up this filter based on the json configuration
}

void LogManager::Log(int filterId, const char *file, const int line, Logger::Severity s, const std::string msg)
{
    // verify the filter id
    if ((filterId < 0) || (filterId >= filters.size())) return;
    auto& f = filters[filterId];
    int dests = f.dests[s];
    const char *sch = "FESWID";
    const char* src = file + srcFilePrefixLength;
    auto logline = fmt::format("{},{},{},{},{}", f.name, sch[s], src, line, msg);
    if (dests & Dest::STDERR) std::cerr << logline << std::endl;
    if (dests & Dest::STDOUT) std::cout << logline << std::endl;
    if (logFile && (dests & Dest::FILE)) (*logFile) << logline << std::endl;
    if (s == Logger::Severity::F) throw navitab::LogFatal(msg);
}



} // namespace logging
} // namesapce navitab
