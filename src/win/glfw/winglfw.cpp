/* This file is part of the Navitab project. See the README and LICENSE for details. */

// inspired by GlfwGUIDriver.cpp in AviTab.

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <math.h>
#include <fmt/core.h>
#include "winglfw.h"
#include "../texbuffer.h"
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
    latestMouse({0,0,0}),
    activeWinPart(nullptr)
{
    glfwSetErrorCallback(GLFWERR);
    if (!glfwInit()) {
        throw StartupError("Couldn't initialize GLFW");
    }

    for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
        winParts[i].textureId = 0;
        winParts[i].active = 0;
        winParts[i].top = winParts[i].left = 0;
    }
    winParts[WindowPart::CANVAS].textureImage = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, (WIN_MAX_HEIGHT - TOOLBAR_HEIGHT));
    winParts[WindowPart::CANVAS].top = TOOLBAR_HEIGHT;
    winParts[WindowPart::TOOLBAR].textureImage = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, TOOLBAR_HEIGHT);
    winParts[WindowPart::MODEBAR].textureImage = std::make_unique<TextureBuffer>(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    winParts[WindowPart::MODEBAR].top = TOOLBAR_HEIGHT;
    winParts[WindowPart::DOODLER].textureImage = std::make_unique<TextureBuffer>((WIN_MAX_WIDTH - MODEBAR_WIDTH), (WIN_MAX_HEIGHT - TOOLBAR_HEIGHT));
    winParts[WindowPart::DOODLER].top = TOOLBAR_HEIGHT;
    winParts[WindowPart::KEYPAD].textureImage = std::make_unique<TextureBuffer>(WIN_MAX_WIDTH, KEYPAD_HEIGHT);
    winParts[WindowPart::KEYPAD].top = winHeight - KEYPAD_HEIGHT;
    winParts[WindowPart::KEYPAD].left = MODEBAR_WIDTH;
}

WindowGLFW::~WindowGLFW()
{
    assert(!window);
    glfwTerminate();
}

void WindowGLFW::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    core->SetWindowControl(shared_from_this());
    prefs = core->GetSettingsManager();
    // TODO - read window size preferences

    winParts[WindowPart::TOOLBAR].client = core->GetToolbar();
    winParts[WindowPart::TOOLBAR].client->SetPainter(shared_from_this());
    winParts[WindowPart::MODEBAR].client = core->GetModebar();
    winParts[WindowPart::MODEBAR].client->SetPainter(shared_from_this());
    winParts[WindowPart::DOODLER].client = core->GetDoodler();
    winParts[WindowPart::DOODLER].client->SetPainter(shared_from_this());
    winParts[WindowPart::KEYPAD].client = core->GetKeypad();
    winParts[WindowPart::KEYPAD].client->SetPainter(shared_from_this());
    winParts[WindowPart::CANVAS].client = core->GetAppCanvas();
    winParts[WindowPart::CANVAS].client->SetPainter(shared_from_this());

    CreateWindow();

    ResizeNotifyAll(winWidth, winHeight);
}

void WindowGLFW::Disconnect()
{
    // TODO - write window size preferences
    DestroyWindow();
    for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
        winParts[i].client.reset();
    }
    prefs.reset();
    core.reset();
}

