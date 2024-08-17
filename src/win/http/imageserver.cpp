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

#include "imageserver.h"
#if defined(_WIN32)
#include <Windows.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fmt/core.h>
#include "winhttp.h"
#include "protocol.h"

namespace navitab {


// these were added to support linux but keep the windows source code largely unchanged
#if !defined(_WIN32)
const int SOCKET_ERROR = -1;
const int SD_BOTH = SHUT_RDWR;
#endif

inline int lastError()
{
    return errno;
    // return WSAGetLastError();
}

inline int closesocket(int s) // disable for windows
{
    return close(s);
}




PanelServer::PanelServer(WindowHTTP *o)
:   LOG(std::make_unique<logging::Logger>("winhttp")),
    owner(o)
{
    reqBuffer = std::make_unique<char[]>(REQ_BUFFER_SIZE);
}

PanelServer::~PanelServer()
{
    stop();
}

void PanelServer::stop()
{
    serverKeepAlive = false;
    if (panelSocket != INVALID_SOCKET) {
        shutdown(panelSocket, SD_BOTH);
        close(panelSocket); // was closesocket for windows
    }
    if (serverThread) {
        serverThread->join();
        serverThread.reset();
    }
}

int PanelServer::start(int port)
{
    stop(); // stop old instance (if any)

    LOGS("Starting WinHTTP image server");

    struct addrinfo *result = nullptr;
    struct addrinfo hints;

    std::ostringstream pstr;
    pstr << port;

    //ZeroMemory(&hints, sizeof(hints));
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    int iResult = getaddrinfo(NULL, pstr.str().c_str(), &hints, &result);
    if ( iResult != 0 ) {
        LOGE(fmt::format("getaddrinfo failed with error: {}", iResult));
        return -2;
    }

    // Create a SOCKET for the server to listen for client connections.
    httpService = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (httpService == INVALID_SOCKET) {
        LOGE(fmt::format("socket failed with error: {}", lastError()));
        freeaddrinfo(result);
        return -2;
    }

    // Setup the TCP listening socket
    iResult = bind( httpService, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        LOGE(fmt::format("bind failed with error: %d", lastError()));
        freeaddrinfo(result);
        closesocket(httpService);
        return -1;
    }

    freeaddrinfo(result);

    iResult = listen(httpService, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        LOGE(fmt::format("listen failed with error: %d", lastError()));
        closesocket(httpService);
        return -2;
    }

    serverKeepAlive = true;
    serverThread = std::make_unique<std::thread>(&PanelServer::listenLoop, this);

    return 0;
}

void PanelServer::listenLoop()
{
    sockaddr_in clientAddr {};
    socklen_t clientLen = sizeof(clientAddr);

    fd_set readSet;

    timeval timeout{};
    timeout.tv_usec = 1000 * 500;

    while (serverKeepAlive) {
        FD_ZERO(&readSet);
        FD_SET(STDIN_FILENO, &readSet);
        FD_SET(httpService, &readSet);
        int fdMax = httpService;

        if (select(fdMax + 1, &readSet, nullptr, nullptr, &timeout) < 0) {
            break;
        }

        if (FD_ISSET(httpService, &readSet)) {
            LOGD("Accepting winhttp client");
            panelSocket = accept(httpService, (sockaddr *) &clientAddr, &clientLen);
            connectionLoop();
            closesocket(panelSocket);
            panelSocket = INVALID_SOCKET;
        }

        if (FD_ISSET(STDIN_FILENO, &readSet)) {
            processStdin();
        }
    }

    closesocket(httpService);
    LOGS("Shutdown WinHTTP image server");
}

void PanelServer::connectionLoop()
{
    LOGD("Client has connected ...");

    bool keepConnection = true;
    while (serverKeepAlive && keepConnection) {
        auto req = std::make_unique<HttpReq>();
        while (serverKeepAlive) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(STDIN_FILENO, &readSet);
            FD_SET(panelSocket, &readSet);
            int fdMax = panelSocket;

            timeval timeout{};
            timeout.tv_sec = 1;
            auto sr = select(fdMax + 1, &readSet, nullptr, nullptr, &timeout);
            if (sr < 0) {
                LOGD(fmt::format("select() returned {}", sr));
                LOGD(fmt::format("Last error was {}", lastError()));
                keepConnection = false;
                break;
            }
            if (FD_ISSET(panelSocket, &readSet)) {
                auto n = recv(panelSocket, reqBuffer.get(), REQ_BUFFER_SIZE, 0);
                LOGD(fmt::format("recv() returned {}", n));
                if (n <= 0) {
                    keepConnection = false;
                    break;
                }
                if (req->feedData(reqBuffer.get(), (int)n)) {
                    keepConnection = processRequest(req.get());
                    break;
                }
            }
            if (FD_ISSET(STDIN_FILENO, &readSet)) {
                processStdin();
            }
        }
    }

