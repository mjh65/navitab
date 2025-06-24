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

#include <lvgl.h>
#include "navitab/toolbar.h"
#include "navitab/logger.h"
#include "../lvglkit/toolkit.h"

namespace navitab {

// The CoreToolbar class implements the toolbar which is drawn across the top
// of the window. The toolbar displays some current status text and a number of
// selectable tools, which trigger related actions on the current screen.
    
class CoreToolbar : public Toolbar, public lvglkit::Display::Updater
{
public:
    CoreToolbar(std::shared_ptr<Toolbar2Core> core, std::shared_ptr<lvglkit::Manager>);
    ~CoreToolbar();

    // APIs called from the Navitab core (sync call OK)
    void SetStausInfo(int zt, int fps, const Location& l) override;
    void SetActiveTools(int selectMask) override;

protected:
    // Implementation of WindowPart
    void onResize(int w, int h) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override {}
    void onKeyEvent(int code) override {}

    // Implementation of DeferredJobRunner
    void RunLater(std::function<void ()> f, void* s = nullptr) override { core->RunLater(f); }

    // Implementation of lvglkit::Display::Updater
    void Update(navitab::FrameRegion r, uint32_t* pixels) override;

private:
    void CreateWidgets();
    void RepaintTools(int statusTextWidth);

private:
    const uint32_t backgroundPixels = 0xff909090;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Toolbar2Core>  core;
    std::shared_ptr<lvglkit::Manager> uiMgr;
    std::shared_ptr<lvglkit::Display> uiDisplay;
    lv_obj_t* lvhStatusInfo;
    std::string statusText;
    int numActiveTools;
    int activeToolsMask;
    int pendingToolsMask;
};

} // namespace navitab
