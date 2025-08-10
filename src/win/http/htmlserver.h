/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#if defined(_WIN32)
#include <WinSock2.h>
#endif
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <queue>
#include <functional>
#include "navitab/logger.h"

// these are added to aid with winsock/bsd portability
#if !defined(_WIN32)
#define SOCKET int
const SOCKET INVALID_SOCKET = -1;
#endif

namespace navitab {

class WindowHTTP;
class HttpReq;

class HtmlServer
{
public:
    HtmlServer(WindowHTTP *);
    virtual ~HtmlServer();

    int start(int port); // returns 0=ok, -1=port-busy, -2=general-failure
    void stop();

    int key();

private:
    void serverLoop();
    bool processRequest(SOCKET s, HttpReq *req); // return true if the connection should be held open

private:
    std::unique_ptr<logging::Logger> LOG;
    WindowHTTP *const owner;
    SOCKET httpService = INVALID_SOCKET;
    std::map<SOCKET, std::unique_ptr<HttpReq>> panelSockets;
    std::atomic_bool serverKeepAlive { false };
    std::unique_ptr<std::thread> serverThread;
    std::vector<char> reqBuffer;
    std::vector<char> respBuffer;
};

} // namespace navitab
