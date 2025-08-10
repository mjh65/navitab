/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <lvgl.h>
#include "navitab/deferred.h"

namespace navitab {
    class ImageRegion;
}

namespace lvglkit {

// Display wraps an LVGL display. Each of the Window parts uses one of these.

class Display
{
public:
    struct Updater {
        virtual void Update(navitab::ImageRegion r, uint32_t* pixels) = 0;
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
    unsigned pendingCalls;
    std::unique_ptr<std::thread> looper;
    uint32_t nextTimer;
    std::mutex nextTimerMutex;

};


} // namespace lvglkit