void WindowGLFW::EventLoop()
{
    while (1) {
        if (glfwWindowShouldClose(window)) {
            return;
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

        if (!pendingButtonEvents.empty()) {
            // there are pending button events to deal with
            MouseState m1 = pendingButtonEvents.front();
            pendingButtonEvents.pop_front();
            if (m1.b && !latestMouse.b) {
                // the button went down
                activeWinPart = LocateWinPart(m1.x, m1.y);
                activeWinPart->client->PostMouseEvent(m1.x - activeWinPart->left, m1.y - activeWinPart->top, true, false);
            } else if (!m1.b && latestMouse.b) {
                // the button was released
                activeWinPart->client->PostMouseEvent(m1.x - activeWinPart->left, m1.y - activeWinPart->top, false, false);
                activeWinPart = nullptr;
            }
            latestMouse = m1;
        } else if (latestMouse.b) {
            // the button remains down and the mouse may have moved
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            latestMouse.x = int(floor(x));
            latestMouse.y = int(floor(y));
            assert(activeWinPart);
            activeWinPart->client->PostMouseEvent(latestMouse.x - activeWinPart->left, latestMouse.y - activeWinPart->top, true, false);
        }

        // TODO - scroll wheel handling

        // TODO - check key events and forward useful stuff
    }
}

void WindowGLFW::ResizeNotifyAll(int w, int h)
{
    winParts[WindowPart::KEYPAD].top = h - KEYPAD_HEIGHT;

    winParts[WindowPart::CANVAS].client->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
    winParts[WindowPart::TOOLBAR].client->PostResize(w, TOOLBAR_HEIGHT);
    winParts[WindowPart::MODEBAR].client->PostResize(MODEBAR_WIDTH, MODEBAR_HEIGHT);
    winParts[WindowPart::DOODLER].client->PostResize(w - MODEBAR_WIDTH, h - TOOLBAR_HEIGHT);
    winParts[WindowPart::KEYPAD].client->PostResize(w - MODEBAR_WIDTH, KEYPAD_HEIGHT);
}

void WindowGLFW::Brightness(int percent)
{
    if (percent < 0) percent = 0;
    else if (percent > 100) percent = 100;
    brightness = 0.1f + (0.9f * percent / 100.0f);
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

    GLuint texIds[WindowPart::TOTAL_PARTS];
    glGenTextures(WindowPart::TOTAL_PARTS, texIds);
    for (auto i = 0; i < WindowPart::TOTAL_PARTS; ++i) {
        winParts[i].textureId = texIds[i];
        glBindTexture(GL_TEXTURE_2D, winParts[i].textureId);
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

void WindowGLFW::Paint(int part, const FrameBuffer* src, const std::vector<ImageRegion>& regions)
{
    // This function is called from the core thread.
    const std::lock_guard<std::mutex> lock(paintMutex);
    auto& wp = winParts[part];
    wp.active = src && regions.size();
    if (!wp.active) return;
    auto& ti = wp.textureImage;
    if ((ti->Width() != src->Width()) || (ti->Height() != src->Height())) {
        ti->Resize(src->Width(), src->Height());
    }
    ti->CopyRegionsFrom(src, regions);
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

    // !rendering order is important!
    RenderPart(WindowPart::CANVAS, 0, TOOLBAR_HEIGHT, winWidth, winHeight);
    RenderPart(WindowPart::TOOLBAR, 0, 0, winWidth, TOOLBAR_HEIGHT);
    RenderPart(WindowPart::MODEBAR, 0, TOOLBAR_HEIGHT, MODEBAR_WIDTH, TOOLBAR_HEIGHT + MODEBAR_HEIGHT);
    RenderPart(WindowPart::DOODLER, MODEBAR_WIDTH, TOOLBAR_HEIGHT, winWidth, winHeight);
    RenderPart(WindowPart::KEYPAD, MODEBAR_WIDTH, winHeight - KEYPAD_HEIGHT, winWidth, winHeight);

    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSwapBuffers(window);
}

void WindowGLFW::RenderPart(int part, int left, int top, int right, int bottom)
{
    const std::lock_guard<std::mutex> lock(paintMutex);
    if (!winParts[part].active) return;
    assert(winParts[part].textureImage);

    auto& buffer = *(winParts[part].textureImage);

    glBindTexture(GL_TEXTURE_2D, winParts[part].textureId);
    if (buffer.NeedsRegistration()) {
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, buffer.Width(), buffer.Height(), 0,
            GL_RGBA, GL_UNSIGNED_BYTE, buffer.Data());
    }

    glEnable(GL_TEXTURE_2D);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
        0, 0, buffer.Width(), buffer.Height(),
        GL_RGBA, GL_UNSIGNED_BYTE, buffer.Data());

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
    if (button != GLFW_MOUSE_BUTTON_LEFT) return; // only left button events are of interest
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    MouseState m { int(floor(x)), int(floor(y)), (action == GLFW_PRESS) };
    pendingButtonEvents.push_back(m);
}

void WindowGLFW::onScrollWheel(double x, double y)
{
    UNIMPLEMENTED(__func__ + fmt::format("({},{})", x, y));
}

void WindowGLFW::onKey(int key, int scanCode, int action, int mods)
{
    UNIMPLEMENTED(__func__ + fmt::format("({},{},{},{})", key, scanCode, action, mods));
}

void WindowGLFW::onChar(unsigned int c)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", c));
}

WindowGLFW::WinPart *WindowGLFW::LocateWinPart(int x, int y)
{
    if (y < TOOLBAR_HEIGHT) {
        return &winParts[WindowPart::TOOLBAR];
    }
    if ((x < MODEBAR_WIDTH) && (y < (TOOLBAR_HEIGHT + MODEBAR_HEIGHT))) {
        return &winParts[WindowPart::MODEBAR];
    }
    if (winParts[WindowPart::KEYPAD].active && (y >= (winHeight - KEYPAD_HEIGHT))) {
        return &winParts[WindowPart::KEYPAD];
    }
    if (winParts[WindowPart::DOODLER].active) {
        return &winParts[WindowPart::DOODLER];
    }
    return &winParts[WindowPart::CANVAS];
}

} // namespace navitab
