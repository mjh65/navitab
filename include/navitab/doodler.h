/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <functional>
#include "navitab/deferred.h"
#include "navitab/window.h"

// The Doodler class represents the user's doodling area. This is a transparent
// overlay to the canvas that can be drawn or typed on when it is activated in the
// mode bar.

namespace navitab {

struct Window;

struct Doodler2Core : public DeferredJobRunner<>
{
    // The doodler doesn't really interact much with the rest of Navitab, so there's
    // nothing in here. But it is needed to extend the DeferredJobRunner.
};

class Doodler : public WindowPart
{
public:
    Doodler() : WindowPart(DOODLER) { }
    virtual ~Doodler() = default;

    // API calls from Navitab core (these will be triggered from the Modebar click handler)
    virtual void EnableDoodler() = 0;
    virtual void DisableDoodler() = 0;
};

} // namespace navitab
