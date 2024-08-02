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

#include <memory>
#include "navitab/window.h"
#include "navitab/logger.h"

namespace navitab {

class WindowMSFS : public Window
{
public:
    WindowMSFS();
    ~WindowMSFS();

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

private:
    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowEvents> coreWinCallbacks;
    std::unique_ptr<logging::Logger> LOG;

};

}
