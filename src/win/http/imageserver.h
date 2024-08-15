/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2023 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

//#include <WinSock2.h>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <queue>
#include <functional>
#include "navitab/logger.h"

namespace navitab {

class WindowHTTP;
class HttpReq;

class PanelServer {
public:
    PanelServer(WindowHTTP *);
    virtual ~PanelServer();

    int start(int port); // returns 0=ok, -1=port-busy, -2=general-failure
    void stop();

    int key();

private:
    void listenLoop();
    void connectionLoop();
    bool processRequest(HttpReq *req); // return true if the connection should be held open
    void processStdin();

private:
    std::unique_ptr<logging::Logger> LOG;
    WindowHTTP *const owner;
    const int INVALID_SOCKET = -1;
    int httpService = INVALID_SOCKET;
    int panelSocket = INVALID_SOCKET;
#if 0 // original windows code
    //SOCKET httpService = INVALID_SOCKET;
    //SOCKET panelSocket = INVALID_SOCKET;
#endif
    std::atomic_bool serverKeepAlive { false };
    std::unique_ptr<std::thread> serverThread;
    const int REQ_BUFFER_SIZE = 4096;
    std::unique_ptr<char[]> reqBuffer;
    std::vector<char> respBuffer;
    std::queue<int> keys;
    std::mutex keyMutex;
};

} // namespace navitab
