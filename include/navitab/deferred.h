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
