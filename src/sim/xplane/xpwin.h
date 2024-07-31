/*
 *  Navitab - Navigation tablet for VR flight simulation
 *  Copyright (C) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (C) 2018-2024 Folke Will <folko@solhost.org>
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
#include <XPLM/XPLMDisplay.h>
#include "navitab/logger.h"

namespace navitab {

// XPlaneWindow extends Navitab Window to provide a common base class for
// XP desktop and VR windows. This allows the XP plugin to swap between these
// specialisations when the simulation enters and leaves VR mode.

class XPlaneWindow : public Window
{
public:
    XPlaneWindow(const char *logId);
    ~XPlaneWindow();

    virtual void Create(std::shared_ptr<Preferences> prefs, std::shared_ptr<WindowEvents> core) = 0;
    virtual void Destroy() = 0;

    virtual void Reset() = 0;

    // Implementation of navitab::Window, common to desktop and VR
    std::shared_ptr<Toolbar> GetToolbar() override;
    std::shared_ptr<Modebar> GetModebar() override;
    std::shared_ptr<Doodlepad> GetDoodlepad() override;
    std::shared_ptr<Keypad> GetKeypad() override;
    int FrameRate() override;
    void Brightness(int percent) override;

    // common behaviour
    void Show();
    void onFlightLoop();
    bool isActive();

protected:
    // these get called internally from the Create/Destroy functions.
    void SetPrefs(std::shared_ptr<Preferences> prefs) override;
    void Connect(std::shared_ptr<WindowEvents> core) override;
    void Disconnect() override;
    int EventLoop(int maxLoops) override { return 0; } // null implementation, not used

    void ProdWatchdog();
    bool UpdateWinGeometry(); // returns true if the size changed
    bool isVisible() const { return winVisible; }
    void ScreenToWindow(int& x, int& y);

protected:
    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowEvents> core;

    std::unique_ptr<logging::Logger> LOG;
    XPLMWindowID winHandle;
    int winWidth, winHeight;

private:
    int winDrawWatchdog;
    int wgl, wgt, wgr, wgb; // most recently observed window geometry
    bool winVisible;

};


} // namespace navitab
