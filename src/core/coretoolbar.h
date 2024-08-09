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

#include "navitab/toolbar.h"
#include "navitab/logger.h"

namespace navitab {

// The CoreToolbar class implements the toolbar which is drawn across the top
// of the window. It regenerates an ImageRect whenever some part of it changes
// and provides this to the window to draw.
    
class CoreToolbar : public Toolbar
{
public:
    CoreToolbar(std::shared_ptr<ToolbarEvents> core);
    ~CoreToolbar();

    // APIs called from the Navitab core (sync call OK)
    void SetSimZuluTime(int h, int m, int s) override;
    void SetFrameRate(int fps) override;
    void SetEnabledTools(int selectMask) override;

protected:
    // Implementation of WindowPart
    void onResize(int w, int h) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override {}
    void onKeyEvent(int code) override {}

    // Implementation of Callback
    void AsyncCall(std::function<void ()> f) override { core->AsyncCall(f); }

private:
    const uint32_t backgroundPixels = 0xffd0d0d0;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<ToolbarEvents>  core;

};

} // namespace navitab
