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

#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <lvgl.h>
#include "navitab/deferred.h"

namespace navitab {
    class FrameRegion;
}

namespace lvglkit {

// Display wraps an LVGL display. Each of the Window parts uses one of these.

class Display
{
public:
    struct Updater {
        virtual void Update(navitab::FrameRegion r, uint32_t* pixels) = 0;
    };
    virtual void Resize(int w, int h, uint32_t* buffer) = 0;
    virtual lv_display_t* GetHandleLVGL() = 0;

    virtual void Select() const = 0;
};

// RefCounter is used to ensure all LVGL resources have been deleted before
// calling lv_deinit()

class RefCounter
{
protected:
    RefCounter();
    virtual ~RefCounter();
    static int libRefs;
};

// The Manager provides the API for creating various top-level LVGL objects,
// and runs the UI update loop.

class Manager : protected RefCounter
{
public:
    Manager(std::shared_ptr<navitab::DeferredJobRunner<int>>);
    ~Manager();

    std::shared_ptr<Display> MakeDisplay(Display::Updater *);

protected:
    void RunLVGL();
    void DoTimerHandler();

private:
    std::shared_ptr<navitab::DeferredJobRunner<int>> core;
    bool running;
    std::unique_ptr<std::thread> looper;
    uint32_t nextTimer;
    std::mutex nextTimerMutex;

};


} // namespace lvglkit
