/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <ctime>
#include "navitab/platform.h"

namespace navitab {

std::string LocalTime(const char *format)
{
    time_t now = time(nullptr);
    tm* local = localtime(&now);

    char buf[32];
    strftime(buf, sizeof(buf), format, local);
    return buf;
}



} // namespace navitab
