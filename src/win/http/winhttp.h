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

class PanelServer;
class TextureBuffer;

class WindowHTTP : public std::enable_shared_from_this<WindowHTTP>,
                   public Window, public PartPainter, public WindowControls
{
public:
    WindowHTTP();
    ~WindowHTTP();

    // Implementation of the Window interface
    void Connect(std::shared_ptr<CoreServices> core) override;
    void Disconnect() override;
    int EventLoop(int maxLoops) override;

    // Implementation of the PartPainter interface
    void Paint(int part, const FrameBuffer* src, const std::vector<FrameRegion>& regions) override;

    // Implementation of the WindowControls interface
    void Brightness(int percent) override;

    // encode a BMP image of the canvas for the http client
    unsigned encodeBMP(std::vector<unsigned char> &png);
    
private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Settings> prefs;
    std::shared_ptr<WindowPart> canvas;
    std::unique_ptr<TextureBuffer> image;
    std::unique_ptr<PanelServer> server;
    std::mutex paintMutex;

    int winWidth;
    int winHeight;

    float brightness;
};

}
