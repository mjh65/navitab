/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <string>
#include <functional>
#include "navitab/deferred.h"
#include "navitab/window.h"

// The Toolbar class represents the toolbar which is drawn across the top
// of the window. It is a fixed height, and displays some current status
// information on the left hand side, and some tool action icons from the
// right hand side which trigger behaviours in the currently active mode/app.

namespace navitab {

struct Window;
struct Location;

enum ClickableTool {
    MENU,
    COG,
    AFFIRM,
    STOP,
    CANCEL,
    REDUCE,
    CENTRE,
    MAGNIFY,
    LAST,
    RIGHT,
    LEFT,
    FIRST,
    ROTATEC,
    ROTATEA,
    BOTTOM,
    DOWN,
    UP,
    TOP,
    kNumTools
};

// The Toolbar2Core interface is used by the UI's toolbar to send user tool
// clicks to the Navitab core for the currently active screen.

struct Toolbar2Core : public DeferredJobRunner<>
{
    // UI-triggered events notified to the Navitab core for further handling

    void PostToolClick(ClickableTool t) {
        RunLater([this, t]() { onToolClick(t); });
    }

protected:
    // Called when a tool icon is clicked
    virtual void onToolClick(ClickableTool) = 0;
};

// The Toolbar interface defines the services that this part of the UI window
// provides to the Navitab core.

class Toolbar : public WindowPart
{
public:
    Toolbar() : WindowPart(TOOLBAR) { }
    virtual ~Toolbar() = default;

    // APIs called from the Navitab core
    virtual void SetStausInfo(int zt, int fps, const Location& l) = 0;
    virtual void SetActiveTools(int selectMask) = 0;
    virtual void SetRepeatingTools(int selectMask) = 0;
};

} // namespace navitab
