/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/keypad.h"
#include "navitab/logger.h"

namespace navitab {

// The CoreKeypad class implements the keypad which can be enabled or disabled by the
// user or by the screen currently in use if keyboard input is required.
    
class CoreKeypad : public Keypad
{
public:
    CoreKeypad(std::shared_ptr<Keypad2Core> core);
    ~CoreKeypad();

    // APIs called from the application/plugin
    void ShowKeypad() override;
    void HideKeypad() override;

protected:
    // Implementation of WindowPart
    void onResize(int w, int h) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onWheelEvent(int x, int y, int xdir, int ydir) override {}
    void onKeyEvent(int code) override {}

    // Implementation of DeferredJobRunner
    void RunLater(std::function<void ()> f, void*s = nullptr) override { core->RunLater(f); }

private:
    const uint32_t backgroundPixels = 0x10202020;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Keypad2Core> core;
    bool visible;

};

} // namespace navitab
