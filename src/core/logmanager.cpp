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
#include <cassert>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <map>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <nlohmann/json.hpp>
#include "navitab/core.h"

namespace navitab {
namespace logging {

std::shared_ptr<LogManager> LogManager::GetLogManager()
{
    // The LogManager uses a singleton pattern, and is created on first use
    static std::shared_ptr<LogManager> me(new LogManager());
    return me;
}

LogManager::LogManager()
:   isConfigured(false),
    srcFilePrefixLength(0),
    hasStdio(false)
{
    // work out how much of the __FILE__ macro should be removed
    // from the log to leave only the relevant relative path
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
    auto now = std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now());
    std::string msg(fmt::format("Navitab logging ended at {:%Y-%m-%d %H:%M:%S}", now));
    if (logFile) {
        (*logFile) << msg << std::endl;
    } else {
        std::cout << msg << std::endl;
    }
}

void LogManager::Configure(bool useStdio, std::filesystem::path path, bool append, const nlohmann::json& prefs)
{
    assert(!isConfigured);

    hasStdio = useStdio;

    // Open the log file, in append mode if indicated
    std::ios_base::openmode mode = {};
    if (append) mode |= std::ios::app;
    std::unique_ptr<std::ofstream> lf(new std::ofstream(path, mode));
    if (lf->good()) {
        std::swap(logFile, lf);
    }

    // Opening message
    auto now = std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now());
    std::string msg(fmt::format("Navitab logging started at {:%Y-%m-%d %H:%M:%S}", now));
    if (logFile) {
        (*logFile) << msg << std::endl;
    } else {
        std::cout << msg << std::endl;
    }

    // Configure the filters using the json object provided
    // Iterate through the json object and extract the filter configurations into a map, keyed by the pattern
    // Do most of this in a try/catch block in case of incorrectly structured json data
    std::map<std::string, std::vector<std::string>> jsfilters;
    try {
        const nlohmann::json& fa(prefs.at("/filters"_json_pointer)); // fa is the array of configurators
        for (auto& fci : fa.items()) {
            try {
                auto fc = fci.value(); // fc is a filter configuration
                nlohmann::json pattern(fc.at("/pattern"_json_pointer));   // pattern is a required key
                std::vector<std::string> dests;
                for (auto s : { "/FATAL", "/ERROR", "/STATUS", "/WARN", "/INFO", "/DETAIL" }) {
                    nlohmann::json::json_pointer k(s);
                    dests.push_back(fc.contains(k) ? fc.at(k) : "");    // severity keys are optional
                }
                jsfilters[pattern] = dests;
            }
            catch (...) {}
        }
    }
    catch (...) {}

    // the json data has been extracted into a map
    // separate out the default pattern "*" first
    Filter fDef((int)filters.size(), "*", hasStdio);
    auto dfi = jsfilters.find("*");
    if (dfi != jsfilters.end()) {
        for (int s = 0; s <= Logger::Severity::D; ++s) {
            fDef.Configure(s, (*dfi).second[s]);
        }
        jsfilters.erase(dfi);
    }
    filters.push_back(fDef);

    // now create a new Filter for each additional configuration supplied
    for (auto fi : jsfilters) {
        Filter f((int)filters.size(), fi.first, hasStdio);
        f.dests = fDef.dests; // filter severity destinations start as copy of default
        for (int s = 0; s <= Logger::Severity::D; ++s) {
            f.Configure(s, fi.second[s]);
        }
        filters.push_back(f);
    }

    // mark configuration as complete so that future logging gets handled immediately
    isConfigured = true;

    // now process any messages that were reported before
    // configuration had been completed
    for (auto& cm : cache) {
        (void)Log(UnknownFilterId, cm.name, cm.file, cm.line, cm.severity, cm.message);
    }
    cache.clear();
}

int LogManager::Log(int filterId, const char* name, const char* file, const int line, Logger::Severity s, const std::string msg)
{
    // If the log manager is not yet configured then this message is
    // added to the cache and will be processed later.
    if (!isConfigured) {
        Cache cm(name, file, line, s, msg);
        cache.push_back(cm);
        return UnknownFilterId;
    }

    // If the filter ID is < 0 then we need to find the filter whose
    // pattern is the longest one that matches the logger's name
    if (filterId < 0) {
        std::pair<int, int> candidate({ 0,0 });
        for (auto& f : filters) {
            if (std::string(name).find(f.pattern) == 0) {
                // this filter's pattern matches. use it if it is longer than the current option
                if (f.pattern.size() > candidate.second) {
                    candidate = { f.id, (int)f.pattern.size() };
                }
            }
        }
        filterId = candidate.first;
    }

    // Now format the arguments into a line for the log, and then send it to the configured destinations
    auto& f = filters[filterId];
    int dests = f.dests[s];
    if (!dests) return filterId;    // short-circuit if this message is being discarded

    auto now = std::chrono::round<std::chrono::seconds>(std::chrono::system_clock::now());
    const char* sch = "FESWID";
    const char* src = file + srcFilePrefixLength;
    auto logline = fmt::format("{:%H:%M:%S}|{}|{:<12}|{:<32}|{:>3}|{}", now, sch[s], name, src, line, msg);
    if (dests & Dest::STDERR) std::cerr << logline << std::endl;
    if (dests & Dest::STDOUT) std::cout << logline << std::endl;
    if (logFile && (dests & Dest::FILE)) (*logFile) << logline << std::endl;
    if (s == Logger::Severity::F) throw navitab::LogFatal(msg);
    return filterId;
}

LogManager::Filter::Filter(int i, const std::string p, bool console)
:   id(i), pattern(p)
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

void LogManager::Filter::Configure(int severity, std::string config)
{
    if (config.empty()) return; // empty strings have no effect, default remains
    dests[severity] = 0;
    if (config.find('N') != std::string::npos) return;
    if (config.find('F') != std::string::npos) dests[severity] |= Dest::FILE;
    if (config.find('1') != std::string::npos) dests[severity] |= Dest::STDOUT;
    if (config.find('2') != std::string::npos) dests[severity] |= Dest::STDERR;
}

LogManager::Cache::Cache(const char* n, const char* f, const int l, Logger::Severity s, const std::string m)
:   name(n),
    file(f),
    line(l),
    severity(s),
    message(m)
{
}


} // namespace logging
} // namesapce navitab
