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
#include <mutex>
#include <GLFW/glfw3.h>
#include "../imagerect.h"
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
    void SetHandlers(std::shared_ptr<Toolbar>, std::shared_ptr<Modebar>, std::shared_ptr<Doodler>, std::shared_ptr<Keypad>) override;
    std::unique_ptr<ImageRectangle> RefreshCanvas(std::unique_ptr<ImageRectangle>) override;
    std::unique_ptr<ImageRectangle> RefreshToolbar(std::unique_ptr<ImageRectangle>) override;
    std::unique_ptr<ImageRectangle> RefreshModebar(std::unique_ptr<ImageRectangle>) override;
    std::unique_ptr<ImageRectangle> RefreshDoodler(std::unique_ptr<ImageRectangle>) override;
    std::unique_ptr<ImageRectangle> RefreshKeypad(std::unique_ptr<ImageRectangle>) override;
    void Brightness(int percent) override;

protected:
    void CreateWindow();
    void DestroyWindow();
    std::unique_ptr<ImageRectangle> Swap(int part, std::unique_ptr<ImageRectangle>);
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
        DOODLER,
        KEYPAD,
        PART_COUNT
    };

    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowEvents> coreWinCallbacks;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Toolbar> toolbar;
    std::shared_ptr<Modebar> modebar;
    std::shared_ptr<Doodler> doodler;
    std::shared_ptr<Keypad> keypad;

    GLFWwindow* window;
    GLuint textureNames[PART_COUNT];

    std::unique_ptr<ImageRectangle> partImages[PART_COUNT];
    std::mutex imageMutex;

    int winResizePollTimer;
    int winWidth;
    int winHeight;

    float brightness;
    float bDelta;
};

}
