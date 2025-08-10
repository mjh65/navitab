/* This file is part of the Navitab project. See the README and LICENSE for details. */

// This file provides the main function for the desktop variant of Navitab.
// It does the required generic initialisation of the Navitab components,
// and if all is successful starts a GL window manager for the UI.
// It's used on Linux and Mac.

#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <fmt/core.h>
#include "navitab/core.h"
#include "navitab/simulator.h"
#include "navitab/window.h"
#include "navitab/logger.h"


int main(int arg, char** argv)
{
#if defined(NAVITAB_MACOSX) && !defined(NDEBUG)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<navitab::CoreServices> nvt;
    std::shared_ptr<navitab::Simulator> sim;
    std::shared_ptr<navitab::Window> win;
    try {
        // try to initialise logging and preferences - raises exception if fails
        LOG = std::make_unique<logging::Logger>("main");
        nvt = navitab::CoreServices::MakeNavitab(navitab::SimEngine::MOCK, navitab::WinServer::DESKTOP);
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
    }
    catch (navitab::Exception& e) {
        LOGF(fmt::format("Navitab exception: {}", e.What()));
    }
    catch (std::exception& e) {
        LOGF(fmt::format("General exception: {}", e.what()));
    }

    return 0;
}