    // cleanup
    (void)shutdown(panelSocket, SD_BOTH);
    closesocket(panelSocket);
    LOGD("Client disconnected");
}

bool PanelServer::processRequest(HttpReq *req)
{
    bool keepAlive = false;
    auto opcode = req->getUrl().substr(1,1);
    std::ostringstream header;
    std::vector<unsigned char> content;
    header << "HTTP/1.1 ";
    if (req->hasError()) {
        header << "404 NOT FOUND\r\n";
        header << "Content-Type: text/plain\r\n";
        std::string reply("AviTab: error");
        content.resize(reply.size());
        std::copy(reply.begin(), reply.end(), content.begin());
    } else {
        keepAlive = req->keepAlive();

        // if provided, get aircraft position and send this into the avitab engine
        std::string longitude, latitude, altitude, heading;
        if (req->getQueryString("lt",latitude) && req->getQueryString("ln",longitude) && req->getQueryString("al",altitude) && req->getQueryString("hg",heading)) {
            LOGD(fmt::format("Got position: {},{},{},{}", latitude, longitude, altitude, heading));
            //owner->updateAircraftLocation(std::stof(longitude), std::stof(latitude), std::stof(altitude), std::stof(heading));
        }

        // if provided, get mouse or wheel information and send it to the driver
        std::string mx, my;
        if (req->getQueryString("mx",mx) && req->getQueryString("my",my)) {
            std::string button, wheel;

            if (req->getQueryString("mb",button)) {
                LOGD(fmt::format("Got mouse state: {} {} {}", mx, my, button));
                //owner->updateMouseState(std::stoi(mx), std::stoi(my), std::stoi(button));
            }
            auto wheelUp = req->getQueryString("wu",wheel);
            auto wheelDown = req->getQueryString("wd",wheel);
            if (wheelUp || wheelDown) {
                LOGD(fmt::format("Got wheel event: {}", wheelUp ? "up" : "down"));
                //owner->updateMousePosition(std::stoi(mx), std::stoi(my));
                //owner->updateWheelState(wheelUp);
            }
        }

        // if provided, get other aircraft information and pass it to the environment
        std::string traffic;
        if (req->getQueryString("tr",traffic)) {
            LOGD(fmt::format("Got traffic information: %s", traffic.c_str()));
            //owner->updateTraffic(traffic);
        }

        if (opcode == "f") {
            // request for frame
            header << "200 OK\r\n";
            header << "Content-Type: image/bmp\r\n";
            owner->encodeBMP(content);
        } else if (opcode == "m") {
            header << "200 OK\r\n";
            header << "Content-Type: text/plain\r\n";
            header << "Access-Control-Allow-Origin: *\r\n";
            std::string reply("OKEY-DOKEY");
            content.resize(reply.size());
            std::copy(reply.begin(), reply.end(), content.begin());
        } else {
            header << "404 NOT FOUND\r\n";
            header << "Content-Type: text/plain\r\n";
            std::string reply("Naviab: unknown opcode");
            content.resize(reply.size());
            std::copy(reply.begin(), reply.end(), content.begin());
        }
    }
    header << "Content-Length: " << content.size() << "\r\n";
    if (!keepAlive) { header << "Connection: close\r\n"; }
    header << "\r\n";

    (void)send(panelSocket, header.str().c_str(), header.str().length(), 0);
    (void)send(panelSocket, reinterpret_cast<char *>(content.data()), content.size(), 0);

    // return false if the connection should be held open
    return keepAlive;
}

void PanelServer::processStdin()
{
    std::string message;
    getline(std::cin, message);
    std::lock_guard<std::mutex> lock(keyMutex);
    for (auto c: message) {
        keys.push(c);
    }
    keys.push('\n');
    if (std::cin.eof()) {
        keys.push(0);
    }
}

int PanelServer::key()
{
    std::lock_guard<std::mutex> lock(keyMutex);
    if (keys.empty()) return -1;
    auto k = keys.front();
    keys.pop();
    return k;
}

} // namespace navitab
