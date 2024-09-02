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

#include "htmlserver.h"
#if defined(_WIN32)
#include <Windows.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#if !defined(_MSC_VER)
#include <unistd.h> // check required
#endif
#include <iostream>
#include <sstream>
#include <fmt/core.h>
#include "winhttp.h"
#include "httpreq.h"

namespace navitab {

// these are added to aid with winsock/bsd portability
#if !defined(_WIN32)
const int SOCKET_ERROR = -1;
const int SD_BOTH = SHUT_RDWR;

inline int closesocket(int s)
{
    return close(s);
}

inline int lastError()
{
    return errno;
}
#else
inline int lastError()
{
    return WSAGetLastError();
}
#endif

const int REQ_BUFFER_SIZE = 4096;

HtmlServer::HtmlServer(WindowHTTP *o)
:   LOG(std::make_unique<logging::Logger>("htmlsvr")),
    owner(o)
{
    reqBuffer.resize(REQ_BUFFER_SIZE);
}

HtmlServer::~HtmlServer()
{
    stop();
}

void HtmlServer::stop()
{
    serverKeepAlive = false;
    if (serverThread) {
        serverThread->join();
        serverThread.reset();
    }
    for (auto& i: panelSockets) {
        shutdown(i.first, SD_BOTH);
        closesocket(i.first);
    }
    panelSockets.clear();
}

int HtmlServer::start(int port)
{
    stop(); // stop old instance (if any)

    LOGS("Starting WinHTTP image server");

    struct addrinfo *result = nullptr;
    struct addrinfo hints;

    std::ostringstream pstr;
    pstr << port;

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
    serverThread = std::make_unique<std::thread>(&HtmlServer::serverLoop, this);

    return 0;
}

void HtmlServer::serverLoop()
{
    sockaddr_in clientAddr {};
    socklen_t clientLen = sizeof(clientAddr);

    fd_set readSet;

    timeval timeout{};
    timeout.tv_sec = 1;

    while (serverKeepAlive) {
        FD_ZERO(&readSet);
        FD_SET(httpService, &readSet);
        SOCKET fdMax = httpService;
        for (auto& i: panelSockets) {
            FD_SET(i.first, &readSet);
            fdMax = std::max(fdMax, i.first);
        }

        auto sr = select(fdMax + 1, &readSet, nullptr, nullptr, &timeout);
        if (sr < 0) {
            LOGD(fmt::format("select() returned {}", sr));
            LOGD(fmt::format("Last error was {}", lastError()));
            break;
        }

        if (FD_ISSET(httpService, &readSet)) {
            auto ps = accept(httpService, (sockaddr *) &clientAddr, &clientLen);
            panelSockets[ps] = std::make_unique<HttpReq>();
            LOGD(fmt::format("Accepted winhttp client on {}", ps));
        }

        std::map<SOCKET, int> toBeClosed;
        for (auto& i: panelSockets) {
            if (FD_ISSET(i.first, &readSet)) {
                auto n = recv(i.first, reqBuffer.data(), REQ_BUFFER_SIZE, 0);
                if (n <= 0) {
                    toBeClosed[i.first] = 1;
                }
                if (i.second->feedData(reqBuffer.data(), (int)n)) {
                    auto keepAlive = processRequest(i.first, i.second.get());
                    if (keepAlive) {
                        panelSockets[i.first] = std::make_unique<HttpReq>();
                    } else {
                        toBeClosed[i.first] = 1;
                    }
                }
            }
        }

        for (auto& i: toBeClosed) {
            LOGD(fmt::format("Closing {} after completion or receive error", i.first));
            panelSockets.erase(i.first);
            (void)shutdown(i.first, SD_BOTH);
            closesocket(i.first);
        }
    }

    closesocket(httpService);
    httpService = INVALID_SOCKET;
    LOGS("Shutdown WinHTTP image server");
}

bool HtmlServer::processRequest(SOCKET s, HttpReq *req)
{
    bool keepAlive = false;
    auto opcode = req->getUrl().substr(1,1);
    std::ostringstream header;
    std::vector<unsigned char> content;
    header << "HTTP/1.1 ";
    if (req->hasError()) {
        header << "404 NOT FOUND\r\n";
        header << "Content-Type: text/plain\r\n";
        std::string reply("Navitab: error");
        content.resize(reply.size());
        std::copy(reply.begin(), reply.end(), content.begin());
    } else {
        keepAlive = req->keepAlive();

        // Protocol supports the following events and query strings
        // RESIZE:  /r  w= h=
        // MODE:    /a  q=
        // TOOL:    /t  q=
        // MOUSE:   /m  x= y= b= wu wd
        // KEY:     /k  c=
        // STATUS:  /s
        // IMAGE:   /i

        if (opcode != "i") LOGD(fmt::format("Opcode '{}'", opcode));

        // if provided, get mouse or wheel information and send it to the driver
        std::string mx, my;
        if (req->getQueryString("mx",mx) && req->getQueryString("my",my)) {
            std::string button, wheel;

            if (req->getQueryString("mb",button)) {
                LOGD(fmt::format("Got mouse state: {} {} {}", mx, my, button));
                owner->mouseEvent(std::stoi(mx), std::stoi(my), std::stoi(button));
            }
            auto wheelUp = req->getQueryString("wu",wheel);
            auto wheelDown = req->getQueryString("wd",wheel);
            if (wheelUp || wheelDown) {
                LOGD(fmt::format("Got wheel event: {}", wheelUp ? "up" : "down"));
                owner->wheelEvent(std::stoi(mx), std::stoi(my), wheelUp ? 1 : -1);
            }
        }

        // handle panel resizing
        std::string w, h;
        if (req->getQueryString("w",w) && req->getQueryString("h",h) && opcode == "r") {
            LOGD(fmt::format("Got resize: {} x {}", w, h));
            owner->panelResize(std::stoi(w), std::stoi(h));
        }

        // TODO - handle mode/app selection (a)
        // TODO - handle tool selection (t)

        if (opcode == "i") {
            // request for latest canvas image, response is a BMP image
            header << "200 OK\r\n";
            header << "Content-Type: image/bmp\r\n";
            owner->EncodeBMP(content);
        } else if (std::string("ratmks").find(opcode) != std::string::npos) {
            // all other known opcodes will respond with the current status
            header << "200 OK\r\n";
            header << "Content-Type: text/plain\r\n";
            header << "Access-Control-Allow-Origin: *\r\n";
            std::string reply = owner->EncodeStatus();
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

    (void)send(s, header.str().c_str(), header.str().length(), 0);
    (void)send(s, reinterpret_cast<char *>(content.data()), content.size(), 0);

    // return false if the connection should be held open
    return keepAlive;
}

} // namespace navitab
