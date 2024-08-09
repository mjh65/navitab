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
#include <mutex>
#include <XPLM/XPLMDisplay.h>
#include "navitab/window.h"
#include "navitab/logger.h"

namespace navitab {

class TextureBuffer;

// XPlaneWindow extends Navitab Window to provide a common base class for
// XP desktop and VR windows. This allows the XP plugin to swap between these
// specialisations when the simulation enters and leaves VR mode.

class XPlaneWindow : public std::enable_shared_from_this<XPlaneWindow>,
                     public Window, public WindowControl, public PartPainter
{
public:
    XPlaneWindow(const char *logId);
    ~XPlaneWindow();

    // Implementation of Window, 
    int EventLoop(int maxLoops) override { return 0; } // null implementation, not used

    // Implementation of WindowControl, common to desktop and VR
    void Brightness(int percent) override;

    // Implementation of PartPainter, common to desktop and VR
    void RefreshPart(int part, const ImageRectangle* src, const std::vector<Region>& regions) override;

    // XPlane addition requiring specific implementations for desktop and VR windows
    virtual void Create(std::shared_ptr<CoreServices> core);
    virtual void Destroy() = 0;
    virtual void Reset() = 0; // Reset size and reposition centrally (in VR attaches to HMD or controller)

    // Common behaviour
    void Show();
    void CheckVitalSigns();
    bool isActive();

protected:
    // Implementation of navitab::Window, common to desktop and VR
    // these get called internally from the Create/Destroy functions.
    void Connect(std::shared_ptr<CoreServices> core) override;
    void Disconnect() override;

    void RenderContent();
    void RenderPart(int part, int left, int top, int right, int bottom);

    void ProdWatchdog();
    bool UpdateWinGeometry(); // returns true if the size changed
    void ResizeNotifyAll(int w, int h);
    bool isVisible() const { return winVisible; }
    void ScreenToWindow(int& x, int& y);

protected:
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Preferences> prefs;
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<WindowPart> parts[WindowPart::TOTAL_PARTS];

    XPLMWindowID winHandle;
    int winWidth, winHeight;

private:
    std::unique_ptr<TextureBuffer> partImages[WindowPart::TOTAL_PARTS];
    std::mutex imageMutex;

private:
    int winDrawWatchdog;
    int wgl, wgt, wgr, wgb; // most recently observed window geometry
    bool winVisible;
    float brightness;

protected:
    // singleton data, initialised once and reused when switching modes
    static std::vector<int> xpTextureIds;

};


} // namespace navitab
