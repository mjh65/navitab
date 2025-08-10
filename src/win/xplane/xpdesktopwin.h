/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
