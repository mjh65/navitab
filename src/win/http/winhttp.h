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
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "navitab/window.h"
#include "navitab/toolbar.h"
#include "navitab/modebar.h"
#include "navitab/doodler.h"
#include "navitab/keypad.h"
#include "navitab/deferred.h"
#include "navitab/logger.h"

namespace navitab {

class HtmlServer;
class TextureBuffer;
class CommandHandler;

class WindowHTTP : public std::enable_shared_from_this<WindowHTTP>,
                   public Window, public PartPainter, public WindowControls,
                   public Toolbar, public Modebar, public Doodler, public Keypad,
                   protected DeferredJobRunner<int>
{
public:
    WindowHTTP();
    ~WindowHTTP();

    // Implementation of the Window interface
    void Connect(std::shared_ptr<CoreServices> core) override;
    void Disconnect() override;
    void EventLoop() override;

    // Implementation of the PartPainter interface
    void Paint(int part, const FrameBuffer* src, const std::vector<FrameRegion>& regions) override;

    // Implementation of the WindowControls interface
    void Brightness(int percent) override;
    std::shared_ptr<Toolbar> GetToolbar() override { return shared_from_this(); }
    std::shared_ptr<Modebar> GetModebar() override { return shared_from_this(); }
    std::shared_ptr<Doodler> GetDoodler() override { return shared_from_this(); }
    std::shared_ptr<Keypad> GetKeypad() override { return shared_from_this(); }

    // Implementation of the Toolbar interface
    void SetStausInfo(std::string s) override;
    void SetEnabledTools(int selectMask) override;

    // Implementation of the Modebar interface
    void SetHighlighted(int selectMask) override;

    // Implementation of the Doodler interface
    void Enable() override;
    void Disable() override;

    // Implementation of the Keypad interface
    void Show() override;
    void Hide() override;

    // Encode a BMP image of the canvas for the panel
    unsigned EncodeBMP(std::vector<unsigned char> &png);
    // Encode the status for the panel
    std::string EncodeStatus();
    // Encode the controls settings for the panel
    std::string EncodeControls();

    void mouseEvent(int x, int y, int b);
    void wheelEvent(int x, int y, int d);
    void panelResize(int w, int h);
    void modeSelect(int m);
    void toolClick(int t);

    // Command handler has finished
    void Finish() { RunLater([this]() {onFinish();}, (void *)nullptr); }

protected:
    // ======================================================================
    // Implementation of DeferredJobRunner (via several other intermediate base classes)
    void RunLater(std::function<void ()>, void* s = nullptr) override;
    void RunLater(std::function<void ()>, int* s = nullptr) override;

    void onFinish() { running = false; }

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Settings> prefs;
    std::shared_ptr<WindowPart> canvas;
    std::unique_ptr<TextureBuffer> image;
    std::unique_ptr<HtmlServer> server;
    std::unique_ptr<CommandHandler> commands;
    std::mutex paintMutex;

    int winWidth;
    int winHeight;

    float brightness;

    bool                                running;
    std::queue<std::function<void()>>   jobs;
    std::condition_variable             qsync;
    std::mutex                          qmutex;
};

}
