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
// when the simulation enters and leaves VR mode.

class XPlaneWindow : public Window
{
public:
    XPlaneWindow(const char *logId);
    ~XPlaneWindow();

    virtual void Create(std::shared_ptr<Preferences> prefs, std::shared_ptr<WindowEvents> core) = 0;
    virtual void Destroy() = 0;

    virtual void Recentre() = 0;

    // common behaviour
    void Show();
    void onFlightLoop();
    bool isActive();

protected:
    // these get called internally from the Create/Destroy functions.
    void SetPrefs(std::shared_ptr<Preferences> prefs) override;
    void Connect(std::shared_ptr<WindowEvents> core) override;
    void Disconnect() override;

protected:
    struct WindowPos {
        int left;
        int top;
        int right;
        int bottom;
        WindowPos(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
        WindowPos() : left(0), top(0), right(0), bottom(0) {}
        WindowPos(std::pair<int, int>);
    };

protected:
    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowEvents> core;

    std::unique_ptr<logging::Logger> LOG;
    XPLMWindowID winHandle;
    bool winVisible;
    int winClosedWatchdog;

protected:
    enum {
        WIN_MIN_WIDTH = 400,
        WIN_STD_WIDTH = 600,
        WIN_MAX_WIDTH = 1600,
        WIN_MIN_HEIGHT = 200,
        WIN_STD_HEIGHT = 300,
        WIN_MAX_HEIGHT = 800
    };

};


} // namespace navitab
