/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>
#include "navitab/logger.h"

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

    /// @brief Configure the log manager.
    /// @param useStdio True if a console is available for log message output.
    /// @param logFile The filesystem path for the log file.
    /// @param prefs The json object for logging from the preferences file.
    void Configure(bool useStdio, std::filesystem::path logFile, bool append, const nlohmann::json& prefs);

    /// @brief Log some text reporting the state of play.
    /// @param filterId Identifies the filter to be applied, may be modified for next use.
    /// @param name Name of the logger
    /// @param file The source file for the code generating the message.
    /// @param line The line number in the source file where the message is logged.
    /// @param s The severity of the message.
    /// @param msg The message string to be logged.
    /// @return Filter identifier to be used in subsequent Log() method calls.
    int Log(int filterId, const char *name, const char *file, const int line, Logger::Severity s, const std::string msg);

    enum { UnknownFilterId = -1 };
    ~LogManager();

private:
    LogManager();

private:
    // has the log been configured, messages are cached until this happens
    bool isConfigured;

    // are the normal stdio streams available for use?
    bool hasStdio;

    // the log file
    std::unique_ptr<std::ofstream> logFile;

    // number of bytes to be removed from the __FILE__ macros before logging
    int srcFilePrefixLength;

    // bit masks used to enable logging destinations
    enum Dest { FILE = 0b100, STDERR = 0b10, STDOUT = 0b1 };

    // filter definitions and collection
    struct Filter
    {
        int const id;               // is also the index in the filters vector
        std::string const pattern;  // the pattern string from the configuration
        std::vector<int> dests;     // each entry refers to a severity category
        Filter(int id, const std::string pattern, bool console);
        void Configure(int severity, std::string config);
    };
    std::vector<Filter> filters;

    // message cache definition and list
    struct Cache
    {
        const char* name;
        const char* file;
        const int line;
        Logger::Severity severity;
        const std::string message;
        Cache(const char* name, const char* file, const int line, Logger::Severity s, const std::string msg);
    };
    std::list<Cache> cache;
};


} // namespace logging
