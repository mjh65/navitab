/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <chrono>
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
    void SetRepeatingTools(int selectMask) override;

protected:
    // Implementation of WindowPart
    void onResize(int w, int h) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override {}
    void onKeyEvent(int code) override {}

    // Implementation of DeferredJobRunner
    void RunLater(std::function<void ()> f, void* s = nullptr) override { core->RunLater(f); }

    // Implementation of lvglkit::Display::Updater
    void Update(navitab::ImageRegion r, uint32_t* pixels) override;

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
    std::vector<int> activeToolIds;
    int activeToolsMask;
    int pendingToolsMask;
    int repeatingToolsMask;
    int pushedToolIdx;
    bool longPressed;
    std::chrono::time_point<std::chrono::steady_clock> nextRepeatTime;
};

} // namespace navitab
