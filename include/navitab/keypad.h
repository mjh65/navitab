/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <functional>
#include "navitab/deferred.h"
#include "navitab/window.h"

// The Keypad class represents an onscreen keyboard. This is a translucent
// overlay on the canvas that generates key codes in response to mouse-clicks.
// The keypad can be displayed/hidden through the mode bar, or by the currently
// running screen if it requires it.

namespace navitab {

struct Window;

// The Keypad2Core interface is used by the UI keypad to send keyboard codes
// to the Navitab core for the currently active screen.

struct Keypad2Core : public DeferredJobRunner<>
{
    // UI-triggered events notified to the Navitab core for further handling
    void PostKeypadEvent(int k) {
        RunLater([this, k]() { onKeypadEvent(k); });
    }

protected:
    // Called when key events occur.
    virtual void onKeypadEvent(int code) = 0;
};

// The Keypad interface defines the services that the UI keypad provides to
// the Navitab core.

class Keypad : public WindowPart
{
public:
    Keypad() : WindowPart(KEYPAD) { }
    virtual ~Keypad() = default;

    // APIs called from the Navitab core (sync call is OK)
    virtual void ShowKeypad() = 0;
    virtual void HideKeypad() = 0;
    // TBD: other stuff, like setting any multi-character keys (nearest airports etc)

};

} // namespace navitab
