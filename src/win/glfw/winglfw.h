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

#pragma once

#include "navitab/window.h"
#include <vector>
#include <GLFW/glfw3.h>
#include "navitab/logger.h"

namespace navitab {

class WindowGLFW : public Window
{
public:
    WindowGLFW();
    ~WindowGLFW();

    // Implementation of the Window interface
    void SetPrefs(std::shared_ptr<Preferences> prefs) override;
    void Connect(std::shared_ptr<WindowEvents> core) override;
    void Disconnect() override;
    int EventLoop(int maxLoops) override;
    void Brightness(int percent) override;

protected:
    void CreateWindow();
    void DestroyWindow();
    void RenderFrame();
    void RenderPart(int part, int left, int top, int right, int bottom);

protected:
    void onMouse(int button, int action, int flags);
    void onScrollWheel(double x, double y);
    void onKey(int key, int scanCode, int action, int mods);
    void onChar(unsigned int c);

private:
    enum {
        CANVAS,
        TOOLBAR,
        MODEBAR,
        DOODLEPAD,
        KEYPAD,
        PART_COUNT
    };

    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowEvents> coreWinCallbacks;
    std::unique_ptr<logging::Logger> LOG;

    GLFWwindow* window;
    GLuint textureNames[PART_COUNT];

    // TODO - move the ImageRectangle structure into a core header file
    struct ImageRectangle {
        int imageWidth;
        int imageHeight;
        std::vector<uint32_t> imageBuffer;
        bool glRegistered;
        ImageRectangle(int w, int h) : imageWidth(w), imageHeight(h) { imageBuffer.resize(w * h); }
        int Width() const { return imageWidth; }
        int Height() const { return imageHeight; }
        uint32_t* Row(int r) { return &imageBuffer[r * imageWidth]; }
    };
    std::unique_ptr<ImageRectangle> partImages[PART_COUNT];

    int winResizePollTimer;
    int winWidth;
    int winHeight;

    float brightness;
    float bDelta;
};

}
