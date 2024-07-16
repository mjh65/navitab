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

// This header file defines the interface to the Navitab logging subsystem.
// Most of it is hidden behind preprocessor macros in order to get the source
// file and line numbers, so the macro names might lead to some issues!

namespace navitab {
namespace logging {

class LogManager;

// The class Logger provides the logging services used by the Navitab
// subsystems. Typically the macros at the end of this header are used
// to generate logging messages, however an instance of the Logger class
// is required as a parameter 

// Create a Logger instance in each place where logging is required.
// The name of the Logger object will be used to figure out what level
// of filtering should be applied. The default filtering is to ignore
// info and detail messages. Filters can be set in the json preferences
// file to enable more detailed logging for areas of interest, based on
// a hierarchical naming system for the Logger objects.

class Logger
{
public:
    enum Severity { F, E, S, W, I, D };

    /// @brief Contruct a Logger to record operational details of the program
    /// @param name The filter name for this logger, used for finer grained filtering 
    Logger(const char *name);

    /// @brief Log some text reporting the state of play
    /// @param file The source file for the code generating the message
    /// @param line The line number in the source file where the message is logged
    /// @param s The severity of the message
    /// @param msg The message string to be logged
    void Log(const char *file, const int line, Severity s, const std::string msg);

private:
    /// @brief The LogManager that does all the work
    std::shared_ptr<LogManager> const lmgr;
    /// @brief Filter identifier provided by the LogManager
    int fid;
};

} // namespace logging
} // namesapce navitab

// Logging in the Navitab subsystems should normally be done using these macros.
// This will simplify the file and line number reporting.
#define FATAL(P,M)  (P.Log(__FILE__,__LINE__,navitab::logging::Logger::Severity::F,M))
#define ERROR(P,M)  (P.Log(__FILE__,__LINE__,navitab::logging::Logger::Severity::E,M))
#define STATUS(P,M) (P.Log(__FILE__,__LINE__,navitab::logging::Logger::Severity::S,M))
#define WARN(P,M)   (P.Log(__FILE__,__LINE__,navitab::logging::Logger::Severity::W,M))
#define INFO(P,M)   (P.Log(__FILE__,__LINE__,navitab::logging::Logger::Severity::I,M))
#define DETAIL(P,M) (P.Log(__FILE__,__LINE__,navitab::logging::Logger::Severity::D,M))
