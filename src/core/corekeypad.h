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
    void Show() override;
    void Hide() override;

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
