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

#include <memory>
#include "navitab/doodler.h"
#include "navitab/logger.h"

namespace navitab {

class Navitab;
class ImageRectangle;

// The CoreToolbar class implements the toolbar which is drawn across the top
// of the window. It regenerates an ImageRect whenever some part of it changes
// and provides this to the window to draw.
    
class CoreDoodler : public Doodler
{
public:
    CoreDoodler(std::shared_ptr<DoodlerEvents> core);
    ~CoreDoodler();

    // APIs called from the application/plugin

    // APIs called from the window
    void SetWindow(std::shared_ptr<Window> window) override;

protected:
    void AsyncCall(std::function<void ()>) override;
    void onEnable() override;
    void onDisable() override;
    void onDoodlerResize(int width, int height) override;
    void onMouseEvent(int x, int y, bool l, bool r) override;
    void onKeyEvent(int c) override;

    void Redraw();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<DoodlerEvents> core;
    std::shared_ptr<Window> window;
    std::unique_ptr<ImageRectangle> image;
    bool enabled;
    bool dirty; // this is to prevent excess redrawing if nothing has changed

};

} // namespace navitab
