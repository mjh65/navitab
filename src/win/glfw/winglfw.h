/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <list>
#include <vector>
#include <mutex>
#include <GLFW/glfw3.h>
#include "navitab/window.h"
#include "navitab/logger.h"

namespace navitab {

class TextureBuffer;

class WindowGLFW : public std::enable_shared_from_this<WindowGLFW>,
                   public Window, public PartPainter, public WindowControls
{
public:
    WindowGLFW();
    ~WindowGLFW();

    // Implementation of the Window interface
    void Connect(std::shared_ptr<CoreServices> core) override;
    void Disconnect() override;
    void EventLoop() override;

    // Implementation of the PartPainter interface
    void Paint(int part, const FrameBuffer* src, const std::vector<ImageRegion>& regions) override;

    // Implementation of the WindowControls interface
    void Brightness(int percent) override;

protected:
    void CreateWindow();
    void DestroyWindow();
    void RenderFrame();
    void RenderPart(int part, int left, int top, int right, int bottom);

protected:
    void ResizeNotifyAll(int w, int h);
    void onMouse(int button, int action, int flags);
    void onScrollWheel(double x, double y);
    void onKey(int key, int scanCode, int action, int mods);
    void onChar(unsigned int c);

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Settings> prefs;

    GLFWwindow* window;

    int winResizePollTimer;
    int winWidth, winHeight;
    float brightness;

private:
    struct WinPart {
        GLuint textureId;
        std::unique_ptr<TextureBuffer> textureImage;
        std::shared_ptr<WindowPart> client;
        bool active;
        int top, left;
    };

    WinPart winParts[WindowPart::TOTAL_PARTS];
    std::mutex paintMutex;

private:
    WinPart *LocateWinPart(int x, int y);

private:
    struct MouseState {
        int x, y;
        int b;
    };
    MouseState latestMouse;
    std::list<MouseState> pendingButtonEvents;
    WinPart *activeWinPart;

};

}
