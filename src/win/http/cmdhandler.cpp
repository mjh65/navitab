/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <chrono>
#include <iostream>
#include <string>
#include "cmdhandler.h"
#include "winhttp.h"

namespace navitab {

CommandHandler::CommandHandler(WindowHTTP *o)
:   LOG(std::make_unique<logging::Logger>("cmds")),
    owner(o)
{
    // Start the background command runner thread. This will run until it
    // gets a command to finish (exit/quit) or encounters an end-of-file.

    worker = std::make_unique<std::thread>([this]() { CommandRunner(); });
}

CommandHandler::~CommandHandler()
{
    worker->join();
}

void CommandHandler::CommandRunner()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "+ NAVITAB COMMAND INTERFACE - Use 'exit' or 'quit' to close this server +" << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(10s);
    std::string line;
    while (1) {
        std::getline(std::cin, line);
        if (!std::cin.good()) break;
        // TODO - split the line into words for full command processing
        if (line.find("quit") != std::string::npos) break;
        if (line.find("exit") != std::string::npos) break;
    }

    owner->Finish();
}



} // namespace navitab
