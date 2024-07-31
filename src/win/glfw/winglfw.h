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

    std::shared_ptr<Toolbar> GetToolbar() override;
    std::shared_ptr<Modebar> GetModebar() override;
    std::shared_ptr<Doodlepad> GetDoodlepad() override;
    std::shared_ptr<Keypad> GetKeypad() override;
    int FrameRate() override;
    void Brightness(int percent) override;

protected:
    void CreateWindow();
    void DestroyWindow();
    void RenderFrame();

protected:
    void onMouse(int button, int action, int flags);
    void onScrollWheel(double x, double y);
    void onKey(int key, int scanCode, int action, int mods);
    void onChar(unsigned int c);

private:
    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowEvents> coreWinCallbacks;
    std::unique_ptr<logging::Logger> LOG;

    // TODO - replace all this with a FrameBuffer once we hook up to Navitab's core
    int bufferWidth;
    int bufferHeight;
    std::vector<uint32_t> imageBuffer;

    GLFWwindow* window;
    GLuint textureId;
    int winResizePollTimer;
    int winWidth;
    int winHeight;
};

}
