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

#include <memory>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include "navitab/logger.h"

namespace navitab {
namespace logging {

// LogManager is a singleton class which does all of the logging work.
// Classes that use the logging system interface to the LogManager by
// creating a Logger object and using a simple macro-based API to
// insert log statements into the source code. The Logger object interacts
// with the singleton LogManager which handles log messages, filtering, and
// routing.

class LogManager
{
public:
    /// @brief Get a shared pointer to the log manager, created on first use.
    /// @return A shared pointer to the log manager
    static std::shared_ptr<LogManager> GetLogManager();

    /// @brief Indicate whether the Navitab system has a console.
    /// @param q True if a console is available for message output.
    void UseConsole(bool q) { isConsole = q; }

    /// @brief Set the path of the log file to be written.
    /// @param path The filesystem path for the log file.
    void SetLogFile(std::filesystem::path path);

    /// @brief Configure the log manager.
    /// @param prefs The json object for logging from the preferences file.
    void Configure(const nlohmann::json& prefs);

    /// @brief Get the identifier of the named filter for use in message logging
    /// @param name The name of the filter
    /// @return Filter identifier to be used in subsequent Log() method calls.
    int GetFilterId(const char *name);

    /// @brief Log some text reporting the state of play.
    /// @param filterId Identifies the filter to be applied.
    /// @param file The source file for the code generating the message.
    /// @param line The line number in the source file where the message is logged.
    /// @param s The severity of the message.
    /// @param msg The message string to be logged.
    void Log(int filterId, const char *file, const int line, Logger::Severity s, const std::string msg);

    ~LogManager();

private:
    LogManager();

private:
    std::unique_ptr<std::ofstream> logFile;

    // bit masks used to enable logging destinations
    enum Dest { FILE = 0b100, STDERR = 0b10, STDOUT = 0b1 };
    struct Filter
    {
        std::string name;
        int id;
        std::vector<int> dests;
        void Configure(/*json*/);
        Filter(const std::string n, int i, bool console);
    };
    std::vector<Filter> filters;

    int srcFilePrefixLength;

    bool isConsole;
};


} // namespace logging
} // namesapce navitab
