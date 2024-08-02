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
#include <iostream>
#include <fmt/core.h>
#include "navitab/config.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"
#include "navitab/modebar.h"
#include "navitab/doodler.h"
#include "navitab/keypad.h"

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowGLFW>();
}

static void GLFWERR(int code, const char *msg)
{
    std::cerr << "GLFW reports: [" << code << "] " << msg << std::endl;
}

namespace navitab {

WindowGLFW::WindowGLFW()
:   LOG(std::make_unique<logging::Logger>("winglfw")),
    window(nullptr),
    winResizePollTimer(0),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT),
    brightness(1.0f),
    bDelta(-0.002f)
{
    for (auto i = 0; i < PART_COUNT; ++i) {
        textureNames[i] = 0;
    }
    partImages[CANVAS] = std::make_unique<ImageRectangle>(WIN_STD_WIDTH, WIN_STD_HEIGHT - TOOLBAR_HEIGHT);
    partImages[TOOLBAR] = std::make_unique<ImageRectangle>(WIN_STD_WIDTH, TOOLBAR_HEIGHT);
    partImages[MODEBAR] = std::make_unique<ImageRectangle>(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    partImages[DOODLER] = std::make_unique<ImageRectangle>(WIN_STD_WIDTH - MODEBAR_WIDTH, WIN_STD_HEIGHT - TOOLBAR_HEIGHT);
    partImages[KEYPAD] = std::make_unique<ImageRectangle>(WIN_STD_WIDTH - MODEBAR_WIDTH, KEYPAD_HEIGHT);

    partImages[TOOLBAR]->Clear(0xffd0d0d0); // alpha 0xff means it is opaque
    partImages[MODEBAR]->Clear(0x40f0f0f0); // alpha 0x40 is quite translucent!

    glfwSetErrorCallback(GLFWERR);
    if (!glfwInit()) {
        throw StartupError("Couldn't initialize GLFW");
    }
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
    coreWinCallbacks->PostWindowResize(winWidth, winHeight);
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
            coreWinCallbacks->PostWindowResize(w, h);
            winWidth = w; winHeight = h;
        }
    }

#if 0
    // TODO - this is just here for development and testing. of course it will get
    // replaced eventually!
    auto& canvas = *(partImages[CANVAS]);
    if (rand() % 60) {
        // write random pixels
        for (int i = 0; i < 8; ++i) {
            size_t rp = ((rand() << 20) + rand()) % canvas.imageBuffer.size();
            // red in 7:0, green in 15:8, blue in 23:16, alpha in 31:24
            canvas.imageBuffer[rp] = (rand() % 0xffffff);
        }
    } else {
        // draw one of our generated SVG icons to test the generator
        auto y0 = rand() % (canvas.Height() - sample_64x64_HEIGHT);
        auto x0 = rand() % (canvas.Width() - sample_64x64_WIDTH);
        for (int y=0; y < sample_64x64_HEIGHT; ++y) {
            for (int x=0; x < sample_64x64_WIDTH; ++x) {
                auto si = y * sample_64x64_WIDTH + x;
                auto di = (y + y0) * canvas.Width() + (x + x0);
                canvas.imageBuffer[di] = sample_64x64[si];
            }
        }
    }
#endif

    if (bDelta > 0.0f) {
        brightness += bDelta;
        if (brightness >= 1.0f) {
            bDelta = 0.0 - bDelta;
            brightness = 1.0f;
        }
    } else {
        brightness += bDelta;
        if (brightness <= 0.2f) {
            bDelta = 0.0 - bDelta;
            brightness = 0.2f;
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

void WindowGLFW::Brightness(int percent)
{
    if (percent < 0) percent = 0;
    else if (percent > 100) percent = 100;
    brightness = 0.1f + (0.9f * percent / 100.0f);
}

void WindowGLFW::SetHandlers(std::shared_ptr<Toolbar> t, std::shared_ptr<Modebar> m, std::shared_ptr<Doodler> d, std::shared_ptr<Keypad> k)
{
    toolbar = t;
    modebar = m;
    doodler = d;
    keypad = k;

    toolbar->PostResize(winWidth);
    doodler->PostResize(winWidth, winHeight);
    keypad->PostResize(winWidth, winHeight);
}

std::unique_ptr<ImageRectangle> WindowGLFW::RefreshCanvas(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED("");
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowGLFW::RefreshToolbar(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED("");
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowGLFW::RefreshModebar(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED("");
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowGLFW::RefreshDoodler(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED("");
    return nullptr;
}

std::unique_ptr<ImageRectangle> WindowGLFW::RefreshKeypad(std::unique_ptr<ImageRectangle>)
{
    UNIMPLEMENTED("");
    return nullptr;
}

void WindowGLFW::CreateWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, true);
    window = glfwCreateWindow(winWidth, winHeight, NAVITAB_NAME " " NAVITAB_VERSION_STR, nullptr, nullptr);

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

    for (auto i = 0; i < PART_COUNT; ++i) {
        glGenTextures(1, &textureNames[i]);
        glBindTexture(GL_TEXTURE_2D, textureNames[i]);
        // TODO - this appears to be required, so will need to be different for each window part
        // ?? will it need to be done again whenever the image buffer is swapped ??
        // ?? does it need doing here, or just once each time a new image buffer is delivered ??
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, partImages[i]->Width(), partImages[i]->Height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, partImages[i]->Row(0));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
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
    glColor4f(brightness, brightness, brightness, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    RenderPart(CANVAS, 0, TOOLBAR_HEIGHT, winWidth, winHeight);
    RenderPart(TOOLBAR, 0, 0, winWidth, TOOLBAR_HEIGHT);
    RenderPart(MODEBAR, 0, TOOLBAR_HEIGHT, MODEBAR_WIDTH, TOOLBAR_HEIGHT + MODEBAR_HEIGHT);
    // TODO - add doodler and keypad, if they are active

    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSwapBuffers(window);
}

void WindowGLFW::RenderPart(int part, int left, int top, int right, int bottom)
{
    auto& image = *(partImages[part]);
    glBindTexture(GL_TEXTURE_2D, textureNames[part]);
    glEnable(GL_TEXTURE_2D);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
        0, 0, image.Width(), image.Height(),
        GL_RGBA, GL_UNSIGNED_BYTE, image.Row(0));

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);  glVertex2i(left, top);
    glTexCoord2i(0, 1);  glVertex2i(left, bottom);
    glTexCoord2i(1, 1);  glVertex2i(right, bottom);
    glTexCoord2i(1, 0);  glVertex2i(right, top);
    glEnd();

    glDisable(GL_TEXTURE_2D);

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
