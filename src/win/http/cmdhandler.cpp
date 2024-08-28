/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
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
