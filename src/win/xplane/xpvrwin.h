/* This file is part of the Navitab project. See the README and LICENSE for details. */

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

    // Implementation of navitab::XPlaneWindow
    void Create(std::shared_ptr<CoreServices> core) override;
    void Destroy() override;
    void Reset() override;

private:
    void onDraw();
    int onLeftClick(int x, int y, XPLMMouseStatus status);
    int onMouseWheel(int x, int y, int wheel, int clicks);
    void onKey(char key, XPLMKeyFlags flags, char vKey, int losingFocus);
    XPLMCursorStatus getCursor(int x, int y) { return xplm_CursorDefault; }

private:
    int winResizePollTimer;

};

} // namespace navitab
