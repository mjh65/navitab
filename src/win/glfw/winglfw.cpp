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

void WindowGLFW::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->PrefsManager();
    // TODO - read window size preferences
    for (auto i = 0; i < PART_COUNT; ++i) {
        parts[i] = core->GetPartCallbacks(i);
        parts[i]->SetPainter(shared_from_this());
    }
    CreateWindow();
    ResizeNotifyAll(winWidth, winHeight);
}

void WindowGLFW::Disconnect()
{
    // TODO - write window size preferences
    DestroyWindow();
    for (auto i = 0; i < PART_COUNT; ++i) {
        parts[i].reset();
    }
    prefs.reset();
    core.reset();
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
            winWidth = w; winHeight = h;
            ResizeNotifyAll(w, h);
        }
    }

    RenderFrame();

    glfwPollEvents();
    // TODO - generate button down events on first press
    // TODO - generate mouse movements when either button is active
    // TODO - generate button up events on release
    // TODO - scroll wheel handling
    // TODO - check key events and forward useful stuff

    return 0; // should return >0 during mouse movements, not sure if this will be useful though?
}

void WindowGLFW::Brightness(int percent)
{
    if (percent < 0) percent = 0;
    else if (percent > 100) percent = 100;
    brightness = 0.1f + (0.9f * percent / 100.0f);
}

std::unique_ptr<ImageRectangle> WindowGLFW::RefreshPart(int part, std::unique_ptr<ImageRectangle> newImage)
{
    // This function is called from the core thread.
    const std::lock_guard<std::mutex> lock(imageMutex);
    std::unique_ptr<ImageRectangle> returnedImage;
    if (partImages[part]) partImages[part]->Reset();
    returnedImage = std::move(partImages[part]);
    partImages[part] = std::move(newImage);
    return returnedImage;
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
    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);

    glViewport(0, 0, fbw, fbh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, winWidth, winHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT);
    glColor4f(brightness, brightness, brightness, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    RenderPart(PART_TOOLBAR, 0, 0, winWidth, TOOLBAR_HEIGHT);
    RenderPart(PART_MODEBAR, 0, TOOLBAR_HEIGHT, MODEBAR_WIDTH, TOOLBAR_HEIGHT + MODEBAR_HEIGHT);
    RenderPart(PART_DOODLER, MODEBAR_WIDTH, TOOLBAR_HEIGHT, winWidth, winHeight);
    RenderPart(PART_KEYPAD, MODEBAR_WIDTH, winHeight - KEYPAD_HEIGHT, winWidth, winHeight);
    RenderPart(PART_CANVAS, 0, TOOLBAR_HEIGHT, winWidth, winHeight);

    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSwapBuffers(window);
}

void WindowGLFW::RenderPart(int part, int left, int top, int right, int bottom)
{
    const std::lock_guard<std::mutex> lock(imageMutex);
    if (!partImages[part]) return;

    auto& image = *(partImages[part]);

    glBindTexture(GL_TEXTURE_2D, textureNames[part]);
    if (image.NeedsRegistration()) {
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, image.Width(), image.Height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, image.Row(0));
    }

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

void WindowGLFW::ResizeNotifyAll(int w, int h)
{
    parts[PART_TOOLBAR]->PostResize(w, TOOLBAR_HEIGHT);
    parts[PART_MODEBAR]->PostResize(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    parts[PART_DOODLER]->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
    parts[PART_KEYPAD]->PostResize(w - MODEBAR_WIDTH, KEYPAD_HEIGHT);
    parts[PART_CANVAS]->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
}

void WindowGLFW::onMouse(int button, int action, int flags)
{
    UNIMPLEMENTED(__func__);
}

void WindowGLFW::onScrollWheel(double x, double y)
{
    UNIMPLEMENTED(__func__);
}

void WindowGLFW::onKey(int key, int scanCode, int action, int mods)
{
    UNIMPLEMENTED(__func__);
}

void WindowGLFW::onChar(unsigned int c)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
