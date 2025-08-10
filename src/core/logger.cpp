/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "navitab/logger.h"
#include "logmanager.h"

namespace logging {

Logger::Logger(const char *n)
:   lmgr(LogManager::GetLogManager()),
    name(n),
    fid(LogManager::UnknownFilterId)
{
}

void Logger::Log(const char *file, const int line, Severity s, const std::string msg)
{
    fid = lmgr->Log(fid, name, file, line, s, msg);
}


} // namespace logging
