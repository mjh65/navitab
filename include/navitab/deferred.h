/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <functional>

namespace navitab {

/**
 * @brief DeferredJobRunner classes have the ability to run a job later.
 * @details RunLater() allows a method call to be wrapped in a 
 * lambda closure and scheduled for later execution on the implementing
 * classes thread. This can be used to avoid blocking of high-priority
 * threads when delivering events (ie from window UI or simulator), or to
 * split execution into smaller chunks for convenience.
 */
template<class SIGNATURE = void>
struct DeferredJobRunner
{
    // Callbacks are wrapped in RunLater() to avoid stalling the UI.
    virtual void RunLater(std::function<void ()>, SIGNATURE*s = nullptr) = 0;
};


} // namespace navitab
