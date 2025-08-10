/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/doodler.h"
#include "navitab/logger.h"

namespace navitab {

// The CoreDoodler class implements the doodler drawing area which can be activated
// to allow the user to draw or annotate on top of the canvas. 
    
class CoreDoodler : public Doodler
{
public:
    CoreDoodler(std::shared_ptr<Doodler2Core> core);
    ~CoreDoodler();

    // APIs called from the core
    void EnableDoodler() override;
    void DisableDoodler() override;

protected:
    // Implementation of WindowPart
    void onResize(int w, int h) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override {}
    void onKeyEvent(int code) override;

    // Implementation of DeferredJobRunner
    void RunLater(std::function<void ()> f, void* s = nullptr) override { core->RunLater(f); }

private:
    const uint32_t backgroundPixels = 0x10000000;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Doodler2Core> core;
    std::unique_ptr<FrameBuffer> oldDoodle;
    bool enabled;

};

} // namespace navitab
