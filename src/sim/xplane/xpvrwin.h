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

#include "xpwin.h"
#include <XPLM/XPLMDisplay.h>

namespace navitab {

// XPVRWindow manages Navitab's window in XPlane.

class XPVRWindow : public XPlaneWindow
{
public:
    XPVRWindow();
    ~XPVRWindow();

    // Implementation of navitab::Window
    int FrameRate() override;

    // Implementation of navitab::XPlaneWindow
    void Create(std::shared_ptr<Preferences> prefs, std::shared_ptr<WindowEvents> core) override;
    void Destroy() override;
    void Show() override;
    void Recentre() override;

private:
    void onDraw();
    int onLeftClick(int x, int y, XPLMMouseStatus status);
    int onRightClick(int x, int y, XPLMMouseStatus status);
    int onMouseWheel(int x, int y, int wheel, int clicks);
    void onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus);
    XPLMCursorStatus getCursor(int x, int y) { return xplm_CursorDefault; }

private:

};

} // namespace navitab
