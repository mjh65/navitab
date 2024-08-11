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
#include "navitab/logger.h"
#include "../lvglkit/toolkit.h"

 // The Toolbar class represents the toolbar which is drawn across the top
 // of the window. It is a fixed height, and displays some current status
 // information on the left hand side, and some tool action icons from the
 // right hand side which trigger behaviours in the currently active mode/app.

namespace navitab {

struct CanvasEvents : public DeferredJobRunner<>
{
    void PostFoo() {
        RunLater([this]() { onFoo(); });
    }

protected:
    // Called when a tool icon is clicked
    virtual void onFoo() = 0;
};

// The Canvas interface defines the services that this part of the UI window
// provides to the Navitab core.

class Canvas : public WindowPart, public lvglkit::Display::Updater
{
public:
    Canvas(std::shared_ptr<CanvasEvents> core, std::shared_ptr<lvglkit::Manager>);
    ~Canvas();

    // transient function during development, will be removed
    void UpdateProtoDevelopment();

    // Implementation of WindowPart
    void onResize(int width, int height) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override;
    void onKeyEvent(int code) override;

    // Implementation of DeferredJobRunner
    void RunLater(std::function<void ()> f, void* s = nullptr) override { core->RunLater(f); }

    // Implementation of lvglkit::Display::Updater
    void Update(navitab::FrameRegion r, uint32_t* pixels) override;

private:
    const uint32_t backgroundPixels = 0xff00df00;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<CanvasEvents> core;
    std::shared_ptr<lvglkit::Manager> uiMgr;
    std::shared_ptr<lvglkit::Display> uiDisplay;
};

} // namespace navitab
