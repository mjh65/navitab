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

#if defined(LOG) || defined(LOGF) || defined(LOGE) || defined(LOGS) || defined(LOGW) || defined(LOGI) || defined(LOGD) || defined(UNIMPLEMENTED)
#error "Hmmm, Navitab's logging macros could cause problems!"
#endif

// Logging in the Navitab subsystems uses the LOGx macros which simplify the logging
// interface (in particular file and line number reporting is automated).
// However, the macros expand to code that requires a symbol 'LOG' whch is used to
// to access Log(std::string).
// Normally 'LOG' will be a pointer to a Logger object which has been instantiated
// (using std::make_unique) for the class, or the function.

#define LOGF(M) (LOG->Log(__FILE__,__LINE__,logging::Logger::Severity::F,M))
#define LOGE(M) (LOG->Log(__FILE__,__LINE__,logging::Logger::Severity::E,M))
#define LOGS(M) (LOG->Log(__FILE__,__LINE__,logging::Logger::Severity::S,M))
#define LOGW(M) (LOG->Log(__FILE__,__LINE__,logging::Logger::Severity::W,M))
#define LOGI(M) (LOG->Log(__FILE__,__LINE__,logging::Logger::Severity::I,M))
#define LOGD(M) (LOG->Log(__FILE__,__LINE__,logging::Logger::Severity::D,M))

#define UNIMPLEMENTED(M) LOGE(std::string("UNIMPLEMENTED CODE: ") + M)

namespace logging {

class LogManager;

// The class Logger provides the logging services used by the Navitab
// subsystems. Normally the macros at the start of this header are used
// to generate logging messages, however an instance of the Logger class
// named LOG is required to be in scope where the macro is used.

// Create a Logger instance in each place where logging is required.
// The name of the Logger object will be used to figure out what level
// of filtering should be applied to messages reported through this logger.
// The default filtering will ignore info and detail messages. Additional
// filters can be set in the json preferences file to enable more detailed
// or sparser logging for specific subsystems, based on the names provided
// or the Logger objects when they are instantiated.

class Logger
{
public:
    enum Severity { F, E, S, W, I, D };

    /// @brief Contruct a Logger to record operational details of the program.
    /// @param name The name for this logger, used for finer grained filtering.
    Logger(const char *name);

    /// @brief Log some text reporting the state of play. Not intended for direct call, use the macros.
    /// @param file The source file for the code generating the message.
    /// @param line The line number in the source file where the message is logged.
    /// @param s The severity of the message.
    /// @param msg The message string to be logged.
    void Log(const char *file, const int line, Severity s, const std::string msg);

private:
    /// @brief The LogManager that does all the actual filtering and logging.
    std::shared_ptr<LogManager> const lmgr;

    /// @brief Name given to this logging object
    const char* const name;

    /// @brief Filter identifier provided by the LogManager.
    int fid;
};

} // namespace logging
