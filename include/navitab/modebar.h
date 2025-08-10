/* This file is part of the Navitab project. See the README and LICENSE for details. */


#pragma once

#include <memory>
#include <functional>
#include "navitab/deferred.h"
#include "navitab/window.h"

// The Modebar class represents the mode/app chooser which is drawn down the
// left hand side of the window. It is a fixed size, and contains a set of
// icons which represent the different modes of Navitab. Clicking on an icon
// changes the mode.

namespace navitab {

struct Window;

// The Modebar2Core interface is used by the UI's modebar to post updates to
// the Navitab core.

struct Modebar2Core : public DeferredJobRunner<>
{
    // UI-triggered events notified to the Navitab core for further handling

    enum Mode {
        // maybe we don't need these as bitmaskable?
        NONE        = 0,
        ABOUT_HELP  = 0b1,
        MAP         = 0b10,
        AIRPORT     = 0b100,
        ROUTE       = 0b1000,
        DOC_VIEWER  = 0b10000,
        SETTINGS    = 0b100000,
        DOODLER     = 0b1000000,
        KEYPAD      = 0b10000000
    };

    void PostAppSelect(Mode m) {
        RunLater([this, m]() { onAppSelect(m); });
    }
    void PostDoodlerToggle() {
        RunLater([this]() { onDoodlerToggle(); });
    }
    void PostKeypadToggle() {
        RunLater([this]() { onKeypadToggle(); });
    }

protected:
    // Called when an app icon is clicked in the mode bar
    virtual void onAppSelect(Mode) = 0;
    // Called when the doodler icon is toggled in the mode bar
    virtual void onDoodlerToggle() = 0;
    // Called when the keypad is toggled in the mode bar
    virtual void onKeypadToggle() = 0;
};

// The Modebar interface defines the services that the UI window provides to
// the Navitab core.

class Modebar : public WindowPart
{
public:
    Modebar() : WindowPart(MODEBAR) { }
    virtual ~Modebar() = default;

    // APIs called from the Navitab core
    virtual void SetHighlightedModes(int selectMask) = 0;
};

} // namespace navitab
