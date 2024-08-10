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

#include "navitab/doodler.h"
#include "navitab/logger.h"

namespace navitab {

// The CoreToolbar class implements the toolbar which is drawn across the top
// of the window. It regenerates an ImageRect whenever some part of it changes
// and provides this to the window to draw.
    
class CoreDoodler : public Doodler
{
public:
    CoreDoodler(std::shared_ptr<Doodler2Core> core);
    ~CoreDoodler();

    // APIs called from the core
    void Enable() override;
    void Disable() override;

protected:
    // Implementation of WindowPart
    void onResize(int w, int h) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override {}
    void onKeyEvent(int code) override;

    // Implementation of DeferredJobRunner
    void RunLater(std::function<void ()> f) override { core->RunLater(f); }

private:
    const uint32_t backgroundPixels = 0x10000000;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Doodler2Core> core;
    std::unique_ptr<FrameBuffer> oldDoodle;
    bool enabled;

};

} // namespace navitab
