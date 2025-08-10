/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/modebar.h"
#include "navitab/logger.h"
#include "../lvglkit/toolkit.h"

namespace navitab {

// The CoreModebar class implements the modebar which is drawn down the left
// of the window. The modebar is used to select which screen the user wishes
// to display.
    
class CoreModebar : public Modebar, public lvglkit::Display::Updater
{
public:
    CoreModebar(std::shared_ptr<Modebar2Core> core, std::shared_ptr<lvglkit::Manager>);
    ~CoreModebar();

    // APIs called from the Navitab core
    void SetHighlightedModes(int selectMask) override;

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
    void RedrawIcons(int drawMask, int selectMask);
    Modebar2Core::Mode GetModeUnderMouse(int x, int y);

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Modebar2Core> core;
    std::shared_ptr<lvglkit::Manager> uiMgr;
    std::shared_ptr<lvglkit::Display> uiDisplay;
    int highlightMask;

    Modebar2Core::Mode mouseDownItem;
    Modebar2Core::Mode mouseOverItem;

private:
    const int kNumSquareItems = 7;
    const int kItemWidth = 40;
    const int kItemHeight = 40;
    const int kKeypadHeight = 24;
};

} // namespace navitab
