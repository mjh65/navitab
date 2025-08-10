/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <thread>
#include "navitab/logger.h"

namespace navitab {

class WindowHTTP;

class CommandHandler
{
public:
    CommandHandler(WindowHTTP*);
    virtual ~CommandHandler();

protected:
    void CommandRunner();


private:
    std::unique_ptr<logging::Logger> LOG;
    WindowHTTP* const owner;
    std::unique_ptr<std::thread> worker;
};


} // namespace navitab
