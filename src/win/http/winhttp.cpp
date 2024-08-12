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

#include <cassert>
#include <memory>
#include <fmt/core.h>
#include "winhttp.h"
#include "navitab/core.h"

std::shared_ptr<navitab::Window> navitab::Window::Factory()
{
    return std::make_shared<navitab::WindowHTTP>();
}

namespace navitab {

WindowHTTP::WindowHTTP()
:   LOG(std::make_unique<logging::Logger>("winmsfs")),
    winWidth(WIN_STD_WIDTH),
    winHeight(WIN_STD_HEIGHT),
    brightness(1.0f)
{
}

WindowHTTP::~WindowHTTP()
{
}

void WindowHTTP::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->GetSettingsManager();
    canvas->SetPainter(shared_from_this());
    canvas->PostResize(winWidth - MODEBAR_WIDTH, winHeight - TOOLBAR_HEIGHT);
    // TODO - start simple web server
}

void WindowHTTP::Disconnect()
{
    // TODO - shutdown web server
    canvas.reset();
    prefs.reset();
    core.reset();
}

int WindowHTTP::EventLoop(int maxLoops)
{
    // TODO - deal with events received from the panel via the web server
    return 0;
}

void WindowHTTP::Paint(int part, const FrameBuffer* src, const std::vector<FrameRegion>& regions)
{
#if 0
    assert(part == CANVAS);
    // This function is called from the core thread.
    const std::lock_guard<std::mutex> lock(imageMutex);
    std::unique_ptr<ImageRectangle> returnedImage;
    if (canvasImage) canvasImage->Reset();
    returnedImage = std::move(canvasImage);
    canvasImage = std::move(newImage);
    return returnedImage;
#endif
}

void WindowHTTP::Brightness(int percent)
{
    UNIMPLEMENTED(__func__);
}


} // namespace navitab
