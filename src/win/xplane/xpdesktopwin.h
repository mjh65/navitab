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

namespace navitab {

// XPDesktopWindow manages Navitab's desktop (or popped out) window in XPlane.

class XPDesktopWindow : public XPlaneWindow
{
public:
    XPDesktopWindow();
    ~XPDesktopWindow();

    // Implementation of navitab::XPlaneWindow
    void Create(std::shared_ptr<CoreServices> core) override;
    void Destroy() override;
    void Reset() override;

private:
    std::pair<int, int> screenCentre(int& l, int& t, int& r, int& b);

    void onDraw();
    int onLeftClick(int x, int y, XPLMMouseStatus status);
    int onMouseWheel(int x, int y, int wheel, int clicks);
    void onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus);
    XPLMCursorStatus getCursor(int x, int y) { return xplm_CursorDefault; }

private:
    int winLeft, winTop;
    bool winPoppedOut;
    int winResizePollTimer;

};

} // namespace navitab
