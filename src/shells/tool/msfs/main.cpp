/* This file is part of the Navitab project. See the README and LICENSE for details. */

// This file provides the main function for the MSFS panel server variant
// of Navitab. It does the required generic initialisation of the Navitab
// components and then runs an event loop that interacts with simulator
// and with the in-game panel.

#if !defined(_WIN32)
#error This source file is not portable; WIN32 only.
#endif
#include <WinSock2.h>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <fmt/core.h>
#include "navitab/core.h"
#include "navitab/simulator.h"
#include "navitab/window.h"
#include "navitab/logger.h"

static bool mainactive = true;
std::shared_ptr<navitab::Window>* winref = nullptr;
static void set_signal_handler();
static BOOL sig_handler(DWORD signum);

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
    winref = &win;
    set_signal_handler();

    // if we get this far then we should have logging enabled, so any further issues
    // can be reported through the logging interface.
    LOGS("Early init completed, starting and enabling");
    try {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            LOGF(fmt::format("Panel server: WSAStartup failed with error {}", WSAGetLastError()));
        }

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
        WSACleanup();
    }
    catch (navitab::Exception& e) {
        LOGF(fmt::format("Navitab exception: {}", e.What()));
    }
    catch (std::exception& e) {
        LOGF(fmt::format("General exception: {}", e.what()));
    }

    mainactive = false;
    return 0;
}

static void set_signal_handler()
{
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)sig_handler, TRUE);
}

static BOOL sig_handler(DWORD signum)
{
    if ((signum == CTRL_BREAK_EVENT) || (signum == CTRL_CLOSE_EVENT)) {
        if (*winref) {
            (*winref)->SigStop();
        }
        while (mainactive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return TRUE;
    }
    if (signum == CTRL_C_EVENT) {
        // it seems as if ctrl-C also generates (concurrently) a break/close signal
        // so some cleanup may have started. nevertheless, we interpret CTRL_C_EVENT
        // as requiring immediate exit.
        ExitProcess(1);
        return TRUE;
    }
    return FALSE;
}
