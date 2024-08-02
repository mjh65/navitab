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
#include <memory>

namespace navitab {

class Window;
class Navitab;
class ImageRectangle;

// The CoreToolbar class implements the toolbar which is drawn across the top
// of the window. It regenerates an ImageRect whenever some part of it changes
// and provides this to the window to draw.
    
class CoreKeypad : public Keypad
{
public:
    CoreKeypad(std::shared_ptr<Navitab> core);
    ~CoreKeypad();

    // APIs called from the application/plugin

    // APIs called from the window
    void SetWindow(std::shared_ptr<Window> window) override;
    void Show() override;
    void Hide() override;

protected:
    void AsyncCall(std::function<void ()>) override;
    void onKeypadResize(int width, int height) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;

    void Redraw();

private:
    std::shared_ptr<Navitab> core;
    std::shared_ptr<Window> window;
    std::unique_ptr<ImageRectangle> image;
    bool dirty; // this is to prevent excess redrawing if nothing has changed

};

} // namespace navitab