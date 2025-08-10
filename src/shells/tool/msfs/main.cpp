/* This file is part of the Navitab project. See the README and LICENSE for details. */

 // This file provides the main function for the console variant of Navitab.
 // It does the required generic initialisation of the Navitab components,
 // and if all is successful emulates an event loop that either runs a script
 // or just does random stuff.

#if defined(_WIN32)
#include <WinSock2.h>
#endif
#include <memory>
#include <iostream>
#include <fmt/core.h>
#include "navitab/core.h"
#include "navitab/simulator.h"
#include "navitab/window.h"
#include "navitab/logger.h"


int main(int arg, char** argv)
{
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<navitab::CoreServices> nvt;
    std::shared_ptr<navitab::Simulator> sim;
    std::shared_ptr<navitab::Window> win;
    try {
        // try to initialise logging and preferences - raises exception if fails
        LOG = std::make_unique<logging::Logger>("main");
        nvt = navitab::CoreServices::MakeNavitab(navitab::SimEngine::MSFS, navitab::WinServer::HTTP);
    }
    catch (navitab::StartupError& e) {
        std::cerr << "Navitab startup exception: " << e.What() << std::endl;
        exit(1);
    }
    catch (std::exception& e) {
        std::cerr << "General exception during startup: " << e.what() << std::endl;
        exit(1);
    }

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    LOGS("Early init completed, starting and enabling");
    try {
#if defined(_WIN32)
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            LOGF(fmt::format("Panel server: WSAStartup failed with error {}", WSAGetLastError()));
        }
#endif
        nvt->Start();

        sim = navitab::Simulator::Factory();
        sim->Connect(nvt);
        win = navitab::Window::Factory();
        win->Connect(nvt);
        nvt->Activate();

        LOGS("Starting event loop");
        win->EventLoop();

        LOGS("Event loop finished, disabling and stopping");
        nvt->Deactivate();
        win->Disconnect();
        win.reset();
        sim->Disconnect();
        sim.reset();

        nvt->Stop();
        nvt.reset();
#if defined(_WIN32)
        WSACleanup();
#endif
    }
    catch (navitab::Exception& e) {
        LOGF(fmt::format("Navitab exception: {}", e.What()));
    }
    catch (std::exception& e) {
        LOGF(fmt::format("General exception: {}", e.what()));
    }

    return 0;
}

