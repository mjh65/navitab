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

// inspired by GlfwGUIDriver.cpp in AviTab.

#include "winglfw.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fmt/core.h>
#include "navitab/core.h"
#include "svg/sample_64x64.h"

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowGLFW>();
}

namespace navitab {

WindowGLFW::WindowGLFW()
:   LOG(std::make_unique<logging::Logger>("winglfw")),
    bufferWidth(WIN_STD_WIDTH),
    bufferHeight(WIN_STD_HEIGHT),
    window(nullptr),
    textureId(0),
    winResizePollTimer(0),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT)
{
    if (!glfwInit()) {
        throw StartupError("Couldn't initialize GLFW");
    }
    imageBuffer.resize(bufferWidth * bufferHeight);
}

WindowGLFW::~WindowGLFW()
{
    assert(!window);
    glfwTerminate();
}

void WindowGLFW::SetPrefs(std::shared_ptr<Preferences> p)
{
    prefs = p;
    // TODO - restore previous window width and height from prefs
}

void WindowGLFW::Connect(std::shared_ptr<WindowEvents> wcb)
{
    coreWinCallbacks = wcb;
    coreWinCallbacks->onWindowResize(winWidth, winHeight);
    CreateWindow();
}

void WindowGLFW::Disconnect()
{
    DestroyWindow();
    coreWinCallbacks.reset();
    // TODO - write window size preferences
}

int WindowGLFW::EventLoop(int maxLoops)
{
    if (glfwWindowShouldClose(window)) {
        return -1;
    }

    // check the window position and size. don't need to do this every frame, to keep the overheads down
    if (++winResizePollTimer > 30) {
        winResizePollTimer = 0;
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        if ((w != winWidth) || (h != winHeight)) {
            coreWinCallbacks->onWindowResize(w, h);
            winWidth = w; winHeight = h;
        }
    }

    // TODO - this is just here for development and testing. of course it will get
    // replaced eventually!
    if (rand() % 60) {
        // write random pixels
        for (int i = 0; i < 8; ++i) {
            size_t rp = ((rand() << 20) + rand()) % imageBuffer.size();
            // red in 7:0, green in 15:8, blue in 23:16, alpha in 31:24
            imageBuffer[rp] = (rand() % 0xffffff);
        }
    } else {
        // draw one of our generated SVG icons to test the generator
        auto y0 = rand() % (bufferHeight - sample_64x64_HEIGHT);
        auto x0 = rand() % (bufferWidth - sample_64x64_WIDTH);
        for (int y=0; y < sample_64x64_HEIGHT; ++y) {
            for (int x=0; x < sample_64x64_WIDTH; ++x) {
                auto si = y * sample_64x64_WIDTH + x;
                auto di = (y + y0) * bufferWidth + (x + x0);
                imageBuffer[di] = sample_64x64[si];
            }
        }
    }

    RenderFrame();

    glfwPollEvents();
    // TODO - generate button down events on first press
    // TODO - generate mouse movements when either button is active
    // TODO - generate button up events on release
    // TODO - scroll wheel handling


    return 0; // should return >0 during mouse movements, not sure if this will be useful though.
}

std::shared_ptr<Toolbar> WindowGLFW::GetToolbar()
{
    UNIMPLEMENTED("GetToolbar");
    return nullptr;
}

std::shared_ptr<Modebar> WindowGLFW::GetModebar()
{
    UNIMPLEMENTED("GetModebar");
    return nullptr;
}

std::shared_ptr<Doodlepad> WindowGLFW::GetDoodlepad()
{
    UNIMPLEMENTED("GetDoodlepad");
    return nullptr;
}

std::shared_ptr<Keypad> WindowGLFW::GetKeypad()
{
    UNIMPLEMENTED("GetKeypad");
    return nullptr;
}

int WindowGLFW::FrameRate()
{
    // TODO - measure this, perhaps average of last 20 frames??
    return 1;
}

void WindowGLFW::Brightness(int percent)
{
    // TODO
}

void WindowGLFW::CreateWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, true);
    window = glfwCreateWindow(bufferWidth, bufferHeight, "Navitab", nullptr, nullptr);

    if (!window) {
        throw StartupError("Couldn't create GLFW window");
    }

    glfwSetWindowSizeLimits(window, WIN_MIN_WIDTH, WIN_MIN_HEIGHT, WIN_MAX_WIDTH, WIN_MAX_HEIGHT);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 1 to enable vsync and avoid tearing, 0 to benchmark

    // event callbacks to feed into Navitab core
    glfwSetWindowUserPointer(window, this);
#if 0 // TODO - probably not needed, we'll just poll the pointer position when the buttons are down
    glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double x, double y) {
        WindowGLFW* us = reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(wnd));
        int w, h;
        glfwGetWindowSize(wnd, &w, &h);
        //us->mouseX = x / w * us->width();
        //us->mouseY = y / h * us->height();
        });
#endif
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int button, int action, int flags) {
        reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(wnd))->onMouse(button, action, flags);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* wnd, double x, double y) {
        reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(wnd))->onScrollWheel(x, y);
    });
    glfwSetKeyCallback(window, [](GLFWwindow* wnd, int key, int scanCode, int action, int mods) {
        reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(wnd))->onKey(key, scanCode, action, mods);
    });
    glfwSetCharCallback(window, [](GLFWwindow* wnd, unsigned int c) {
        reinterpret_cast<WindowGLFW*>(glfwGetWindowUserPointer(wnd))->onChar(c);
    });

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGBA, bufferWidth, bufferHeight, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void WindowGLFW::DestroyWindow()
{
    assert(window);
    glfwDestroyWindow(window);
    window = nullptr;
}

void WindowGLFW::RenderFrame()
{
    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);

    glViewport(0, 0, winWidth, winHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, winWidth, winHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glEnable(GL_TEXTURE_2D);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
        0, 0, bufferWidth, bufferHeight,
        GL_RGBA, GL_UNSIGNED_BYTE, imageBuffer.data());

    glColor3f(1.0f, 1.0f, 1.0f); // TODO - brightness control

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i(0, 0);
    glTexCoord2i(0, 1);  glVertex2i(0, winHeight);
    glTexCoord2i(1, 1);  glVertex2i(winWidth, winHeight);
    glTexCoord2i(1, 0);  glVertex2i(winWidth, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSwapBuffers(window);
}

void WindowGLFW::onMouse(int button, int action, int flags)
{
    UNIMPLEMENTED("onMouse");
}

void WindowGLFW::onScrollWheel(double x, double y)
{
    UNIMPLEMENTED("onWheel");
}

void WindowGLFW::onKey(int key, int scanCode, int action, int mods)
{
    UNIMPLEMENTED("onKey");
}

void WindowGLFW::onChar(unsigned int c)
{
    UNIMPLEMENTED("onChar");
}

} // namespace navitab
