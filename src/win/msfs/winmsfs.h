/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
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
#include <mutex>
#include "navitab/window.h"
#include "navitab/logger.h"

namespace navitab {

class TextureBuffer;

class WindowMSFS : public std::enable_shared_from_this<WindowMSFS>,
                   public Window, public PartPainter, public WindowControl
{
public:
    WindowMSFS();
    ~WindowMSFS();

    // Implementation of the Window interface
    void Connect(std::shared_ptr<CoreServices> core) override;
    void Disconnect() override;
    int EventLoop(int maxLoops) override;

    // Implementation of the PartPainter interface
    void RefreshPart(int part, const ImageRectangle* src, const std::vector<Region>& regions) override;

    // Implementation of the WindowControl interface
    void Brightness(int percent) override;

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Preferences> prefs;
    std::shared_ptr<WindowPart> canvas;

    std::unique_ptr<TextureBuffer> canvasImage;
    std::mutex imageMutex;

    int winWidth;
    int winHeight;

    float brightness;
};

}
